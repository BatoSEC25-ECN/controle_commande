import math
import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
import csv
import time
from tkinter import *
from tkinter import filedialog

# Variables globales
ApplicationGL = False
data = []
index = 0  # Index pour parcourir les données

# Charger les données du fichier CSV
def load_csv_file():
    global data
    file_path = filedialog.askopenfilename(title="Select CSV File", filetypes=[("CSV files", "*.csv")])
    if file_path:
        with open(file_path, 'r') as file:
            reader = csv.DictReader(file)
            data = list(reader)
            print(f"{len(data)} lignes chargées depuis le fichier {file_path}")

# Initialisation de Pygame
def InitPygame():
    global display
    pygame.init()
    display = (800, 600)
    pygame.display.set_mode(display, DOUBLEBUF | OPENGL)
    pygame.display.set_caption('Données CSV - Visualisation 3D')

# Initialisation OpenGL
def InitGL():
    glClearColor(0.18, 0.18, 0.25, 1)
    glEnable(GL_DEPTH_TEST)
    gluPerspective(100, (display[0] / display[1]), 0.1, 50.0)
    glTranslatef(0.0, 0.0, -5)

def DrawText(textString):     
    font = pygame.font.SysFont ("Courier New",25, True)
    textSurface = font.render(textString, True, (255,255,0), (46,45,64,255))     
    textData = pygame.image.tostring(textSurface, "RGBA", True)         
    glDrawPixels(textSurface.get_width(), textSurface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, textData)    

def DrawCylinder(base_radius, top_radius, height, slices, translation):
    glPushMatrix()
    glTranslatef(0, 0, translation)  # Déplacement le long de l'axe Z
    step = 2 * math.pi / slices
    glBegin(GL_TRIANGLE_STRIP)
    for i in range(slices + 1):
        angle = i * step
        x = math.cos(angle)
        y = math.sin(angle)
        glVertex3f(base_radius * x, base_radius * y, 0)  # Base
        glVertex3f(top_radius * x, top_radius * y, height)  # Top
    glEnd()
    glPopMatrix()

def DrawBoard(scale_x, scale_y, scale_z):
    # Corps principal en rouge
    glPushMatrix()
    glColor4f(1.0, 0.0, 0.0, 0.8)  # Rouge avec transparence
    glScalef(0.1 * scale_x, 0.1 * scale_y, 2.0 * scale_z)  # Redimensionnement
    glBegin(GL_QUADS)

    # Face avant
    glVertex3f(-0.5, -0.5, 0.5)
    glVertex3f(0.5, -0.5, 0.5)
    glVertex3f(0.5, 0.5, 0.5)
    glVertex3f(-0.5, 0.5, 0.5)

    # Face arrière
    glVertex3f(-0.5, -0.5, -0.5)
    glVertex3f(-0.5, 0.5, -0.5)
    glVertex3f(0.5, 0.5, -0.5)
    glVertex3f(0.5, -0.5, -0.5)

    # Face gauche
    glVertex3f(-0.5, -0.5, -0.5)
    glVertex3f(-0.5, -0.5, 0.5)
    glVertex3f(-0.5, 0.5, 0.5)
    glVertex3f(-0.5, 0.5, -0.5)

    # Face droite
    glVertex3f(0.5, -0.5, -0.5)
    glVertex3f(0.5, 0.5, -0.5)
    glVertex3f(0.5, 0.5, 0.5)
    glVertex3f(0.5, -0.5, 0.5)

    # Face basse
    glVertex3f(-0.5, -0.5, -0.5)
    glVertex3f(-0.5, -0.5, 0.5)
    glVertex3f(0.5, -0.5, 0.5)
    glVertex3f(0.5, -0.5, -0.5)

    # Face haute
    glVertex3f(-0.5, 0.5, -0.5)
    glVertex3f(0.5, 0.5, -0.5)
    glVertex3f(0.5, 0.5, 0.5)
    glVertex3f(-0.5, 0.5, 0.5)

    glEnd()
    glPopMatrix()

def DrawAvant(scale_x, scale_y, scale_z):
    # Pointe avant en bleu
    glPushMatrix()
    glColor4f(0.0, 0.0, 1.0, 0.8)  # Bleu avec transparence
    glTranslatef(0.0, 0.0, -1.2)  # Place la pointe à l'avant
    glRotatef(0, 1, 0, 0)  # Tourne pour aligner avec l'axe Z
    DrawCylinder(0.0, 0.2 * scale_x, 0.4 * scale_y, 20, -(scale_x/1.3))  # Dessine un cône
    glPopMatrix()

def DrawAiles(scale_x, scale_y, scale_z):
    # Ailes et queue en vert
    glColor4f(0.0, 1.0, 0.0, 0.8)  # Vert avec transparence

    # Ailes
    glBegin(GL_TRIANGLES)
    glVertex3f(-1.0 * scale_x, 0.02 * scale_y, 0.3 * scale_z)
    glVertex3f(1.0 * scale_x, 0.02 * scale_y, 0.3 * scale_z)
    glVertex3f(0.0, 0.02 * scale_y, -0.8 * scale_z)
    glEnd()


def DrawQueue(scale_x, scale_y, scale_z):
    # Queue
    glBegin(GL_TRIANGLES)
    glVertex3f(0, 0.0, 0.9 * scale_z)
    glVertex3f(0, 0.0, 0.5 * scale_z)
    glVertex3f(0.0, 0.6 * scale_y, 0.9 * scale_z)
    glEnd()

def DrawEverything(scale_x, scale_y, scale_z):
    DrawBoard(scale_x, scale_y, scale_z)
    DrawAvant(scale_x, scale_y, scale_z)
    DrawAiles(scale_x, scale_y, scale_z)
    DrawQueue(scale_x, scale_y, scale_z)

# Fonction pour dessiner la scène
def DrawScene():
    global index, data
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    gluPerspective(90, (display[0] / display[1]), 0.1, 50.0)
    glTranslatef(0.0, 0.0, -5)

    # Mise à jour des rotations en fonction des données
    if index < len(data):
        row = data[index]
        accel_x = float(row["Acceleration_X"]) / 10000
        accel_y = float(row["Acceleration_Y"]) / 10000
        accel_z = float(row["Acceleration_Z"]) / 10000
        glRotatef(accel_x * 100, 1, 0, 0)  # Rotation X
        glRotatef(accel_y * 100, 0, 1, 0)  # Rotation Y
        glRotatef(accel_z * 100, 0, 0, 1)  # Rotation Z
        index += 1  # Passer à la ligne suivante
    else:
        index = 0  # Recommencer à zéro

    # DrawText("  Roll: {}°  Pitch: {}°  Yaw: {}°".format(round(myimu.Roll, 1), round(myimu.Pitch, 1), round(myimu.Yaw, 1)))
    # Dessiner un cube représentant l'objet
    DrawEverything(2, 2, 2)
    pygame.display.flip()

# Lancer l'application
def RunApplication():
    global ApplicationGL
    ApplicationGL = True
    ConfWindw.destroy()

# Fenêtre de configuration (Tkinter)
ConfWindw = Tk()
ConfWindw.title("Configuration du fichier CSV")
ConfWindw.configure(bg="#2E2D40")
ConfWindw.geometry('300x150')
ConfWindw.resizable(width=False, height=False)

load_button = Button(text="Charger CSV", command=load_csv_file, bg="#135EF2", fg="#FFFFFF")
load_button.place(x=150, y=50, anchor="center")
ok_button = Button(text="Démarrer", command=RunApplication, bg="#135EF2", fg="#FFFFFF")
ok_button.place(x=150, y=100, anchor="center")

# Boucle principale Tkinter
ConfWindw.mainloop()

# Si l'utilisateur démarre l'application
if ApplicationGL:
    InitPygame()
    InitGL()

    while True:
        for event in pygame.event.get():
            if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
                pygame.quit()
                exit()

        DrawScene()
        pygame.time.wait(100)
