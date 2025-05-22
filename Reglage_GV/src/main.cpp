// Inclusion de la bibliothèque Servo
#include <Servo.h>
#include <Arduino.h>
#include "ReglageGV.h"


// Fonction d'initialisation du programme
void setup() {
    //Serial.begin(115200); // Initialisation de la liaison série
}

// Boucle principale
void loop() {
    Reglage_GV();
}
