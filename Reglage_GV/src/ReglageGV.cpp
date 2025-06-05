/**
 * @file ReglageGV.cpp
 * @brief Réglage manuel interactif des positions min et max d'un servo-moteur via la liaison série.
 */

// Inclusion de la bibliothèque Servo
#include <Servo.h>
#include <Arduino.h>

// Définition du servo
Servo monServo; /**< Servo moteur utilisé pour les tests */

// Déclaration des variables
float valeurRef = 1.5;      /**< Valeur de référence (en ms) pour la position neutre du servo */
float pas = 0.05;           /**< Pas d'incrémentation/décrémentation (en ms) pour l'ajustement */
float valeurMin, valeurMax; /**< Stockage des valeurs min et max réglées */
const int pinServo = 9;     /**< Broche du servo sur la carte L432KC */

/**
 * @brief Attend une réponse de l'utilisateur depuis la liaison série.
 * @return Le caractère lu (par exemple 'o' ou 'n').
 */
char attendreReponse() {
    while (!Serial.available());
    char reponse = Serial.read();
    Serial.println(reponse); // Confirmation visuelle
    return reponse;
}

/**
 * @brief Démarre le processus de réglage :
 * - attend validation utilisateur
 * - demande la valeur de référence et le pas
 * - initialise le servo sur la valeur de référence
 */
void demarrage(void)
{
    Serial.println("Bonjour, appuyez sur 'o' pour commencer.");
    while (attendreReponse() != 'o');

    Serial.println("Entrez la valeur de référence (ms) :");
    while (!Serial.available());
    valeurRef = Serial.parseFloat();
    Serial.println(valeurRef);

    Serial.println("Entrez le pas d'incrémentation/décrémentation (ms) :");
    while (!Serial.available());
    pas = Serial.parseFloat();
    Serial.println(pas);

    monServo.writeMicroseconds(valeurRef * 1000);
}

/**
 * @brief Initialise le port série et le servo.
 */
void init_GV() {
    Serial.begin(115200);
    monServo.attach(pinServo);
}

/**
 * @brief Réglage de la position minimale du servo :
 * - Décrémente la position jusqu'à la validation par l'utilisateur.
 * - Enregistre la valeur minimale trouvée.
 */
void reglerMin() {
    Serial.println("Réglage du minimum...");

    float valeurActuelle = valeurRef;

    while (true) {
        monServo.writeMicroseconds(valeurActuelle * 1000);
        Serial.print("Valeur actuelle (ms) : ");
        Serial.println(valeurActuelle);

        Serial.println("Continuer ? (o/n)");
        if (attendreReponse() == 'n') break;

        valeurActuelle -= pas;
    }

    valeurMin = valeurActuelle;
}

/**
 * @brief Réglage de la position maximale du servo :
 * - Incrémente la position jusqu'à la validation par l'utilisateur.
 * - Enregistre la valeur maximale trouvée.
 */
void reglerMax() {
    Serial.println("Réglage du maximum...");

    float valeurActuelle = valeurRef;

    while (true) {
        monServo.writeMicroseconds(valeurActuelle * 1000);
        Serial.print("Valeur actuelle (ms) : ");
        Serial.println(valeurActuelle);

        Serial.println("Continuer ? (o/n)");
        if (attendreReponse() == 'n') break;

        valeurActuelle += pas;
    }

    valeurMax = valeurActuelle;
}

/**
 * @brief Boucle principale de réglage :
 * - démarrage avec saisie des valeurs de référence
 * - réglage min puis max
 * - affichage des résultats et validation finale
 */
void boucle_init_GV() {
    while (true) {
        demarrage();
        reglerMin();
        reglerMax();

        Serial.print("Valeur minimale trouvée : ");
        Serial.println(valeurMin);
        Serial.print("Valeur maximale trouvée : ");
        Serial.println(valeurMax);

        Serial.println("Êtes-vous satisfait des valeurs trouvées ? (o/n)");
        char reponse = attendreReponse();
        if (reponse == 'o') {
            Serial.println("Réglage terminé. Merci !");
            while (true); // Fin du programme
        } else {
            Serial.println("Redémarrage du réglage...");
        }
    }
}

/**
 * @brief Point d'entrée pour lancer l'algorithme de réglage des positions min et max.
 */
void Reglage_GV(void)
{
    init_GV();
    boucle_init_GV();
}
