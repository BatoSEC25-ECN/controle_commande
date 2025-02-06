#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>



Servo monServo;  // Crée un objet Servo


void setup() {
  Serial.begin(9600);
  // Attache le servomoteur à la broche PA9 (D1 sur la Nucleo-L432KC)
  monServo.attach(PA9);
}

void loop() {
  // Ne rien faire dans la boucle principale

  Serial.println("fonctionne");

  monServo.write(70);   // Déplace le servomoteur à 0 degrés
  delay(500);         // Attend 0,1 seconde
  Serial.println("70°");
  monServo.write(120);  // Déplace le servomoteur à 90 degrés
  delay(500);         // Attend 0,1 seconde
  Serial.println("120");
  monServo.write(170); // Déplace le servomoteur à 180 degrés
  Serial.println("170°");
  delay(500); 
}
