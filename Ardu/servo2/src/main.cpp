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

String readString;
Servo myservoa, myservob, myservoc, myservod;  // create servo object to control a servo

void setup() {
  Serial.begin(9600); //init comunication with usb

  //myservoa.writeMicroseconds(1500); //set initial servo position if desired

  myservoa.attach(8);  //the pin for the servoa control
  myservob.attach(9);  //the pin for the servob control
  myservoc.attach(10);  //the pin for the servoc control
  myservod.attach(11);  //the pin for the servod control
  Serial.println("multi-servo-delimit-test-dual-input-string"); // so I can keep track of what is loaded
}

void loop() {

  //expect single strings like 700a, or 1500c, or 2000d,
  //or like 30c, or 90a, or 180d,
  //or 30c,180b,70a,120d,

  if (Serial.available())  {
    char c = Serial.read();  //gets one byte from serial buffer
    if (c == ',') {
      if (readString.length() >1) {
        Serial.println(readString); //prints string to serial port out

        int n = readString.toInt();  //convert readString into a number

        // auto select appropriate value, copied from someone elses code.
        if(n >= 500)
        {
          Serial.print("writing Microseconds: ");
          Serial.println(n);
          if(readString.indexOf('a') >0) myservoa.writeMicroseconds(n);
          if(readString.indexOf('b') >0) myservob.writeMicroseconds(n);
          if(readString.indexOf('c') >0) myservoc.writeMicroseconds(n);
          if(readString.indexOf('d') >0) myservod.writeMicroseconds(n);
        }
        else
        {   
          Serial.print("writing Angle: ");
          Serial.println(n);
          if(readString.indexOf('a') >0) myservoa.write(n);
          if(readString.indexOf('b') >0) myservob.write(n);
          if(readString.indexOf('c') >0) myservoc.write(n);
          if(readString.indexOf('d') >0) myservod.write(n);
        }
         readString=""; //clears variable for new input
      }
    } 
    else {     
      readString += c; //makes the string readString
    }
  }
}