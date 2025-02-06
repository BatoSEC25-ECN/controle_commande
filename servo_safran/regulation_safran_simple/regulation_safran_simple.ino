#include <Servo.h>
#include <QMC5883LCompass.h>

// Définition de la broche pour le servomoteur
const int servoPin = 12;

// Création d'un objet pour le servomoteur
Servo myservo;

// Création d'un objet pour la boussole
QMC5883LCompass compass;

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

void setup() {
  // Configuration de Serial2 avec 9600 bauds, 8 data bits, aucune parité, 1 stop bit pour le module Xbee
  Serial2.begin(9600, SERIAL_8N1);                 
  Serial.begin(115200);

  // Initialisation de la boussole
  compass.init();
  
  // Attache du servomoteur à la broche
  myservo.attach(servoPin);
}

void loop() {
  /////////////////////////////////////////////////////////////////
  ///// ----- Partie régulation / asservissement de cap ----- /////
  /////////////////////////////////////////////////////////////////
  // Lecture de la boussole
  compass.read();
  
  // Récupération de l'azimuth actuel
  int currentAzimuth = compass.getAzimuth() + compass_offset;
  
  // Calcul de l'erreur entre le cap de consigne et le cap actuel
  int error = desiredCap - currentAzimuth;
  
  // Limitation de l'erreur à 360 degrés (pour éviter des erreurs trop importantes)
  if (error > 180) {
    error -= 360;
  } else if (error < -180) {
    error += 360;
  }
  
  // Terme proportionnel
  int correction = (int)(kp * error) + offset;

  // Terme intégral
  errors_sum += error * dt;
  correction += (int)(ki * errors_sum);
  
  // Limitation de la correction à la plage de déplacement du servomoteur (0 à 180 degrés)
  if (correction > 180) {
    correction = 180;
  } else if (correction < 0) {
    correction = 0;
  }

  // Affichage des valeurs dans le moniteur série
  Serial.print("Azimuth actuel : "); Serial.print(currentAzimuth); Serial.println("°");
  Serial.print("Erreur : "); Serial.print(error); Serial.println("°");
  Serial.print("Correction : "); Serial.print(correction); Serial.println("°");
  
  // Envoi de la correction au servomoteur
  myservo.write(correction);

  // fréquence de régulation
  delay(dt); 

  ///////////////////////////////////////////////////////
  ///// ----- Partie réception de nouveau cap ----- /////
  ///////////////////////////////////////////////////////
  String receivedString = ""; // Chaîne pour stocker les données reçues

  if (Serial2.available()) {
    // Lire la chaîne complète envoyée via Serial2
    while (Serial2.available()) {
      char c = Serial2.read();
      receivedString += c; // Construire la chaîne reçue
      delay(10); // Petit délai pour s'assurer que tous les caractères arrivent
    }

    Serial.println("String reçue : " + receivedString); // Afficher la chaîne reçue entière

    // Rechercher "cap " dans la chaîne reçue
    int index = receivedString.indexOf("cap ");
    if (index != -1) { // Si "cap " est trouvé
      // Extraire la portion qui suit "cap "
      String capValue = receivedString.substring(index + 4); // Extraire après "cap "

      // Si besoin, convertir en entier ou autre type
      desiredCap = capValue.toInt(); // Convertir en entier
      Serial.println("Valeur cap convertie : " + String(desiredCap));
    }

    // Rechercher "compass " dans la chaîne reçue
    index = receivedString.indexOf("compass ");
    if (index != -1) {
      String compassValue = receivedString.substring(index + 8); // Extraire après "compass "

      // Si besoin, convertir en entier ou autre type
      compass_offset = compassValue.toInt(); // Convertir en entier
      Serial.println("Valeur boussole convertie : " + String(compass_offset));
    }

    // Rechercher "coeffP " dans la chaîne reçue
    index = receivedString.indexOf("coeffP ");
    if (index != -1) {
      String PValue = receivedString.substring(index + 7); // Extraire après "coeffP "

      // Si besoin, convertir en entier ou autre type
      kp = PValue.toInt(); // Convertir en entier
      Serial.println("Valeur coeff P convertie : " + String(kp));
    }

    // Rechercher "coeffI " dans la chaîne reçue
    index = receivedString.indexOf("coeffI ");
    if (index != -1) {
      String IValue = receivedString.substring(index + 7); // Extraire après "coeffI "

      // Si besoin, convertir en entier ou autre type
      ki = IValue.toInt(); // Convertir en entier
      Serial.println("Valeur coeff I convertie : " + String(ki));
    }
  }
}
