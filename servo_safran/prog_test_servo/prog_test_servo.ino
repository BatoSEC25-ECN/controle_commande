/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position
uint delay_between_2_pos = 30;
int pos_min = 70;
int pos_max = 180;
// position 0° est donc à (pos_max - pos_min) / 2 + pos_min soit 125° pour pos min = 70° et pos max = 180°

void setup() {
  myservo.attach(12);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  for (pos = pos_min; pos <= pos_max; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    Serial.print("Position : "); Serial.print(pos); Serial.println("°");
    delay(delay_between_2_pos);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = pos_max; pos >= pos_min; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    Serial.print("Position : "); Serial.print(pos); Serial.println("°");
    delay(delay_between_2_pos);                       // waits 15 ms for the servo to reach the position
  }
  delay(1000);
  myservo.write((pos_max - pos_min) / 2 + pos_min);
  delay(1000);
}
