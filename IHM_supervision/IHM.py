from tkinter import Tk, Label, Entry, Button, INSERT
import serial
import serial.tools.list_ports

# Classe pour stocker les informations du port
class SerialPort:
    def __init__(self):
        self.Name = "COM3"
        self.Speed = 9600

myport = SerialPort()
serial_connection = None
ApplicationGL = False

# Fonction pour envoyer les données d'un champ
def send_data(field_name, field_value):
    global serial_connection
    if serial_connection:
        try:
            message = f"{field_name} {field_value}\n"
            serial_connection.write(message.encode())
            print(f"Sent: {message}")
        except Exception as e:
            print(f"Error sending data: {e}")
    else:
        print("Serial connection is not established.")

# Fonction pour configurer et ouvrir la connexion série
def setup_serial_connection():
    global serial_connection
    try:
        serial_connection = serial.Serial(myport.Name, int(myport.Speed), timeout=1)
        print(f"Connected to {myport.Name} at {myport.Speed} baud.")
    except Exception as e:
        print(f"Error connecting to serial port: {e}")

# Fonction pour afficher la seconde page (paramètres)
def open_parameters_window():
    def validate_cap():
        send_data("cap", cap_entry.get())

    def validate_coeffP():
        send_data("coeffP", coeffP_entry.get())

    def validate_coeffI():
        send_data("coeffI", coeffI_entry.get())

    def validate_offset():
        send_data("compass", offset_entry.get())

    param_window = Tk()
    param_window.title("Enter Parameters")
    param_window.configure(bg="#2E2D40")
    param_window.geometry('450x300')
    param_window.resizable(width=False, height=False)
    positionRight = int(param_window.winfo_screenwidth()/2 - 400/2)
    positionDown = int(param_window.winfo_screenheight()/2 - 300/2)
    param_window.geometry("+{}+{}".format(positionRight, positionDown))

    # Labels, champs de saisie et boutons pour les paramètres
    cap_label = Label(param_window, text="Cap souhaité (en °):", font=("", 12), justify="right", bg="#2E2D40", fg="#FFFFFF")
    cap_label.place(x=80, y=50, anchor="center")
    cap_entry = Entry(param_window, width=20, bg="#37364D", fg="#FFFFFF", justify="center")
    cap_entry.place(x=230, y=50, anchor="center")
    cap_button = Button(param_window, text="Validate", command=validate_cap, bg="#135EF2", fg="#FFFFFF", activebackground="#0D47A1", activeforeground="#FFFFFF", relief="raised", bd=2)
    cap_button.place(x=380, y=50, anchor="center")

    coeffP_label = Label(param_window, text="Coefficient P:", font=("", 12), justify="right", bg="#2E2D40", fg="#FFFFFF")
    coeffP_label.place(x=80, y=120, anchor="center")
    coeffP_entry = Entry(param_window, width=20, bg="#37364D", fg="#FFFFFF", justify="center")
    coeffP_entry.place(x=230, y=120, anchor="center")
    coeffP_button = Button(param_window, text="Validate", command=validate_coeffP, bg="#135EF2", fg="#FFFFFF", activebackground="#0D47A1", activeforeground="#FFFFFF", relief="raised", bd=2)
    coeffP_button.place(x=380, y=120, anchor="center")

    coeffI_label = Label(param_window, text="Coefficient I:", font=("", 12), justify="right", bg="#2E2D40", fg="#FFFFFF")
    coeffI_label.place(x=80, y=190, anchor="center")
    coeffI_entry = Entry(param_window, width=20, bg="#37364D", fg="#FFFFFF", justify="center")
    coeffI_entry.place(x=230, y=190, anchor="center")
    coeffI_button = Button(param_window, text="Validate", command=validate_coeffI, bg="#135EF2", fg="#FFFFFF", activebackground="#0D47A1", activeforeground="#FFFFFF", relief="raised", bd=2)
    coeffI_button.place(x=380, y=190, anchor="center")

    offset_label = Label(param_window, text="Offset boussole:", font=("", 12), justify="right", bg="#2E2D40", fg="#FFFFFF")
    offset_label.place(x=80, y=260, anchor="center")
    offset_entry = Entry(param_window, width=20, bg="#37364D", fg="#FFFFFF", justify="center")
    offset_entry.place(x=230, y=260, anchor="center")
    offset_button = Button(param_window, text="Validate", command=validate_offset, bg="#135EF2", fg="#FFFFFF", activebackground="#0D47A1", activeforeground="#FFFFFF", relief="raised", bd=2)
    offset_button.place(x=380, y=260, anchor="center")
    

    param_window.mainloop()

# Fonction pour valider les paramètres de port
def validate_port():
    global myport
    myport.Name = Port_entry.get()
    try:
        myport.Speed = int(Baud_entry.get())
    except ValueError:
        print("Vitesse doit être un entier")
        return False

    # if not myport.Name.startswith("COM") or not myport.Name[3:].isdigit():
    #     print("Port COM invalide")
    #     return False

    if myport.Speed not in [1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200]:
        print("Vitesse invalide")
        return False

    # Vérifier si le port est disponible
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if port.device == myport.Name:
            return True
    print("Port COM non disponible")
    return False

# Fonction pour lancer l'application
def RunApplication():
    global ApplicationGL
    if validate_port():
        ApplicationGL = True
        ConfWindw.destroy()
        setup_serial_connection()
        open_parameters_window()
    else:
        print("Erreur dans les paramètres de port")

# Configuration de la fenêtre Tkinter
ConfWindw = Tk()
ConfWindw.title("Configure Serial Port")
ConfWindw.configure(bg="#2E2D40")
ConfWindw.geometry('300x150')
ConfWindw.resizable(width=False, height=False)
positionRight = int(ConfWindw.winfo_screenwidth()/2 - 300/2)
positionDown = int(ConfWindw.winfo_screenheight()/2 - 150/2)
ConfWindw.geometry("+{}+{}".format(positionRight, positionDown))

# Labels et champs de saisie pour le port
Port_label = Label(ConfWindw, text="Port:", font=("", 12), justify="right", bg="#2E2D40", fg="#FFFFFF")
Port_label.place(x=50, y=30, anchor="center")
Port_entry = Entry(ConfWindw, width=20, bg="#37364D", fg="#FFFFFF", justify="center")
Port_entry.insert(INSERT, myport.Name)
Port_entry.place(x=180, y=30, anchor="center")

# Labels et champs de saisie pour la vitesse
Baud_label = Label(ConfWindw, text="Speed:", font=("", 12), justify="right", bg="#2E2D40", fg="#FFFFFF")
Baud_label.place(x=50, y=80, anchor="center")
Baud_entry = Entry(ConfWindw, width=20, bg="#37364D", fg="#FFFFFF", justify="center")
Baud_entry.insert(INSERT, str(myport.Speed))
Baud_entry.place(x=180, y=80, anchor="center")

# Bouton OK
ok_button = Button(
    ConfWindw, text="Ok", width=8, command=RunApplication, bg="#135EF2", fg="#FFFFFF",
    activebackground="#0D47A1", activeforeground="#FFFFFF", relief="raised", bd=2
)
ok_button.place(x=150, y=120, anchor="center")

ConfWindw.mainloop()
