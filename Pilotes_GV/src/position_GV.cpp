#include <Arduino.h>
#include <Servo.h>

// Définition des modes de navigation
#define FACE_AU_VENT 0
#define SOUS_LE_VENT 1

// SE5 : Paramètres moteur
#define POS_MAX_CHOQUE 1.32  // Position max choqué (ms)
#define POS_MIN_BORDE 1.80   // Position min bordé (ms)

// Classe principale gérant l'ouverture de la Grand Voile
class Ouverture_Grand_Voile {
private:
    // SE1 - Capteurs
    float Cap_Actuel;
    float Direction_VA;
    
    // SE3 & SE4 - Gîte
    float Gite;

    // SE5 - Position de la Grand Voile
    float Angle_GV;
    Servo moteurGV;  // Moteur pas à pas ou servo moteur

    // SE2 - Déterminer le mode de navigation
    int Mode_Navigation() {
        if (abs(Cap_Actuel - Direction_VA) < 90) {
            return FACE_AU_VENT;
        }
        return SOUS_LE_VENT;
    }

    // SE3 - Calcul de l'angle optimal en navigation face au vent
    void Angle_Opti_FV() {
        float Orientation = Cap_Actuel - Direction_VA;
        if (abs(Orientation) > 140) {
            Angle_GV = 0;  // Trop face au vent, on borde
        } else if (Orientation > 40 && Orientation < 90) {
            Angle_GV = (45.0 / 50.0) * (Direction_VA - 90) + 90;
        } else {
            Angle_GV = 0;
        }
    }

    // SE4 - Calcul de l'angle optimal en navigation sous le vent
    void Angle_Opti_SV() {
        float Orientation = Cap_Actuel - Direction_VA;
        if (Orientation > -180 && Orientation < 90) {
            Angle_GV = abs(Direction_VA - 90) % 180;
        } else if (Orientation > 0 && Orientation < 45) {
            Angle_GV = 90;
        } else if (Orientation > 45 && Orientation < 90) {
            Angle_GV = 0;
        }
    }

    // SE5 - Contrôle du moteur
    void pos_moteur() {
        // Calcul du temps d'impulsion en fonction de l'angle de la voile
        float t_impulsion = ((POS_MIN_BORDE - POS_MAX_CHOQUE) * Angle_GV / 90) + POS_MAX_CHOQUE;
        int pulseWidth = int(t_impulsion * 1000);  // Conversion en microsecondes

        // Commande le moteur
        moteurGV.writeMicroseconds(pulseWidth);
    }

public:
    // SE1 - Initialisation des capteurs
    void Init_GV(float Boussole, float Girouette, float IMU_Gite, int pinMoteur) {
        Cap_Actuel = Boussole;
        Direction_VA = Girouette;
        Gite = IMU_Gite;
        moteurGV.attach(pinMoteur);
    }

    // SE1 - Système global de gestion de la GV
    void System_GV(float Boussole, float Girouette, float IMU_Gite) {
        Init_GV(Boussole, Girouette, IMU_Gite, 9);  // Moteur sur la broche 9

        if (Mode_Navigation() == FACE_AU_VENT) {
            Angle_Opti_FV();
        } else {
            Angle_Opti_SV();
        }

        pos_moteur();
    }
};

// Création de l'objet Grand Voile
Ouverture_Grand_Voile GV;

void setup() {
    Serial.begin(9600);
}

void loop() {
    // Simulation de valeurs capteurs (à remplacer par des valeurs réelles)
    float capteurBoussole = 45.0;
    float capteurGirouette = 90.0;
    float capteurIMU = 5.0;

    // Gestion automatique de la Grand Voile
    GV.System_GV(capteurBoussole, capteurGirouette, capteurIMU);

    delay(2000); // Attente 2s pour observer l'effet
}
