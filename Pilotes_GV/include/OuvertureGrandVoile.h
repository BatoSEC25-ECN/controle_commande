// Fichier: OuvertureGrandVoile.h
// Description: Définition de la classe Ouverture_Grand_Voile et des constantes nécessaires

#ifndef OUVERTURE_GRAND_VOILE_H
#define OUVERTURE_GRAND_VOILE_H

#include <Arduino.h>
#include <Servo.h>

// Définition des modes de navigation
#define FACE_AUX_VENT 0
#define SOUS_LE_VENT 1

// Valeurs maximales pour le gîte et la position de la grand-voile
#define MAX_GITE 35 // Valeur max tolérée du gîte du bateau
#define POS_MAX_CHOQUE  1.32  // Position max choqué (ms)
#define POS_MIN_BORDE 1.80   // Position min bordé (ms)

// Angles de position de la Grand Voile et Babord
#define ANGLE_GV_MIN 0
#define ANGLE_GV_MAX 90
#define MIN_BABORD 40
#define MAX_BABORD 180
#define OUVERTURE_BABORD (MAX_BABORD-MIN_BABORD)
#define ORIENT_PROUE 0

//le temps d'impulsion est le plus grand pour l'angle le plus petit, et Inversement. Il faut donc en tenir Compte
#define CONV_ANGLE_TEMPS 90

//broche du moteur
#define BROCHE_MOTEUR D9

//transpose un repère de [0 ; 360] à un repère [-180 ; 180]
#define TRANSPO_REPERE 180

class Ouverture_Grand_Voile {
private:
    int Cap_Actuel;   // Cap actuel du bateau
    int Direction_VA; // Direction du vent apparent
    int Gite;         // Inclinaison du bateau
    int Angle_GV;     // Position angulaire de la grand-voile
    int Mode_Nav;       // Mode de navigation actuel
    Servo moteurGV;     // Moteur de la grand-voile
    int deltaAngle;     // Angle relatif en fonction de la proue et corrigé selon mon référentiel

    void Mode_Navigation();   // Détermine le mode de navigation
    void RG_Face_au_Vent();   // Règle la GV face au vent
    void RG_Sous_le_Vent();   // Règle la GV sous le vent
    void pos_moteur();        // Positionne la grand-voile selon l'angle calculé

public:
    void Init_GV(int Girouette, int IMU_Gite, int pinMoteur); 
    void System_GV(int Girouette, int IMU_Gite); 
};

#endif