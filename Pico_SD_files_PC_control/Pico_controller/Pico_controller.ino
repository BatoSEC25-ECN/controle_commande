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

    // Commande de récupération de l'arborescence et du contenu complet des fichiers sur la carte SD
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
    // Commande d'affichage de l'arborescence (sans contenu)
    else if (command.equalsIgnoreCase("LIST_SDFILES")) {
      Serial.println("=== Affichage de l'arborescence des fichiers SD ===");
      File root = SD.open("/");
      if (root) {
        listDirectory(root, "/");
        root.close();
      } else {
        Serial.println("Erreur : Impossible d'ouvrir la racine de la carte SD.");
      }
      Serial.println("=== Fin de l'arborescence ===");
    }
    // Commande de suppression d'un fichier
    else if (command.startsWith("DELETE_FILE:")) {
      String filePath = command.substring(String("DELETE_FILE:").length());
      filePath.trim();
      if (filePath.length() > 0) {
        if (SD.exists(filePath)) {
          if (SD.remove(filePath)) {
            Serial.print("FILE_DELETED:");
            Serial.println(filePath);
          } else {
            Serial.print("ERROR_DELETE:");
            Serial.println(filePath);
          }
        } else {
          Serial.print("FILE_NOT_FOUND:");
          Serial.println(filePath);
        }
      } else {
        Serial.println("ERROR: Aucun chemin de fichier fourni.");
      }
    }
    // Commande de suppression de tous les fichiers et dossiers présents sur la carte SD
    else if (command.equalsIgnoreCase("DELETE_ALL")) {
      Serial.println("=== Suppression de tous les fichiers de la carte SD ===");
      deleteAllFiles();
      Serial.println("=== Fin suppression de tous les fichiers ===");
    }
    // Commande de suppression d'un dossier (et de son contenu)
    else if (command.startsWith("DELETE_DIR:")) {
      String dirPath = command.substring(String("DELETE_DIR:").length());
      dirPath.trim();
      if (dirPath.length() > 0) {
        if (SD.exists(dirPath)) {
          if (deleteDirectory(dirPath)) {
            // Le message de succès est affiché dans la fonction deleteDirectory.
          }
        } else {
          Serial.print("DIR_NOT_FOUND:");
          Serial.println(dirPath);
        }
      } else {
        Serial.println("ERROR: Aucun chemin de dossier fourni.");
      }
    }
  }
}

// Fonction récursive qui parcourt un répertoire et envoie le contenu complet des fichiers
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
      Serial.print("DIR:");
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
        // Transmission en binaire directement sur le port série
        Serial.write(buffer, bytesRead);
        delay(1); // Délai pour éviter les pertes de données
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

// Fonction récursive qui parcourt un répertoire et affiche uniquement le nom des fichiers et dossiers
void listDirectory(File dir, String path) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // Fin du répertoire
      break;
    }
    
    String fullPath = path;
    fullPath += entry.name();
    
    if (entry.isDirectory()) {
      Serial.print("DIR:");
      Serial.println(fullPath);
      // Appel récursif pour le sous-dossier, en ajoutant le séparateur "/"
      listDirectory(entry, fullPath + "/");
    } else {
      Serial.print("FILE:");
      Serial.println(fullPath);
    }
    entry.close();
  }
}

// Fonction qui supprime récursivement tout le contenu d'un dossier, puis le dossier lui-même
bool deleteDirectory(String dirPath) {
  File dir = SD.open(dirPath);
  if (!dir) {
    Serial.print("ERROR: Impossible d'ouvrir le dossier ");
    Serial.println(dirPath);
    return false;
  }
  if (!dir.isDirectory()) {
    Serial.print("ERROR: ");
    Serial.print(dirPath);
    Serial.println(" n'est pas un dossier.");
    dir.close();
    return false;
  }
  
  // Supprime tous les fichiers et sous-dossiers
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) break;
    
    String entryPath = dirPath;
    if (!dirPath.endsWith("/")) entryPath += "/";
    entryPath += entry.name();
    
    if (entry.isDirectory()) {
      deleteDirectory(entryPath);
    } else {
      if (SD.remove(entryPath)) {
        Serial.print("FILE_DELETED:");
        Serial.println(entryPath);
      } else {
        Serial.print("ERROR_DELETE:");
        Serial.println(entryPath);
      }
    }
    entry.close();
  }
  dir.close();
  
  // Suppression du dossier lui-même (supposons que SD.rmdir() soit disponible)
  if (SD.rmdir(dirPath)) {
    Serial.print("DIR_DELETED:");
    Serial.println(dirPath);
    return true;
  } else {
    Serial.print("ERROR_DELETE_DIR:");
    Serial.println(dirPath);
    return false;
  }
}

// Fonction qui supprime tous les fichiers et dossiers de la racine de la carte SD
void deleteAllFiles() {
  File root = SD.open("/");
  if (!root) {
    Serial.println("Erreur : Impossible d'ouvrir la racine de la carte SD.");
    return;
  }
  
  while (true) {
    File entry = root.openNextFile();
    if (!entry) break;
    
    String fullPath = "/";
    fullPath += entry.name();
    
    if (entry.isDirectory()) {
      deleteDirectory(fullPath);
    } else {
      if (SD.remove(fullPath)) {
        Serial.print("FILE_DELETED:");
        Serial.println(fullPath);
      } else {
        Serial.print("ERROR_DELETE:");
        Serial.println(fullPath);
      }
    }
    entry.close();
  }
  root.close();
}
