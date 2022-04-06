#include <Servo.h>
 
Servo myservo_pan;  // create servo object to control a servo
Servo myservo_tilt;  // create servo object to control a servo

int pos = 90;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin_pan = 16;
int servoPin_tilt = 2;

 
void setup() {
  myservo_pan.attach(servoPin_pan, 2); // attaches the servo on pin to the servo object, for selected timer
  myservo_tilt.attach(servoPin_tilt, 3); // attaches the servo on pin to the servo object, for selected timer
  
  myservo_pan.write(pos);    // tell servo to go to position in variable 'pos'
  myservo_tilt.write(pos);    // tell servo to go to position in variable 'pos'
  pinMode(servoPin_pan, OUTPUT);
  pinMode(servoPin_tilt, OUTPUT);
  digitalWrite(servoPin_pan, LOW);
  digitalWrite(servoPin_tilt, LOW);
}
 
void loop() {
 
  for (; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    digitalWrite(servoPin_tilt, LOW);
    myservo_pan.write(pos);    // tell servo to go to position in variable 'pos'
    delay(150);             // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    digitalWrite(servoPin_tilt, LOW);
    myservo_pan.write(pos);    // tell servo to go to position in variable 'pos'
    delay(150);             // waits 15ms for the servo to reach the position
  }
  myservo_pan.write(90);    // tell servo to go to position in variable 'pos'
  delay(150);             // waits 15ms for the servo to reach the position
  for (pos = 90; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    digitalWrite(servoPin_pan, LOW);
    myservo_tilt.write(pos);    // tell servo to go to position in variable 'pos'
    delay(150);             // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    digitalWrite(servoPin_pan, LOW);
    myservo_tilt.write(pos);    // tell servo to go to position in variable 'pos'
    delay(150);             // waits 15ms for the servo to reach the position
  }
  myservo_tilt.write(90);    // tell servo to go to position in variable 'pos'

}
