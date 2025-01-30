#include "I2Cdev.h"
// #include "MPU6050.h" // not necessary if using MotionApps include file
#include <SD.h>
#include <TinyGPSPlus.h>
#include <QMC5883LCompass.h>
#include "MPU6050_6Axis_MotionApps20.h"

// --- MPU6050 --- //
MPU6050 mpu;

// MPU control/status var
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 acc;        // [x, y, z]            accel sensor measurements
VectorInt16 gyro;       // [x, y, z]            gyro sensor measurements

// --- GPS --- //
static const uint32_t GPSBaud = 9600;  // GPS baud rate
TinyGPSPlus gps;

// --- Compass --- //
QMC5883LCompass compass;

// --- SD Card --- //
const int SS_pin = 17; // Pin pour le module SD
File dataFile;

void setup() {
  // Initialisation du port série
  Serial.begin(115200);

  // --- MPU6050 --- //
  // Initialisation du MPU6050 (centrale inertielle)
  Wire.begin();
  Serial.println("Initialisation du MPU6050...");
  mpu.initialize();
  while (!mpu.testConnection()) {
    Serial.println("Erreur de connexion avec le MPU6050.");
  }

  // load and configure the DMP, 
  Serial.println(F("Initialisation du Digital Motion Processer..."));
  mpu.dmpInitialize(); // Digital Motion Processer initialization

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // Calibration Time: generate offsets and calibrate our MPU6050
  mpu.CalibrateAccel(6);
  mpu.CalibrateGyro(6);
  mpu.PrintActiveOffsets();

  // turn on the DMP, now that it's ready
  mpu.setDMPEnabled(true);

  // --- SD Card --- //
  // Ouvrir/Créer le fichier CSV
  SD.begin(SS_pin);
  String file_name = "DATA" + String(millis()) + ".CSV";
  dataFile = SD.open(file_name, FILE_WRITE);
  dataFile.println("Timestamp,Acceleration_X,Acceleration_Y,Acceleration_Z,Rate_X,Rate_Y,Rate_Z,Quaternion_W,Quaternion_X,Quaternion_Y,Quaternion_Z,Mag_X,Mag_Y,Mag_Z,Azimuth,Latitude,Longitude,Altitude,Speed,Date,Time");
  dataFile.flush();

  // --- GPS --- //
  // Connection avec le module GPS
  Serial1.begin(GPSBaud);

  // --- Compass --- //
  // Initialisation de la boussole
  compass.init();
}

void loop() {
  if (dataFile) {
    // --- MPU6050 --- //
    mpu.dmpGetCurrentFIFOPacket(fifoBuffer);  // get the latest packet
    mpu.dmpGetQuaternion(&q, fifoBuffer);     // get the quaternion through the latest packet of the FIFO buffer
    mpu.dmpGetAccel(&acc, fifoBuffer);        // get the acceleration on the 3 axis
    mpu.dmpGetGyro(&gyro, fifoBuffer);        // get the angle rate on the 3 axis

    // --- GPS --- //
    // Lecture des données GPS
    while (Serial1.available()) {
      gps.encode(Serial1.read());
    }

    // Variables pour les données GPS
    float latitude = 0.0, longitude = 0.0, altitude = 0.0, speed = 0.0;
    int hour, minute, second, centisecond = 0;
    int day, month, year = 0;

    // Vérification des données GPS
    if (gps.location.isValid()) {
      latitude = gps.location.lat();   // Latitude en degrés
      longitude = gps.location.lng();  // Longitude en degrés
    } else {
      latitude = -1.0;  // Valeurs par défaut (ex: 0.0 ou -1, selon vos besoins)
      longitude = -1.0;
    }

    if (gps.altitude.isValid()) {
      altitude = gps.altitude.meters(); // Altitude en mètres
    } else {
      altitude = -1.0; // Altitude non disponible
    }

    if (gps.speed.isValid()) {
      speed = gps.speed.kmph(); // Vitesse en km/h
    } else {
      speed = -1.0; // Vitesse non disponible
    }

    if (gps.time.isValid()) {
      hour = gps.time.hour() + 1; // UTC+1 in France
      minute = gps.time.minute();
      second = gps.time.second();
      centisecond = gps.time.centisecond();
    } else {
      hour = -1;
      minute = -1;
      second = -1;
      centisecond = -1;
    }

    if (gps.date.isValid()) {
      day = gps.date.day();
      month = gps.date.month();
      year = gps.date.year();
    } else {
      day = -1;
      month = -1;
      year = -1;
    }

    // --- Compass --- //
    compass.read();
    
    int mx = compass.getX();
    int my = compass.getY();
    int mz = compass.getZ();
    int azimuth = compass.getAzimuth();

    // --- SD card --- //
    // Écriture des données dans le fichier CSV
    dataFile.print(millis()); // Temps écoulé en ms
    dataFile.print(',');
    dataFile.print(acc.x); dataFile.print(',');
    dataFile.print(acc.y); dataFile.print(',');
    dataFile.print(acc.z); dataFile.print(',');
    dataFile.print(gyro.x); dataFile.print(',');
    dataFile.print(gyro.y); dataFile.print(',');
    dataFile.print(gyro.z); dataFile.print(',');
    dataFile.print(q.w); dataFile.print(',');
    dataFile.print(q.x); dataFile.print(',');
    dataFile.print(q.y); dataFile.print(',');
    dataFile.print(q.z); dataFile.print(',');
    dataFile.print(mx); dataFile.print(',');
    dataFile.print(my); dataFile.print(',');
    dataFile.print(mz); dataFile.print(',');
    dataFile.print(azimuth); dataFile.print(',');
    dataFile.print(latitude, 6); dataFile.print(',');
    dataFile.print(longitude, 6); dataFile.print(',');
    dataFile.print(altitude, 2); dataFile.print(',');
    dataFile.print(speed, 2); dataFile.print(',');
    dataFile.print(String(day) + "/" + String(month) + "/" + String(year)); dataFile.print(',');
    dataFile.print(String(hour) + ":" + String(minute) + ":" + String(second) + "." + String(centisecond));
    dataFile.println();

    // Sauvegarde les données sur la carte SD
    dataFile.flush();

    // Debug terminal
    Serial.print("Acc: ");
    Serial.print(acc.x); Serial.print(", "); Serial.print(acc.y); Serial.print(", ");
    Serial.print(acc.z); 
    Serial.print(", Rate: "); 
    Serial.print(gyro.x); Serial.print(", "); Serial.print(gyro.y); Serial.print(", "); 
    Serial.print(gyro.z);
    Serial.print(", Quaternion: "); 
    Serial.print(q.w); Serial.print(", "); Serial.print(q.x); Serial.print(", "); 
    Serial.print(q.y); Serial.print(", "); Serial.print(q.z);
    Serial.print(" | Mag: ");
    Serial.print(mx); Serial.print(", "); Serial.print(my); Serial.print(", ");
    Serial.print(mz);
    Serial.print(" | Azim: ");
    Serial.print(azimuth);
    Serial.print(" | Time: " + String(hour) + "h" + String(minute) + "mn" + String(second) + "." + String(centisecond) + "s");
    Serial.print(", Lat: ");
    Serial.print(latitude, 6);
    Serial.print(", Lon: ");
    Serial.print(longitude, 6);
    Serial.print(", Alti: ");
    Serial.print(altitude, 2);
    Serial.print("m, Speed: ");
    Serial.print(speed, 2);
    Serial.println("km/h");

  } else {
    // Erreur d'ouverture du fichier
    Serial.println("Erreur : Impossible d'écrire dans le fichier CSV !");
  }

  delay(100); // Limite la fréquence d'échantillonnage
}
