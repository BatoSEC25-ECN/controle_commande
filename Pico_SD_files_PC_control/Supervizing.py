import serial
import time
import os

def list_sd_files(ser):
    """
    Envoie la commande LIST_SDFILES et affiche les lignes reçues (arborescence).
    """
    print("\n[INFO] Envoi de la commande LIST_SDFILES...")
    ser.write(b"LIST_SDFILES\n")
    time.sleep(0.5)  # Petite pause pour que le microcontrôleur réponde
    while True:
        line = ser.readline()
        if not line:
            break
        try:
            decoded_line = line.decode('ascii').strip()
        except UnicodeDecodeError:
            decoded_line = ""
        # Affichage des messages retournés
        print(decoded_line)

def download_sd_files(ser, destination_directory="recup_sd_files", timeout_sec=5):
    """
    Envoie la commande RECUP_SDFILES et enregistre les fichiers reçus dans le répertoire
    destination_directory en recréant l'arborescence.
    
    Si aucune donnée n'est reçue pendant 'timeout_sec' secondes, le téléchargement sera considéré comme terminé,
    permettant ainsi de revenir au menu.
    """
    os.makedirs(destination_directory, exist_ok=True)
    print("\n[INFO] Envoi de la commande RECUP_SDFILES...")
    ser.write(b"RECUP_SDFILES\n")
    time.sleep(0.5)  # Petite pause pour que le microcontrôleur réponde
    
    file_handle = None       # Gestionnaire pour le fichier en cours de réception
    current_filename = None  # Nom du fichier en cours
    last_receipt_time = time.time()  # Temps du dernier octet reçu

    while True:
        # Si aucune donnée n'a été reçue pendant 'timeout_sec' secondes, on sort de la boucle.
        if time.time() - last_receipt_time > timeout_sec:
            print("[INFO] Fin du téléchargement (timeout).")
            break

        line = ser.readline()
        if not line:
            continue  # Pas de donnée reçue, la boucle vérifie le timeout
        last_receipt_time = time.time()  # On réinitialise le timeout dès qu'une donnée est reçue

        try:
            decoded_line = line.decode('ascii').strip()
        except UnicodeDecodeError:
            decoded_line = ""

        # Détection du début d'un fichier
        if decoded_line.startswith("FILE_START:"):
            current_filename = decoded_line[len("FILE_START:"):].strip()
            print(f"[INFO] Début de réception du fichier : {current_filename}")
            # Pour éviter de recréer un chemin absolu sur le PC
            if current_filename.startswith("/"):
                current_filename = current_filename[1:]
            full_path = os.path.join(destination_directory, current_filename)
            os.makedirs(os.path.dirname(full_path), exist_ok=True)
            file_handle = open(full_path, "wb")
            continue

        # Détection de la fin d'un fichier
        if decoded_line.startswith("FILE_END:"):
            if file_handle:
                file_handle.close()
                print(f"[INFO] Fichier '{current_filename}' enregistré.")
                file_handle = None
                current_filename = None
            continue

        # Écriture des données dans le fichier en cours
        if file_handle:
            file_handle.write(line)
        else:
            # Affichage des autres messages (infos, erreurs, etc.)
            print(decoded_line)

        # Si on reçoit un marqueur de fin explicite, on sort aussi
        if "=== Fin récupération fichiers SD ===" in decoded_line:
            break

def delete_file(ser, file_path):
    """
    Envoie la commande DELETE_FILE avec le chemin spécifié puis affiche la réponse.
    """
    command = "DELETE_FILE:" + file_path + "\n"
    print(f"\n[INFO] Envoi de la commande pour supprimer le fichier : {file_path}")
    ser.write(command.encode('ascii'))
    # Lecture de la réponse pendant quelques secondes
    timeout = time.time() + 5
    while time.time() < timeout:
        line = ser.readline()
        if line:
            try:
                decoded_line = line.decode('ascii').strip()
            except UnicodeDecodeError:
                decoded_line = ""
            print(decoded_line)
        else:
            break

def delete_all(ser):
    """
    Envoie la commande DELETE_ALL pour supprimer tous les fichiers et dossiers de la carte SD,
    puis affiche les messages reçus.
    """
    print("\n[INFO] Envoi de la commande DELETE_ALL...")
    ser.write(b"DELETE_ALL\n")
    # Lecture de la réponse pendant quelques secondes
    timeout = time.time() + 10
    while time.time() < timeout:
        line = ser.readline()
        if line:
            try:
                decoded_line = line.decode('ascii').strip()
            except UnicodeDecodeError:
                decoded_line = ""
            print(decoded_line)
        else:
            break

def delete_directory(ser, dir_path):
    """
    Envoie la commande DELETE_DIR avec le chemin spécifié pour supprimer un dossier (et son contenu),
    puis affiche la réponse.
    """
    command = "DELETE_DIR:" + dir_path + "\n"
    print(f"\n[INFO] Envoi de la commande pour supprimer le dossier : {dir_path}")
    ser.write(command.encode('ascii'))
    # Lecture de la réponse pendant quelques secondes
    timeout = time.time() + 10
    while time.time() < timeout:
        line = ser.readline()
        if line:
            try:
                decoded_line = line.decode('ascii').strip()
            except UnicodeDecodeError:
                decoded_line = ""
            print(decoded_line)
        else:
            break

def main():
    # Configuration du port série
    port = "COM18"  # Remplacez par le port de votre microcontrôleur
    baudrate = 115200

    try:
        ser = serial.Serial(port, baudrate, timeout=1)
    except Exception as e:
        print("Erreur d'ouverture du port série :", e)
        return

    print(f"[INFO] Port série {port} ouvert à {baudrate} bauds.")
    time.sleep(2)  # Pause pour laisser le temps au microcontrôleur de s'initialiser

    while True:
        print("\n====== MENU ======")
        print("1. Afficher l'arborescence des fichiers sur la carte SD\n")
        print("2. Télécharger les fichiers de la carte SD\n")
        print("3. Supprimer un fichier sur la carte SD")
        print("4. Supprimer un dossier sur la carte SD\n")
        print("5. Supprimer TOUS les fichiers et dossiers sur la carte SD\n")
        print("6. Quitter\n")
        choix = input("Votre choix : ").strip()

        if choix == "1":
            list_sd_files(ser)
        elif choix == "2":
            download_sd_files(ser)
        elif choix == "3":
            file_path = input("Indiquez le chemin complet du fichier à supprimer (ex: /dossier/fichier.txt) : ").strip()
            delete_file(ser, file_path)
        elif choix == "4":
            dir_path = input("Indiquez le chemin complet du dossier à supprimer (ex: /dossier) : ").strip()
            delete_directory(ser, dir_path)
        elif choix == "5":
            confirm = input("ATTENTION : Vous êtes sur le point de supprimer TOUTE la carte SD. Confirmez (O/N) : ").strip().lower()
            if confirm == "o":
                delete_all(ser)
            else:
                print("Opération annulée.")
        elif choix == "6":
            print("Fermeture du script.")
            break
        else:
            print("Choix invalide. Veuillez réessayer.")

    ser.close()

if __name__ == "__main__":
    main()
