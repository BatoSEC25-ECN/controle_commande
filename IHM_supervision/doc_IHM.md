# Documentation de l'IHM de supervision SEULEMENT pour la régulation et l'asservissement du **safran**

Le but de cette Interface Homme-Machine est de pouvoir modifier les caractéristiques du correcteur PI de la boucle d'asservissement de notre safran ainsi que les données de la boussole qui peuvent avoir un biais (qui en général ne varie pas). Cela se fait par radiocommunication à l'aide d'une communication ZigBee (deux modules XBee S2C). L'IHM va contrôler le module émetteur de la liaison point à point.

Cette solution est très pratique pour paramétrer le correcteur PI de la boucle d'asservissement de notre safran à distance, sans avoir à reprogrammer le microcontrôleur à chaque fois que l'on souhaite essayer de nouveaux coefficients pour le correcteur.

## Utilisation de l'application

### Prérequis

- Python 3 installé.
- Les bibliothèques Tkinter et pySerial :
    - Tkinter est généralement inclus avec Python.
    - Pour installer pySerial, vous pouvez utiliser pip :
        `pip install pyserial`

### Lancement de l'application

**Configuration du Port Série**

Exécutez le script Python (main.py par exemple). La première fenêtre apparaîtra avec deux champs :

- **Port :** Vérifiez ou modifiez le port (ex. "COM5").
- **Speed :** Vérifiez ou modifiez la vitesse (ex. 9600).

Cliquez sur le bouton Ok pour valider.

Si les paramètres ne sont pas valides ou si le port n'est pas disponible, un message d'erreur s'affiche dans la console et l'application attend une correction.

**Connexion et Validation des Paramètres**

Une fois la connexion série établie, la deuxième fenêtre s'ouvre pour saisir les paramètres :

- **Cap souhaité :** Saisir le cap (en degrés) et cliquer sur Validate pour envoyer la valeur.
- **Coefficient P :** Saisir la valeur du coefficient P et cliquer sur Validate pour l'envoyer.
- **Coefficient I :** Saisir la valeur du coefficient I et cliquer sur Validate pour l'envoyer.
- **Offset boussole :** Saisir l'offset et cliquer sur Validate pour envoyer la valeur du compas.

À chaque validation, le message formaté (par exemple, cap 45, coeffP 1.5, etc.) est envoyé via la connexion série. Les messages envoyés ainsi que d'éventuelles erreurs de transmission sont affichés dans la console.

### Communication avec le périphérique

Le périphérique équipé pour recevoir les paramètres via le port série doit être en écoute sur le port configuré. Chaque commande envoyée par l'IHM permet d’ajuster dynamiquement des paramètres de fonctionnement du périphérique (exemple courant : système de contrôle ou régulation).

### Points importants

- **Sécurité de connexion :** Si la connexion série échoue (port non disponible ou paramètres invalides), vérifiez que le port spécifié correspond bien à la connexion physique.
- **Portabilité :** L'application est conçue pour fonctionner sur Windows (exemple d'un port COM). Pour d'autres systèmes, adaptez le nom du port (par exemple /dev/ttyUSB0 sur Linux).

## Présentation de l'IHM

Cette application propose une interface graphique (IHM) réalisée avec Tkinter et permet de configurer une connexion série pour envoyer des paramètres prédéfinis à un périphérique connecté (par exemple, un microcontrôleur). Le script se décompose en deux parties principales :

### a) Configuration du port série

**Fenêtre de configuration :**

Lors du démarrage, une fenêtre s'ouvre pour que l'utilisateur saisisse les informations du port série.

- **Port :** Champ de saisie pré-rempli (par défaut "COM5").
- **Speed (vitesse) :** Champ de saisie pré-rempli avec la valeur 9600.
- Un bouton Ok permet de valider ces paramètres.

**Validation :**

La fonction `validate_port()` vérifie que le port existe et que la vitesse choisie est valide (doit être l'un des standards : 1200, 2400, 4800, 9600, 19200, 38400, 57600, ou 115200 bauds).

**Connexion série :**

Si les paramètres sont corrects, la fonction `setup_serial_connection()` ouvre la communication série grâce à la bibliothèque pySerial.

### b) Fenêtre des paramètres

Une fois la connexion série établie, une seconde fenêtre s'ouvre pour permettre la configuration de plusieurs paramètres :

- **Cap souhaité (en °) :** Un champ pour entrer la valeur du cap désiré.
- **Coefficient P et Coefficient I :** Deux champs pour entrer respectivement les coefficients de l'algorithme de régulation (par exemple, pour un PID).
- **Offset boussole :** Un champ pour spécifier l'offset de la boussole.

Chaque paramètre dispose d'un bouton Validate associé à ce champ. Lors de la validation, la fonction `send_data()` envoie le nom du champ et la valeur correspondante via la connexion série au périphérique cible. Le message envoyé est formaté sous la forme :

```
<nom du champ> <valeur>
```

Ce système de validation permet d'envoyer les commandes de manière individuelle dès que l'utilisateur clique sur le bouton associé.

