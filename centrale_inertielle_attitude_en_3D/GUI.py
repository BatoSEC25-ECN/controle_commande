import math
import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QLineEdit, QLabel, QPushButton, QWidget
from PyQt5.QtOpenGL import QGLWidget
from OpenGL.GL import *
from OpenGL.GLU import *
import serial
import threading

# Classes pour les paramètres du port série et les données IMU
class PortSettings:
    Name = "COM5"
    Speed = 115200
    Timeout = 2

class IMU:
    Roll = 0
    Pitch = 0
    Yaw = 0

myport = PortSettings()
myimu = IMU()

# Variables globales
serial_object = None
synced = 0
serial_count = 0
teapot_packet = [0] * 12
q = [0.0] * 4  # Quaternions
ypr = [0.0] * 3  # Yaw, Pitch, Roll
gravity = [0.0] * 3

# Fonction de connexion série
def SerialConnection():
    global serial_object
    serial_object = serial.Serial(myport.Name, baudrate=myport.Speed, timeout=myport.Timeout)

def ReadData():
    global synced, serial_count, teapot_packet, q, ypr
    while True:
        ch = serial_object.read(1)
        ch = ord(ch)
        if synced == 0 and ch != ord('$'):
            continue
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

            if serial_count == 12:
                serial_count = 0
                q[0] = ((teapot_packet[2] << 8) | teapot_packet[3]) / 16384.0
                q[1] = ((teapot_packet[4] << 8) | teapot_packet[5]) / 16384.0
                q[2] = ((teapot_packet[6] << 8) | teapot_packet[7]) / 16384.0
                q[3] = ((teapot_packet[8] << 8) | teapot_packet[9]) / 16384.0
                for i in range(4):
                    if q[i] >= 2:
                        q[i] = -4 + q[i]

                gravity[0] = 2 * (q[1] * q[3] - q[0] * q[2])
                gravity[1] = 2 * (q[0] * q[1] + q[2] * q[3])
                gravity[2] = q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3]

                ypr[0] = math.atan2(2 * q[1] * q[2] - 2 * q[0] * q[3],
                                    2 * q[0] * q[0] + 2 * q[1] * q[1] - 1)
                ypr[1] = math.atan(gravity[0] / math.sqrt(gravity[1] * gravity[1] + gravity[2] * gravity[2]))
                ypr[2] = math.atan(gravity[1] / math.sqrt(gravity[0] * gravity[0] + gravity[2] * gravity[2]))

# Widget OpenGL
class OpenGLWidget(QGLWidget):
    def initializeGL(self):
        glClearColor(46 / 255, 45 / 255, 64 / 255, 1)
        glEnable(GL_DEPTH_TEST)
        glDepthFunc(GL_LEQUAL)
        gluPerspective(100, (640 / 480), 0.1, 50.0)
        glTranslatef(0.0, 0.0, -5)

    def paintGL(self):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glLoadIdentity()
        gluPerspective(90, (640 / 480), 0.1, 50.0)
        glTranslatef(0.0, 0.0, -5)

        # Convert quaternion to axis-angle representation
        angle = 2 * math.acos(q[0])
        x = q[1] / math.sqrt(1 - q[0] * q[0])
        y = q[2] / math.sqrt(1 - q[0] * q[0])
        z = q[3] / math.sqrt(1 - q[0] * q[0])

        # Apply rotation
        glRotatef(math.degrees(angle), x, z, -y)
        self.drawCube()

    def drawCube(self):
        glBegin(GL_QUADS)
        # Face avant
        glColor3f(1, 0, 0)
        glVertex3f(-0.5, -0.5, -0.5)
        glVertex3f(0.5, -0.5, -0.5)
        glVertex3f(0.5, 0.5, -0.5)
        glVertex3f(-0.5, 0.5, -0.5)
        glEnd()

# Interface principale
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("IMU Visualizer")
        self.setGeometry(100, 100, 800, 600)

        self.main_widget = QWidget()
        self.setCentralWidget(self.main_widget)
        self.layout = QVBoxLayout(self.main_widget)

        # Champs de saisie pour le port et la vitesse
        self.port_label = QLabel("Port:")
        self.port_entry = QLineEdit(myport.Name)
        self.speed_label = QLabel("Speed:")
        self.speed_entry = QLineEdit(str(myport.Speed))
        self.start_button = QPushButton("Start")

        # Connecter le bouton à l'action
        self.start_button.clicked.connect(self.startApplication)

        # Ajouter les widgets à la mise en page
        self.layout.addWidget(self.port_label)
        self.layout.addWidget(self.port_entry)
        self.layout.addWidget(self.speed_label)
        self.layout.addWidget(self.speed_entry)
        self.layout.addWidget(self.start_button)

        # Ajouter le widget OpenGL
        self.opengl_widget = OpenGLWidget()
        self.layout.addWidget(self.opengl_widget)

    def startApplication(self):
        myport.Name = self.port_entry.text()
        myport.Speed = int(self.speed_entry.text())
        threading.Thread(target=SerialConnection).start()
        threading.Thread(target=ReadData, daemon=True).start()


# Lancer l'application
if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
