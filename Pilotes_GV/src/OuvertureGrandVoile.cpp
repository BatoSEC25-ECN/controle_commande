// Fichier: OuvertureGrandVoile.cpp
// Description: Implémentation des méthodes de la classe Ouverture_Grand_Voile

#include "ReglageGV.h"
#include "OuvertureGrandVoile.h"
#include <math.h>

// Initialise les attributs et attache le moteur
void Ouverture_Grand_Voile::Init_GV(int Girouette, int IMU_Gite, int pinMoteur) {
    
    static int moteur_init = 0;

    //delay(2000); // Pause de 2 secondes avant la prochaine commande
 
    Serial.println("Init_GV: Initialisation des capteurs et moteur\n"); // Message indiquant l'initialisation
    //Cap_Actuel = Boussole; // Stocke la valeur actuelle du cap
    Cap_Actuel = ORIENT_PROUE;//On prend la proue comme référence de cap
    Direction_VA = Girouette; // Stocke la direction du vent apparent
    Gite = IMU_Gite; // Stocke l'inclinaison du bateau
    deltaAngle = Cap_Actuel - Direction_VA - TRANSPO_REPERE; // Calcul de la différence d'angle et le transpose dan sle bon référentiel

    if(moteur_init == 0) {
        moteurGV.attach(pinMoteur); // Attache le moteur à la broche spécifiée
        moteur_init = 1;// Le moteur est initialisé, on ne doit plus l'initialiser
    }
}

// Détermine le mode de navigation en fonction de l'angle entre le cap actuel et le vent
void Ouverture_Grand_Voile::Mode_Navigation() {

    //delay(2000); // Pause de 2 secondes avant la prochaine commande
 
    Serial.println("Mode_Navigation: Détermination du mode de navigation\n"); // Message indiquant l'analyse du mode
    //int deltaAngle = Cap_Actuel - Direction_VA; // Calcul de la différence d'angle
    int delta_abs = abs(deltaAngle);
    if (delta_abs >= 90) {
        Mode_Nav = SOUS_LE_VENT; // Sous le vent
    } else if (abs(deltaAngle) < 90) {
        Mode_Nav = FACE_AUX_VENT; // Vent de face
    }
}

// Gestion de la grand-voile lorsque le bateau est face au vent
void Ouverture_Grand_Voile::RG_Face_au_Vent() {

    //delay(2000); // Pause de 2 secondes avant la prochaine commande
 
    Serial.println("\n\nRG_Face_au_Vent: Réglage GV face au vent\n"); // Message indiquant le réglage
    Angle_GV = ANGLE_GV_MIN; // Réduction maximale de la grand-voile
    if (Gite > MAX_GITE) {
        Angle_GV--; // Ajustement pour réduire la gîte
    }
}

// Gestion de la grand-voile lorsque le bateau est sous le vent
void Ouverture_Grand_Voile::RG_Sous_le_Vent() {
    //delay(2000); // Pause de 2 secondes avant la prochaine commande
 
    Serial.println("\n\nRG_Sous_le_Vent: Réglage GV sous le vent\n"); // Message indiquant le réglage
    Angle_GV = ((abs(deltaAngle)-MIN_BABORD) / OUVERTURE_BABORD) * ANGLE_GV_MAX;// Calcul d'ouverture en fonction de l'angle
}


// Positionne le moteur de la grand-voile en fonction de l'angle calculé
void Ouverture_Grand_Voile::pos_moteur() {
    Serial.println("pos_moteur: Positionnement du moteur\n"); // Message indiquant l'action sur le moteur
    float t_impulsion = ((POS_MIN_BORDE - POS_MAX_CHOQUE) * (CONV_ANGLE_TEMPS - Angle_GV) / ANGLE_GV_MAX) + POS_MAX_CHOQUE;
    // Conversion de l'angle en temps d'impulsion pour le moteur
    if(t_impulsion > POS_MIN_BORDE || t_impulsion < POS_MAX_CHOQUE) {
        Serial.println("Erreur: Angle invalide, passage en position par défaut\n"); // Message d'erreur si l'angle est hors limites
        Angle_GV = POS_MAX_CHOQUE;
    }
    //delay(2000); // Pause de 2 secondes avant la prochaine commande
    Serial.println("\n\n\nDébut commande\n");

    moteurGV.writeMicroseconds(int(t_impulsion * 1000)); // Envoi de l'impulsion au moteur

    //delay(2000); // Pause de 2 secondes avant la prochaine itération
    Serial.println("Fin délai après commande\n\n\n");

}


// Fonction principale pour gérer la grand-voile
void Ouverture_Grand_Voile::System_GV(int Girouette, int IMU_Gite) {
    Serial.println("System_GV: Démarrage du système de gestion de la grand-voile\n"); // Message indiquant le début du processus
    Init_GV(Girouette, IMU_Gite, BROCHE_MOTEUR); // Initialisation des capteurs et du moteur
    Mode_Navigation(); // Détermination du mode de navigation#
    
    if (Mode_Nav == FACE_AUX_VENT) {
        RG_Face_au_Vent(); // Ajustement si face au vent
    } else if (Mode_Nav == SOUS_LE_VENT) {
        RG_Sous_le_Vent(); // Ajustement si sous le vent
    }

    //delay(2000); // Pause de 2 secondes avant la prochaine commande
 
    pos_moteur(); // Application de l'ajustement au moteur
}
