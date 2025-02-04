/* Sweep using writeMicroseconds
 This example uses direct pulse widths to control the servo
*/

#include <Servo.h>

Servo myservo;  // Create a servo object to control a servo

int pulse_min = 1070;   // Minimum pulse width in microseconds (µs)
int pulse_max = 1690;  // Maximum pulse width in microseconds (µs)
int pulse_center = (pulse_max - pulse_min) / 2 + pulse_min; // Center position

uint delay_between_2_pos = 16; // Delay between two positions in milliseconds

void setup() {
  myservo.attach(28);  // Attach the servo on pin 28
  Serial.begin(9600);  // Initialize serial communication for debugging
}

void loop() {
  // Move the servo from pulse_min to pulse_max
  for (int pulse = pulse_min; pulse <= pulse_max; pulse += 10) { // Increment by 10µs
    myservo.writeMicroseconds(pulse); // Set the servo position using pulse width
    Serial.print("Pulse Width: "); Serial.print(pulse); Serial.println(" µs");
    delay(delay_between_2_pos); // Wait for the servo to reach the position
  }

  // Move the servo from pulse_max to pulse_min
  for (int pulse = pulse_max; pulse >= pulse_min; pulse -= 10) { // Decrement by 10µs
    myservo.writeMicroseconds(pulse); // Set the servo position using pulse width
    Serial.print("Pulse Width: "); Serial.print(pulse); Serial.println(" µs");
    delay(delay_between_2_pos); // Wait for the servo to reach the position
  }

  // Pause for 1 second after completing the full sweep
  delay(1000);

  // Move the servo to the center position
  myservo.writeMicroseconds(pulse_center);
  Serial.print("Moving to center position: "); Serial.print(pulse_center); Serial.println(" µs");
  
  // Pause for 5 seconds at the center position
  delay(5000);
}
