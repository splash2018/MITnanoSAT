//#######################################
//#                                     #
//#     Jcunion group                   #
//#     Eng Marcelo Anjos               #
//#     marcelo@airspacedefense.org     #
//#     marcelu.phd@gmail.com           #
//#     16/01/2013                      #
//#######################################

#include <Arduino.h> //use ibrary arduino
#include <Servo.h> // use servo motor library

Servo servo1;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  servo1.attach(8);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    servo1.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }

}
