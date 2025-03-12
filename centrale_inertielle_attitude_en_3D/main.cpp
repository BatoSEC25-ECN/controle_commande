#include <Arduino.h>
#include <driver/i2c.h>
#include <esp_log.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "MPU6050.h"
#include "MPU6050_6Axis_MotionApps20.h"

#define PIN_SDA 21
#define PIN_CLK 22

/*
Un AHRS (Attitude and Heading Reference System) est un ensemble 
de capteurs sur 3 axes permettant de définir la position angulaire 
d'un avion dans l'espace grâce aux accélérations et aux champs magnétiques qu'ils subissent.

Ce système vient en remplacement des systèmes traditionnels à base de gyroscopes. 
Il est plus fiable et plus précis[réf. nécessaire]. Il est constitué par un ensemble 
de MEMS (gyromètres, accéléromètres, magnétomètres). Ce sont des dispositifs utilisant 
des vibrations dont les variations permettent de mesurer des changements de direction. 
L'accélération de la gravité permet de donner une référence verticale initiale mais également en vol.

Un calculateur permet de déduire assiette et cap ainsi que corriger les erreurs par différentes techniques.

Des entrées externes telles que des données sur la vitesse peuvent être utilisées pour en améliorer encore la précision.

Un système typique va comprendre :
    3 accéléromètres pour les accélérations longitudinales
    3 gyromètres pour les accélérations angulaires
    3 magnétomètres pour les références de cap
*/

Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
uint16_t packetSize = 42;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU

MPU6050 mpu = MPU6050();

void setup() {
	Serial.begin(115200);
	// Serial.println("Coucou");

	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = (gpio_num_t)PIN_SDA;
	conf.scl_io_num = (gpio_num_t)PIN_CLK;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 400000;
	ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));

	// Initialize MPU6050
	mpu.initialize();
	mpu.dmpInitialize();

	// This need to be setup individually
	mpu.setXGyroOffset(220);
	mpu.setYGyroOffset(76);
	mpu.setZGyroOffset(-85);
	mpu.setZAccelOffset(1788);

    // mpu.CalibrateAccel(6);
    // mpu.CalibrateGyro(6);

	mpu.setDMPEnabled(true);
}

void loop() {
	mpuIntStatus = mpu.getIntStatus();
	// get current FIFO count
	fifoCount = mpu.getFIFOCount();

	// Vérifiez si de nouvelles données DMP sont disponibles
	if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
		// reset so we can continue cleanly
		mpu.resetFIFO();

	// otherwise, check for DMP data ready interrupt frequently)
	} else if (mpuIntStatus & 0x02) {
		// wait for correct available data length, should be a VERY short wait
		while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

		// read a packet from FIFO
		mpu.getFIFOBytes(fifoBuffer, packetSize);

		// Récupérez les données du quaternion
		mpu.dmpGetQuaternion(&q, fifoBuffer);
		// mpu.dmpGetGravity(&gravity, &q);
		// mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
		// Serial.printf("YAW: %3.1f, ", ypr[0] * 180/M_PI);
		// Serial.printf("PITCH: %3.1f, ", ypr[1] * 180/M_PI);
		// Serial.printf("ROLL: %3.1f \n", ypr[2] * 180/M_PI);

		// #### Envoi des données pour Processing ####
		// Convertissez les quaternions en entiers sur 16 bits
		int16_t q0 = (int16_t)(q.w * 16384);
		int16_t q1 = (int16_t)(q.x * 16384);
		int16_t q2 = (int16_t)(q.y * 16384);
		int16_t q3 = (int16_t)(q.z * 16384);

		// Envoi les données au format attendu
		Serial.write('$');                     // Début de paquet
		Serial.write((uint8_t)2);              // Type de paquet

		Serial.write((q0 >> 8) & 0xFF);        // Octet de poids fort de q0
		Serial.write(q0 & 0xFF);               // Octet de poids faible de q0
		Serial.write((q1 >> 8) & 0xFF);        // Octet de poids fort de q1
		Serial.write(q1 & 0xFF);               // Octet de poids faible de q1
		Serial.write((q2 >> 8) & 0xFF);        // Octet de poids fort de q2
		Serial.write(q2 & 0xFF);               // Octet de poids faible de q2
		Serial.write((q3 >> 8) & 0xFF);        // Octet de poids fort de q3
		Serial.write(q3 & 0xFF);               // Octet de poids faible de q3

		Serial.write('\r');                    // Retour chariot
		Serial.write('\n');                    // Nouvelle ligne

		// #### Envoi des données pour Python ####
		// Serial.printf("%f,%f,%f,%f\n", q.w, q.x, q.y, q.z);
	}
}
