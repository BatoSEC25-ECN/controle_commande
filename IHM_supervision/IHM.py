from tkinter import Tk, Label, Entry, Button, INSERT

# Classe pour stocker les informations du port
class SerialPort:
    def __init__(self):
        self.Name = "COM3"
        self.Speed = 9600

myport = SerialPort()
ApplicationGL = False

# Fonction pour envoyer les données d'un champ

def send_data(field_name, field_value):
    print(f"Sending {field_name}: {field_value}")  # Remplace par l'envoi réel si nécessaire

# Fonction pour afficher la seconde page (paramètres)
def open_parameters_window():
    def validate_offset():
        send_data("Offset", offset_entry.get())

    def validate_coef():
        send_data("Coef P", coef_entry.get())

    def validate_cap():
        send_data("Cap", cap_entry.get())

    param_window = Tk()
    param_window.title("Enter Parameters")
    param_window.configure(bg="#2E2D40")
    param_window.geometry('400x300')
    param_window.resizable(width=False, height=False)
    positionRight = int(param_window.winfo_screenwidth()/2 - 400/2)
    positionDown = int(param_window.winfo_screenheight()/2 - 300/2)
    param_window.geometry("+{}+{}".format(positionRight, positionDown))

    # Labels, champs de saisie et boutons pour les paramètres
    offset_label = Label(param_window, text="Offset:", font=("", 12), justify="right", bg="#2E2D40", fg="#FFFFFF")
    offset_label.place(x=50, y=50, anchor="center")
    offset_entry = Entry(param_window, width=20, bg="#37364D", fg="#FFFFFF", justify="center")
    offset_entry.place(x=200, y=50, anchor="center")
    offset_button = Button(param_window, text="Validate", command=validate_offset, bg="#135EF2", fg="#FFFFFF", activebackground="#0D47A1", activeforeground="#FFFFFF", relief="raised", bd=2)
    offset_button.place(x=350, y=50, anchor="center")

    coef_label = Label(param_window, text="Coef P:", font=("", 12), justify="right", bg="#2E2D40", fg="#FFFFFF")
    coef_label.place(x=50, y=120, anchor="center")
    coef_entry = Entry(param_window, width=20, bg="#37364D", fg="#FFFFFF", justify="center")
    coef_entry.place(x=200, y=120, anchor="center")
    coef_button = Button(param_window, text="Validate", command=validate_coef, bg="#135EF2", fg="#FFFFFF", activebackground="#0D47A1", activeforeground="#FFFFFF", relief="raised", bd=2)
    coef_button.place(x=350, y=120, anchor="center")

    cap_label = Label(param_window, text="Cap:", font=("", 12), justify="right", bg="#2E2D40", fg="#FFFFFF")
    cap_label.place(x=50, y=190, anchor="center")
    cap_entry = Entry(param_window, width=20, bg="#37364D", fg="#FFFFFF", justify="center")
    cap_entry.place(x=200, y=190, anchor="center")
    cap_button = Button(param_window, text="Validate", command=validate_cap, bg="#135EF2", fg="#FFFFFF", activebackground="#0D47A1", activeforeground="#FFFFFF", relief="raised", bd=2)
    cap_button.place(x=350, y=190, anchor="center")

    param_window.mainloop()

# Fonction pour lancer l'application
def RunApplication():
    global ApplicationGL
    myport.Name = Port_entry.get()
    myport.Speed = Baud_entry.get()
    ApplicationGL = True
    ConfWindw.destroy()
    open_parameters_window()

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
