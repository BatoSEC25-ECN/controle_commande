#include <Servo.h>
#include <QMC5883LCompass.h>

// Définition de la broche pour le servomoteur
const int servoPin = 12;

// Création d'un objet pour le servomoteur
Servo myservo;

// Création d'un objet pour la boussole
QMC5883LCompass compass;

// Cap de consigne (azimuth)
int desiredAzimuth = -90; // à modifier pour changer le cap de consigne

// Gain du correcteur P
const float kp = 1; // à ajuster pour optimiser la régulation

// Offset pour la correction (pour que -90° soit 0° et +90° soit 180°)
const int offset = 90;

void setup() {
  // Initialisation de la boussole
  compass.init();
  
  // Attache du servomoteur à la broche
  myservo.attach(servoPin);
}

void loop() {
  // Lecture de la boussole
  compass.read();
  
  // Récupération de l'azimuth actuel
  int currentAzimuth = compass.getAzimuth();
  
  // Calcul de l'erreur entre le cap de consigne et le cap actuel
  int error = desiredAzimuth - currentAzimuth;
  
  // Limitation de l'erreur à 360 degrés (pour éviter des erreurs trop importantes)
  if (error > 180) {
    error -= 360;
  } else if (error < -180) {
    error += 360;
  }
  
  // Calcul de la sortie du correcteur P
  int correction = (int)(kp * error) + offset;
  
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
  
  delay(50); // fréquence de régulation
}
