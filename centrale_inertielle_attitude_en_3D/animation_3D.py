import math
import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
from figure import *
from tkinter import *
from tkinter import ttk
import time
import serial
import threading

ApplicationGL = False

class PortSettings:
    Name = "COM5"
    Speed = 115200
    Timeout = 2
class IMU:
    Roll = 0
    Pitch = 0
    Yaw = 0



myport = PortSettings()
myimu  = IMU()

# Variables globales
synced = 0
serial_count = 0
teapot_packet = [0] * 12
q = [0.0] * 4  # Quaternions
ypr = [0.0] * 3  # Yaw, Pitch, Roll
gravity = [0.0] * 3

def RunAppliction():
    global ApplicationGL
    myport.Name = Port_entry.get()
    myport.Speed = Baud_entry.get()
    ApplicationGL = True
    ConfWindw.destroy()

ConfWindw = Tk()
ConfWindw.title("Configure Serial Port")
ConfWindw.configure(bg = "#2E2D40") 
ConfWindw.geometry('300x150')
ConfWindw.resizable(width=False, height=False)
positionRight = int(ConfWindw.winfo_screenwidth()/2 - 300/2)
positionDown = int(ConfWindw.winfo_screenheight()/2 - 150/2)
ConfWindw.geometry("+{}+{}".format(positionRight, positionDown))

Port_label = Label(text = "Port:",font =("",12),justify= "right",bg = "#2E2D40",fg = "#FFFFFF")
Port_label.place(x = 50, y =30,anchor = "center")
Port_entry = Entry(width = 20,bg = "#37364D", fg = "#FFFFFF", justify = "center")
Port_entry.insert(INSERT,myport.Name)
Port_entry.place(x = 180, y = 30,anchor = "center")

Baud_label = Label(text = "Speed:",font =("",12),justify= "right",bg = "#2E2D40",fg = "#FFFFFF")
Baud_label.place(x = 50, y =80,anchor = "center")
Baud_entry = Entry(width = 20,bg = "#37364D", fg = "#FFFFFF", justify = "center")
Baud_entry.insert(INSERT,str(myport.Speed))
Baud_entry.place(x = 180, y = 80,anchor = "center")

ok_button = Button(text = "Ok",width = 8,command = RunAppliction,bg="#135EF2",fg ="#FFFFFF")
ok_button.place(x = 150, y = 120,anchor="center")

def InitPygame():
    global display
    pygame.init()
    display = (640,480)
    pygame.display.set_mode(display, DOUBLEBUF|OPENGL)
    pygame.display.set_caption('IMU visualizer   (Press Esc to exit)')


def InitGL():
    glClearColor((1.0/255*46),(1.0/255*45),(1.0/255*64),1)
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)

    gluPerspective(100, (display[0]/display[1]), 0.1, 50.0)
    glTranslatef(0.0,0.0, -5)


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


def DrawGL():

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    glLoadIdentity() 
    gluPerspective(90, (display[0]/display[1]), 0.1, 50.0)
    glTranslatef(0.0, 0.0, -5)   

    # Convert quaternion to axis-angle representation
    angle = 2 * math.acos(q[0])
    x = q[1] / math.sqrt(1 - q[0] * q[0])
    y = q[2] / math.sqrt(1 - q[0] * q[0])
    z = q[3] / math.sqrt(1 - q[0] * q[0])
    
    # Apply rotation
    glRotatef(math.degrees(angle), x, z, -y)

    DrawText("  Roll: {}°  Pitch: {}°  Yaw: {}°".format(round(myimu.Roll, 1), round(myimu.Pitch, 1), round(myimu.Yaw, 1)))
    DrawEverything(2, 2, 2)
    pygame.display.flip()

def SerialConnection ():
    global serial_object
    serial_object = serial.Serial( myport.Name, baudrate= myport.Speed, timeout = myport.Timeout)

def ReadData():
    global synced, serial_count, teapot_packet, q, ypr
    while True:  # Tant qu'il y a des données disponibles
        ch = serial_object.read(1)  # Lecture d'un seul octet
        ch = ord(ch)  # Conversion de l'octet en entier pour manipulation

        if synced == 0 and ch != ord('$'):
            continue  # Synchronisation initiale
        synced = 1

        if ((serial_count == 1 and ch != 2)
                or (serial_count == 12 and ch != ord('\r'))
                or (serial_count == 13 and ch != ord('\n'))):
            serial_count = 0
            synced = 0
            continue

        if serial_count > 0 or ch == ord('$'):  
            teapot_packet[serial_count] = ch
            serial_count += 1

            if serial_count == 12:  # Paquet complet
                serial_count = 0  # Réinitialisation pour le prochain paquet

                # Extraction des quaternions depuis le paquet
                q[0] = ((teapot_packet[2] << 8) | teapot_packet[3]) / 16384.0
                q[1] = ((teapot_packet[4] << 8) | teapot_packet[5]) / 16384.0
                q[2] = ((teapot_packet[6] << 8) | teapot_packet[7]) / 16384.0
                q[3] = ((teapot_packet[8] << 8) | teapot_packet[9]) / 16384.0

                # Ajustement si un quaternion dépasse la plage acceptable
                for i in range(4):
                    if q[i] >= 2:
                        q[i] = -4 + q[i]

                # Calcul des vecteurs de gravité
                gravity[0] = 2 * (q[1] * q[3] - q[0] * q[2])
                gravity[1] = 2 * (q[0] * q[1] + q[2] * q[3])
                gravity[2] = q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]

                # Calcul Yaw, Pitch, Roll
                ypr[0] = math.atan2(2 * q[1] * q[2] - 2 * q[0] * q[3],
                                   2 * q[0] * q[0] + 2 * q[1] * q[1] - 1)
                ypr[1] = math.atan(gravity[0] / math.sqrt(gravity[1] * gravity[1] + gravity[2] * gravity[2]))
                ypr[2] = math.atan(gravity[1] / math.sqrt(gravity[0] * gravity[0] + gravity[2] * gravity[2]))

                # Affichage des résultats
                # print(f"Yaw: {math.degrees(ypr[0]):.2f}, Roll: {math.degrees(ypr[1]):.2f}, Pitch: {math.degrees(ypr[2]):.2f}")

                # Update de l'oject IMU 3D
                myimu.Yaw = math.degrees(ypr[0])
                myimu.Roll = -math.degrees(ypr[1])
                myimu.Pitch = math.degrees(ypr[2])

def main():
    ConfWindw.mainloop()
    if ApplicationGL == True:
        InitPygame()
        InitGL()
 
        try:
            SerialConnection()
            myThread1 = threading.Thread(target = ReadData)
            myThread1.daemon = True
            myThread1.start() 
            while True:
                event = pygame.event.poll()
                if event.type == QUIT or (event.type == KEYDOWN and event.key == K_ESCAPE):
                    pygame.quit()
                    break 

                DrawGL()
                pygame.time.wait(10)

        except:
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
            DrawText("Sorry, something is wrong :c")
            pygame.display.flip()
            time.sleep(5)

if __name__ == '__main__': main()