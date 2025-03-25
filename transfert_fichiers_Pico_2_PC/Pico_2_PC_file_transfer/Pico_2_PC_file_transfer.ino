#include <SPI.h>
#include <SD.h>

// Définition des broches pour la carte SD (adaptées à la Raspberry Pi Pico)
const int _CS   = 15;
const int _SCK  = 10;
const int _MOSI = 11;
const int _MISO = 12;

void setup() {
  // Initialisation du port série (USB) pour la communication avec le PC
  Serial.begin(115200);
  while (!Serial) {
    ; // Attente de la connexion de la console, pour certains systèmes
  }
  
  // Configuration de l'interface SPI dédiée (pour RPi Pico)
  SPI1.setRX(_MISO);
  SPI1.setTX(_MOSI);
  SPI1.setSCK(_SCK);
  
  // Initialisation de la carte SD
  if (!SD.begin(_CS, 4000000, SPI1)) {
    Serial.println("Erreur : Carte SD non présente ou initialisation échouée.");
  } else {
    Serial.println("Carte SD initialisée.");
  }
}

void loop() {
  // Attente d'une commande sur le port série USB
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.equalsIgnoreCase("RECUP_SDFILES")) {
      Serial.println("=== Lancement récupération fichiers SD ===");
      File root = SD.open("/");
      if (root) {
        processDirectory(root, "/");
        root.close();
      } else {
        Serial.println("Erreur : Impossible d'ouvrir la racine de la carte SD.");
      }
      Serial.println("=== Fin récupération fichiers SD ===");
    }
  }
}

// Fonction récursive qui parcourt un répertoire
// "path" contient le chemin complet courant sur la carte SD
void processDirectory(File dir, String path) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // Fin du répertoire
      break;
    }
    
    String fullPath = path;
    fullPath += entry.name();
    
    if (entry.isDirectory()) {
      Serial.print("DIR : ");
      Serial.println(fullPath);
      // Appel récursif pour le sous-dossier, en ajoutant le séparateur "/"
      processDirectory(entry, fullPath + "/");
    } else {
      // Envoi d'un marqueur de début de fichier
      Serial.print("FILE_START:");
      Serial.println(fullPath);
      
      // Lecture et transmission du contenu du fichier par blocs
      const int bufferSize = 64;
      byte buffer[bufferSize];
      while (entry.available()) {
        int bytesRead = entry.read(buffer, bufferSize);
        // On transmet en binaire directement sur le port série
        Serial.write(buffer, bytesRead);
      }
      
      // Fin du fichier : marqueur de fin
      Serial.println(); // Terminer la ligne si le fichier ne finit pas par un saut de ligne
      Serial.print("FILE_END:");
      Serial.println(fullPath);
      Serial.println(); // Ligne vide pour séparer les fichiers  
    }
    entry.close();
  }
}
