# Documentation : Gestion de Carte SD via Raspberry Pi Pico et Script Python

Ce document explique comment utiliser le système complet qui permet de communiquer avec une carte SD connectée à un Raspberry Pi Pico (via Arduino/C++ sur Pico) et de gérer la carte SD (affichage de l'arborescence, téléchargement, suppression de fichiers/dossiers) à l’aide d’un script Python.

## Table des Matières
- [Introduction](#introduction)
- [Matériel et Prérequis](#matériel-et-prérequis)
- [Fonctionnalités](#fonctionnalités)
- [Côté Arduino (Raspberry Pi Pico)](#côté-arduino-raspberry-pi-pico)
    - [Configuration Matérielle](#configuration-matérielle)
    - [Commandes Disponibles](#commandes-disponibles)
- [Utilisation du Script Python](#utilisation-du-script-python)
    - [Installation et Configuration](#installation-et-configuration)
    - [Menu et Actions](#menu-et-actions)
- [Instructions Pas à Pas](#instructions-pas-à-pas)
- [Conclusion](#conclusion)

---

## Introduction

Ce projet permet d'interfacer un Raspberry Pi Pico équipé d'une carte SD avec un ordinateur via un port série. Un firmware Arduino (C/C++) gère l'accès à la carte SD et répond à diverses commandes envoyées par le PC. Le script Python (contenu dans `Supervizing.py`) permet d'envoyer ces commandes et de récupérer les informations ou le contenu des fichiers présents sur la carte SD.

---

## Matériel et Prérequis

### Matériel :

- Raspberry Pi Pico (ou autre microcontrôleur compatible Arduino/C++)  
- Module de carte SD avec interface SPI (les broches de connexion peuvent varier selon la plateforme)   

### Logiciel et bibliothèques :
- **Arduino IDE** pour programmer le Raspberry Pi Pico  
- **Bibliothèque SD.h** (pour la gestion de la carte SD)  
- **Bibliothèque SPI.h** (pour la communication SPI)  
- **Python 3** installé sur votre ordinateur  
- **Bibliothèque pyserial** (pour la communication série) que vous pouvez installer via pip :  
    ```bash
    pip install pyserial
    ```

---

## Fonctionnalités

Le système offre plusieurs commandes pour interagir avec la carte SD :

- **Récupération des fichiers** :  
    Envoi de l'arborescence complète et du contenu de chaque fichier (commande `RECUP_SDFILES`).

- **Affichage de l'arborescence** :  
    Affichage uniquement des noms des fichiers et dossiers (commande `LIST_SDFILES`).

- **Suppression des fichiers et dossiers** :  
    - Suppression d’un fichier spécifique (commande `DELETE_FILE:<chemin>`).  
    - Suppression d’un dossier et de son contenu (commande `DELETE_DIR:<chemin>`).  
    - Suppression de tous les fichiers et dossiers de la carte SD (commande `DELETE_ALL`).

---

## Côté Arduino (Raspberry Pi Pico)

### Configuration Matérielle

Le code Arduino doit être adapté à votre configuration matérielle. Dans l'exemple fourni, les broches utilisées pour la carte SD sont :

- **CS** : Pin 15  
- **SCK** : Pin 10  
- **MOSI** : Pin 11  
- **MISO** : Pin 12  

### Commandes Disponibles

Le firmware Arduino écoute sur le port série et répond aux commandes suivantes :

- **`RECUP_SDFILES`**  
    Parcourt récursivement la carte SD, envoie pour chaque fichier :  
    - Un marqueur de début de fichier : `FILE_START:<chemin_du_fichier>`  
    - Le contenu binaire du fichier (avec un léger délai pour éviter la perte de données).  
    - Un marqueur de fin de fichier : `FILE_END:<chemin_du_fichier>`  

- **`LIST_SDFILES`**  
    Parcourt récursivement la carte SD et affiche uniquement l'arborescence, avec :  
    - `DIR:<chemin_du_dossier>` pour les dossiers  
    - `FILE:<chemin_du_fichier>` pour les fichiers  

- **`DELETE_FILE:<chemin>`**  
    Supprime un fichier spécifique. Exemple :  
    ```text
    DELETE_FILE:/dossier/fichier.txt
    ```
    Réponse :  
    - Succès : `FILE_DELETED:<chemin>`  
    - Échec : `ERROR_DELETE:<chemin>` ou `FILE_NOT_FOUND:<chemin>`  

- **`DELETE_DIR:<chemin>`**  
    Supprime un dossier et son contenu.  

- **`DELETE_ALL`**  
    Supprime tous les fichiers et dossiers présents sur la carte SD.

---

## Utilisation du Script Python

Le script Python (par exemple, `Supervizing.py`) sert d’interface entre l’utilisateur et le microcontrôleur en utilisant la communication série.

### Installation et Configuration

1. **Installation de Python 3** :  
     Assurez-vous que Python 3 est installé sur votre système.

2. **Installation de la bibliothèque pyserial** :  
     ```bash
     pip install pyserial
     ```

3. **Configuration du script** :  
     Modifiez la variable `port` pour qu’elle corresponde au port série de votre microcontrôleur. Exemple :  
     ```python
     port = "COM17"  # Sous Windows, ou "/dev/ttyUSB0" sous Linux/Mac
     baudrate = 115200
     ```

### Menu et Actions

Le script affiche un menu interactif avec les options suivantes :

1. **Afficher l’arborescence des fichiers sur la carte SD**  
     Envoi de la commande `LIST_SDFILES` et affichage de l’arborescence reçue du Raspberry Pi Pico.

2. **Télécharger les fichiers de la carte SD**  
     Envoi de la commande `RECUP_SDFILES` pour récupérer les fichiers et créer une copie locale dans le répertoire `recup_sd_files`.

3. **Supprimer un fichier sur la carte SD**  
     Demande à l’utilisateur d’entrer le chemin complet du fichier à supprimer, puis envoie la commande `DELETE_FILE:<chemin>`.

4. **Supprimer un dossier sur la carte SD**  
     Demande à l’utilisateur d’entrer le chemin complet du dossier à supprimer, puis envoie la commande `DELETE_DIR:<chemin>`.

5. **Supprimer TOUS les fichiers et dossiers sur la carte SD**  
     Demande confirmation à l’utilisateur avant d’envoyer la commande `DELETE_ALL`.

6. **Quitter**  
     Quitte le script et ferme la connexion série.

---

## Instructions Pas à Pas

### 1. Configuration du Microcontrôleur (Arduino / Raspberry Pi Pico)
- Branchez la carte SD à votre Raspberry Pi Pico en vous assurant que les connexions SPI sont correctes.  
- Chargez le sketch Arduino dans l'IDE Arduino.  
- Compilez et téléversez le sketch sur le Raspberry Pi Pico.  
- Vérifiez l'initialisation de la carte SD via le moniteur série.

### 2. Exécution du Script Python
- Connectez le Raspberry Pi Pico à votre ordinateur via un câble USB.  
- Modifiez le script Python pour configurer le port série.  
- Exécutez le script Python :  
    ```bash
    python Supervizing.py
    ```

---

## Conclusion

Ce système permet une gestion complète de la carte SD à distance via une interface série. En combinant le firmware Arduino pour le Raspberry Pi Pico et le script Python, l’utilisateur peut facilement lister, récupérer et supprimer des fichiers ainsi que des dossiers sur la carte SD.
