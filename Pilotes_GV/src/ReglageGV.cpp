// Inclusion de la bibliothèque Servo
#include <Servo.h>
#include <Arduino.h>
#include "ReglageGV.h"

// Définition du servo
Servo monServo;

// Définition des variables (avec initialisation si souhaité)
float valeurRef = 1.5;
float pas = 0.05;
float valeurMin = 1.0;
float valeurMax = 2.0;
const int pinServo = 9;

// Fonction pour attendre une réponse de l'utilisateur (o/n)
char attendreReponse() {
    while (!Serial.available()); // Attendre une entrée
    char reponse = Serial.read();
    Serial.println(reponse); // Affichage retour pour confirmation
    return reponse;
}

void demarrage(void)
{
    // Message de démarrage
    Serial.println("Bonjour, appuyez sur 'o' pour commencer.");
    while (attendreReponse() != 'o'); // Attendre la validation

    // Saisie de la valeur de référence
    Serial.println("Entrez la valeur de référence (ms) :");
    while (!Serial.available());
    valeurRef = Serial.parseFloat();
    Serial.println(valeurRef); // Confirmation

    // Saisie du pas d'incrémentation/décrémentation
    Serial.println("Entrez le pas d'incrémentation/décrémentation (ms) :");
    while (!Serial.available());
    pas = Serial.parseFloat();
    Serial.println(pas); // Confirmation

    // Initialisation de la position du servo
    monServo.writeMicroseconds(valeurRef * 1000);
}

// Fonction d'initialisation du programme
void init_GV() {
    Serial.begin(9600); // Initialisation de la liaison série
    monServo.attach(pinServo); // Attache le servo à la broche définie
}

// Fonction de réglage du min
void reglerMin() {
    Serial.println("Réglage du minimum...");

    float valeurActuelle = valeurRef; // Départ sur la valeur de référence

    while (true) {
        monServo.writeMicroseconds(valeurActuelle * 1000); // Envoi de la valeur
        Serial.print("Valeur actuelle (ms) : ");
        Serial.println(valeurActuelle);

        Serial.println("Continuer ? (o/n)");
        if (attendreReponse() == 'n') break; // Sortie de boucle si "n"

        valeurActuelle -= pas; // Décrémentation
    }

    valeurMin = valeurActuelle;
}

// Fonction de réglage du max
void reglerMax() {
    Serial.println("Réglage du maximum...");

    float valeurActuelle = valeurRef; // Départ sur la valeur de référence

    while (true) {
        monServo.writeMicroseconds(valeurActuelle * 1000); // Envoi de la valeur
        Serial.print("Valeur actuelle (ms) : ");
        Serial.println(valeurActuelle);

        Serial.println("Continuer ? (o/n)");
        if (attendreReponse() == 'n') break; // Sortie de boucle si "n"

        valeurActuelle += pas; // Incrémentation
    }

    valeurMax = valeurActuelle;
}

// Boucle principale
void boucle_init_GV() {
    bool reglage = true;
    while (reglage) {

        demarrage();//réglage des valeurs de références

        reglerMin(); // Trouver le min
        reglerMax(); // Trouver le max

        // Affichage des résultats
        Serial.print("Valeur minimale trouvée : ");
        Serial.println(valeurMin);
        Serial.print("Valeur maximale trouvée : ");
        Serial.println(valeurMax);

        // Demander confirmation utilisateur
        Serial.println("Êtes-vous satisfait des valeurs trouvées ? (o/n)");

        char reponse = attendreReponse();
        if (reponse == 'o') {
            Serial.println("Réglage terminé. Merci !");
            reglage = false; // Fin du programme
        } else {
            Serial.println("Redémarrage du réglage...");
        }
    }
}

void Reglage_GV(void)
{
    init_GV();
    boucle_init_GV();
}
