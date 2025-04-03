// Fichier: main.cpp
// Description: Programme principal pour tester la gestion de la grand-voile

#include <Arduino.h>
#include "OuvertureGrandVoile.h"


Ouverture_Grand_Voile GV; // Instance de la classe

void setup() {

    //pinMode(D9, OUTPUT);
    pinMode(BROCHE_MOTEUR, OUTPUT);
    Serial.begin(9600); // Initialisation de la communication série
}

void loop() {
    static float capteurBoussole = 0;   // Simulation de la valeur du capteur boussole
    static float capteurGirouette = 0;  // Simulation de la valeur du capteur girouette
    static float capteurIMU = 5.0;         // Simulation de la valeur du capteur IMU

    //digitalToggle(D9);

    GV.System_GV(capteurBoussole, capteurGirouette, capteurIMU); // Exécution du système GV
    delay(2000); // Pause de 2 secondes avant la prochaine itération

    capteurGirouette = 90;
    GV.System_GV(capteurBoussole, capteurGirouette, capteurIMU); // Exécution du système GV
    delay(2000); // Pause de 2 secondes avant la prochaine itération

    Serial.println(capteurGirouette);

    capteurGirouette = 100;
    GV.System_GV(capteurBoussole, capteurGirouette, capteurIMU); // Exécution du système GV
    delay(2000); // Pause de 2 secondes avant la prochaine itération

    Serial.println(capteurGirouette);

    capteurGirouette = 250;
    GV.System_GV(capteurBoussole, capteurGirouette, capteurIMU); // Exécution du système GV
    delay(2000); // Pause de 2 secondes avant la prochaine itération
    Serial.println(capteurGirouette);


    capteurGirouette = 0;
    GV.System_GV(capteurBoussole, capteurGirouette, capteurIMU); // Exécution du système GV
    delay(2000); // Pause de 2 secondes avant la prochaine itération
    Serial.println(capteurGirouette);


    capteurGirouette = 355;
    GV.System_GV(capteurBoussole, capteurGirouette, capteurIMU); // Exécution du système GV
    delay(2000); // Pause de 2 secondes avant la prochaine itération

    Serial.println(capteurGirouette);

    capteurGirouette = 225;
    GV.System_GV(capteurBoussole, capteurGirouette, capteurIMU); // Exécution du système GV
    delay(2000); // Pause de 2 secondes avant la prochaine itération

    Serial.println(capteurGirouette);

}
