#ifndef REGLAGE_GV_H
#define REGLAGE_GV_H

#include <Arduino.h>
#include <Servo.h>

// Définition du servo
extern Servo monServo;

// Fonctions
char attendreReponse(void);
void demarrage(void);
void init_GV(void);
void reglerMin(void);
void reglerMax(void);
void boucle_init_GV(void);
void Reglage_GV(void);

#endif // REGLAGE_GV_H
