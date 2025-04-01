#include <Servo.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

//--------------------------------------------------------------
// Pour activer les messages de debug, décommentez la ligne suivante
// #define DEBUG

#ifdef DEBUG
  #define DEBUG_PRINT(x)    Serial.print(x)
  #define DEBUG_PRINTLN(x)  Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif
//--------------------------------------------------------------

// Address of the CMPS12 compass on i2C
#define _i2cAddress 0x60

#define CONTROL_Register 0

#define BEARING_Register 2 
#define PITCH_Register 4 
#define ROLL_Register 5

#define MAGNET_X_Register  6
#define MAGNET_Y_Register  8
#define MAGNET_Z_Register 10

#define ACCELERO_X_Register 12
#define ACCELERO_Y_Register 14
#define ACCELERO_Z_Register 16

#define _Register_GYRO_X 18
#define _Register_GYRO_Y 20
#define _Register_GYRO_Z 22

#define ONE_BYTE 1
#define TWO_BYTES 2

//---------------------------------

int _bearing;
int nReceived;
byte _fine;
byte _byteHigh;
byte _byteLow;
char _pitch;
char _roll;

float accelx = 0;
float accely = 0;
float accelz = 0;
float _accelScale = 1.0f/100.f; // 1 m/s^2 = 100 LSB

float gyrox = 0;
float gyroy = 0;
float gyroz = 0;
float _gyroScale = 1.0f/16.f; // 1 Dps = 16 LSB

//---------------------------------

// Définition de la broche pour le servomoteur
const int servoPin = 3;

// Création d'un objet pour le servomoteur
Servo myservo;

int pulse_min = 1070 + 120;   // Minimum pulse width in microseconds (µs)
int pulse_max = 1690 + 120;   // Maximum pulse width in microseconds (µs)
int pulse_center = (pulse_max - pulse_min) / 2 + pulse_min; // Center position

// Cap de consigne (azimuth)
int desiredCap = -90; // à modifier pour changer le cap de consigne

// Offset boussole
int compass_offset = 0;

// Gain du correcteur : P & I (à ajuster pour optimiser la régulation)
float kp = 1; 
float ki = 10;

float errors_sum = 0;

// Période d'échantillonnage
const int dt = 50;

// Offset pour la correction (pour que -90° soit 0° et +90° soit 180°)
const int offset = 90;

///// SD card /////
File dataFile;

const int _MISO = 12;
const int _MOSI = 11;
const int _CS = 15;
const int _SCK = 10;

///// Xbee /////
const int XBee_reset_pin = 21;
const int XBee_rssi_pin  = 28;
const int XBee_dout_pin  = 1;
const int XBee_din_pin   = 0;

void initXBee() {
  pinMode(XBee_rssi_pin, INPUT);

  // **Warning**: DOUT should NOT be LOW during reset (else SPI only mode)!!
  pinMode(XBee_dout_pin, INPUT_PULLUP);
  pinMode(XBee_reset_pin, OUTPUT);
  digitalWrite(XBee_reset_pin, LOW); // pin active low
  delay(10);
  digitalWrite(XBee_reset_pin, HIGH); // pin active low

  pinMode(XBee_dout_pin, INPUT);
  Serial1.setRX(XBee_dout_pin);
  Serial1.setTX(XBee_din_pin);

  // Configuration de Serial1 avec 9600 bauds, 8 data bits, aucune parité, 1 stop bit pour le module Xbee
  Serial1.begin(9600, SERIAL_8N1);
}

void setup() {
  // Initialisation de la communication série avec le module Xbee
  initXBee();

#ifdef DEBUG
  // Debug : Initialisation du port série pour la communication avec le PC
  Serial.begin(115200);
  while (!Serial);
#endif
  DEBUG_PRINTLN("Initialisation du programme");

  // Initialize i2c network
  Wire.begin();

  // SD card initialization
  pinMode(13, INPUT_PULLUP); //connected SD card
  pinMode(14, INPUT_PULLUP); //connected SD card

  // Ensure the SPI pinout the SD card is connected to is configured properly
  SPI1.setRX(_MISO);
  SPI1.setTX(_MOSI);
  SPI1.setSCK(_SCK);

  // see if the card is present and can be initialized:
  if (!SD.begin(_CS,4000000,SPI1)) {
    DEBUG_PRINTLN("Card failed, or not present");
    // don't do anything more:
    return;
  } else {
    DEBUG_PRINTLN("card initialized.");
    String file_name = "DATALOG" + String(millis()) + ".CSV";
    dataFile = SD.open(file_name, FILE_WRITE);
    dataFile.println("Timestamp,Consigne_cap,Cap_reel,Offset_boussole,Coeff_P,Coeff_I,Correction");
    // Sauvegarde les données sur la carte SD
    dataFile.flush();
    DEBUG_PRINTLN("Fichier de données créé.");
  }

  // Attache du servomoteur à la broche
  myservo.attach(servoPin);
}

void loop() {
  /////////////////////////////////////////////////////////////////
  ///// ----- Partie régulation / asservissement de cap ----- /////
  /////////////////////////////////////////////////////////////////
  // Lecture de la boussole
  int bearing = getBearing();
  
  // Récupération de l'azimuth actuel
  int currentAzimuth = bearing + compass_offset;
  
  // Calcul de l'erreur entre le cap de consigne et le cap actuel
  int error = desiredCap - currentAzimuth;
  
  // Limitation de l'erreur à 360 degrés (pour éviter des erreurs trop importantes)
  if (error > 180) {
    error -= 360;
  } else if (error < -180) {
    error += 360;
  }
  
  // Terme proportionnel
  int correction = (int)(-kp * error) + offset;

  // Terme intégral
  errors_sum -= error * dt;
  correction -= (int)(ki * errors_sum);
  
  // Limitation de la correction à la plage de déplacement du servomoteur (0 à 180 degrés)
  if (correction > 180) {
    correction = 180;
  } else if (correction < 0) {
    correction = 0;
  }

  // Affichage des valeurs dans le moniteur série (debug)
  DEBUG_PRINT("Azimuth actuel : "); DEBUG_PRINT(currentAzimuth); DEBUG_PRINTLN("°");
  DEBUG_PRINT("Erreur : ");         DEBUG_PRINT(error);         DEBUG_PRINTLN("°");
  DEBUG_PRINT("Correction : ");     DEBUG_PRINT(correction - 90);    DEBUG_PRINTLN("°");
  
  // Calcul de la largeur d'impulsion en microsecondes en utilisant un produit en croix
  int pulseWidth = map(correction, 0, 180, pulse_min, pulse_max);

  // Envoi de la correction au servomoteur en microsecondes
  myservo.writeMicroseconds(pulseWidth);

  // fréquence de régulation
  delay(dt); 

  ///////////////////////////////////////////////////////
  ///// ----- Partie réception de nouveau cap ----- /////
  ///////////////////////////////////////////////////////
  String receivedString = ""; // Chaîne pour stocker les données reçues

  if (Serial1.available()) {
    // Lire la chaîne complète envoyée via Serial1
    while (Serial1.available()) {
      char c = Serial1.read();
      receivedString += c; // Construire la chaîne reçue
      delay(10); // Petit délai pour s'assurer que tous les caractères arrivent
    }

    DEBUG_PRINTLN("String reçue : " + receivedString);

    // Rechercher "cap " dans la chaîne reçue
    int index = receivedString.indexOf("cap ");
    if (index != -1) { // Si "cap " est trouvé
      // Extraire la portion qui suit "cap "
      String capValue = receivedString.substring(index + 4); // Extraire après "cap "

      // Si besoin, convertir en entier ou autre type
      desiredCap = capValue.toInt(); // Convertir en entier
      DEBUG_PRINTLN("Valeur cap convertie : " + String(desiredCap));
    }

    // Rechercher "compass " dans la chaîne reçue
    index = receivedString.indexOf("compass ");
    if (index != -1) {
      String compassValue = receivedString.substring(index + 8); // Extraire après "compass "

      // Si besoin, convertir en entier ou autre type
      compass_offset = compassValue.toInt(); // Convertir en entier
      DEBUG_PRINTLN("Valeur boussole convertie : " + String(compass_offset));
    }

    // Rechercher "coeffP " dans la chaîne reçue
    index = receivedString.indexOf("coeffP ");
    if (index != -1) {
      String PValue = receivedString.substring(index + 7); // Extraire après "coeffP "

      // Si besoin, convertir en entier ou autre type
      kp = PValue.toInt(); // Convertir en entier
      DEBUG_PRINTLN("Valeur coeff P convertie : " + String(kp));
    }

    // Rechercher "coeffI " dans la chaîne reçue
    index = receivedString.indexOf("coeffI ");
    if (index != -1) {
      String IValue = receivedString.substring(index + 7); // Extraire après "coeffI "

      // Si besoin, convertir en entier ou autre type
      ki = IValue.toInt(); // Convertir en entier
      DEBUG_PRINTLN("Valeur coeff I convertie : " + String(ki));
    }
  }
  if (dataFile) {
    ///////////////////////////////////////////////////////
    ///// ----------------- SD card ----------------- /////
    ///////////////////////////////////////////////////////
    DEBUG_PRINTLN("Ecriture des données sur la carte SD");

    // Écriture des données dans le fichier CSV
    dataFile.print(millis()); dataFile.print(','); // Temps écoulé en ms
    dataFile.print(desiredCap); dataFile.print(',');
    dataFile.print(currentAzimuth); dataFile.print(',');
    dataFile.print(compass_offset); dataFile.print(',');
    dataFile.print(kp); dataFile.print(',');
    dataFile.print(ki); dataFile.print(',');
    dataFile.print(correction - 90);
    dataFile.println();

    // Sauvegarde les données sur la carte SD
    dataFile.flush();
  }
}


// Fonction pour lire la valeur de la boussole
int16_t getBearing()
{
  // Begin communication with CMPS12
  Wire.beginTransmission(_i2cAddress);

  // Tell register you want some data
  Wire.write(BEARING_Register);

  // End the transmission
  int nackCatcher = Wire.endTransmission();

  // Return if we have a connection problem 
  if(nackCatcher != 0){return 0;}
  
  // Request 2 bytes from CMPS12
  nReceived = Wire.requestFrom(_i2cAddress , TWO_BYTES);

  // Something has gone wrong
  if (nReceived != TWO_BYTES) return 0;

  // Read the values
  _byteHigh = Wire.read(); 
  _byteLow = Wire.read();

  // Calculate full bearing
  _bearing = ((_byteHigh << 8) + _byteLow) / 10;
  
  return _bearing;
}
