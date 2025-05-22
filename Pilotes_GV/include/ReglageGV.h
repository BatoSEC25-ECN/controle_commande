#ifndef REGLAGE_GV_H
#define REGLAGE_GV_H

#include <Arduino.h>
#include <Servo.h>

// Définition du servo
extern Servo monServo;

// Déclaration des variables de réglage de GV
extern float valeurRef;      // Valeur de référence en millisecondes (modifiable par l'utilisateur)
extern float pas;           // Pas d'incrémentation/décrémentation (modifiable par l'utilisateur)
extern float valeurMin, valeurMax; // Stockage des valeurs min et max
extern const int pinServo;           // Broche du servo sur la carte L432KC

// Fonctions
char attendreReponse(void);
void demarrage(void);
void init_GV(void);
void reglerMin(void);
void reglerMax(void);
void boucle_init_GV(void);
void Reglage_GV(void);

#endif //REGLAGE_GV_H
