//#######################################
//#                                     #
//#     Jcunion group                   #
//#     Eng Marcelo Anjos               #
//#     marcelo@airspacedefense.org     #
//#     marcelu.phd@gmail.com           #
//#     16/01/2013                      #
//#######################################

#include <Arduino.h>
#include <Servo.h>

Servo myservoa, myservob, myservoc, myservod; // create servo object to control a servo
                                              // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0; // variable to store the servo position

void setup()
{
  myservoa.attach(8);                                         //the pin for the servoa control
  myservob.attach(9);                                         //the pin for the servob control
  myservoc.attach(10);                                        //the pin for the servoc control
  myservod.attach(11);                                        //the pin for the servod control
  Serial.println("multi-servo-delimit-test-dual-input-auto"); // so I can keep track of what is loaded
}

void loop()
{
  for (int number = 0; number <= 4; number += 1)
  { // goes from 0 degrees to 180 degrees
    for (pos = 0; pos <= 180; pos += 1)
    { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      switch (number)
      {
      case 1:
        myservoa.write(pos);
        delay(15); // waits 15ms for the servo to reach the position
        break;

      case 2:
        myservob.write(pos);
        delay(15); // waits 15ms for the servo to reach the position
        break;

      case 3:
        myservoc.write(pos);
        delay(15); // waits 15ms for the servo to reach the position
        break;

      case 4:
        myservod.write(pos);
        delay(15); // waits 15ms for the servo to reach the position
        break;

      default:
        // statements
        break;
      }
    }
  }

  for (int number = 0; number <= 4; number += 1)
  { // goes from 0 degrees to 180 degrees
    for (pos = 180; pos >= 0; pos -= 1)
    { // goes from 180 degrees to 0 degrees
      switch (number)
      {
      case 1:
        myservoa.write(pos);
        delay(15); // waits 15ms for the servo to reach the position
        break;

      case 2:
        myservob.write(pos);
        delay(15); // waits 15ms for the servo to reach the position
        break;

      case 3:
        myservoc.write(pos);
        delay(15); // waits 15ms for the servo to reach the position
        break;

      case 4:
        myservod.write(pos);
        delay(15); // waits 15ms for the servo to reach the position
        break;

      default:
        // statements
        break;
      } // tell servo to go to position in variable 'pos'
        // waits 15ms for the servo to reach the position
    }
  }
}
