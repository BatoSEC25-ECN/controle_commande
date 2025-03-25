import serial
import time
import os

def main():
    # Configuration du port série
    port = "COM17"            # Remplacez par votre port 
    baudrate = 115200

    # Répertoire de destination pour l'enregistrement des fichiers
    destination_directory = "recup_sd_files"
    os.makedirs(destination_directory, exist_ok=True)

    try:
        ser = serial.Serial(port, baudrate, timeout=1)
    except Exception as e:
        print("Erreur d'ouverture du port série :", e)
        return

    print(f"Port série {port} ouvert à {baudrate} bauds.")
    time.sleep(2)  # Petite pause pour laisser le temps au microcontrôleur de s'initialiser

    # Envoi de la commande pour déclencher l'export des fichiers depuis la Pico
    command = "RECUP_SDFILES\n"
    ser.write(command.encode('ascii'))
    print("Commande 'RECUP_SDFILES' envoyée.")

    print("En attente de données...")

    file_handle = None       # Gestionnaire du fichier en cours
    current_filename = None  # Chemin complet transmis par la Pico

    while True:
        try:
            # Lecture d'une ligne depuis le port série
            line = ser.readline()
            if not line:
                continue

            # Tentative de décodage en ASCII
            try:
                decoded_line = line.decode('ascii').strip()
            except UnicodeDecodeError:
                decoded_line = ""

            # Détection du marqueur de début de fichier
            if decoded_line.startswith("FILE_START:"):
                # Extraction du chemin transmis
                current_filename = decoded_line[len("FILE_START:"):].strip()
                print("Début de réception du fichier :", current_filename)

                # Supprimer le caractère de début '/' s'il est présent pour éviter d'obtenir un chemin absolu
                if current_filename.startswith("/"):
                    current_filename = current_filename[1:]

                # Construction du chemin complet dans le répertoire de destination
                full_path = os.path.join(destination_directory, current_filename)
                # Création des dossiers intermédiaires
                os.makedirs(os.path.dirname(full_path), exist_ok=True)
                # Ouverture en écriture binaire
                file_handle = open(full_path, "wb")
                continue

            # Détection du marqueur de fin de fichier
            if decoded_line.startswith("FILE_END:"):
                if file_handle is not None:
                    file_handle.close()
                    print("Fichier", current_filename, "enregistré.")
                    file_handle = None
                    current_filename = None
                continue

            # Écriture des données dans le fichier en cours
            if file_handle is not None:
                file_handle.write(line)
            else:
                # Affichage d'éventuels autres messages
                print(decoded_line)
        except KeyboardInterrupt:
            print("Arrêt du script par l'utilisateur.")
            if file_handle is not None:
                file_handle.close()
            break
        except Exception as e:
            print("Erreur :", e)
            time.sleep(0.1)

if __name__ == "__main__":
    main()
