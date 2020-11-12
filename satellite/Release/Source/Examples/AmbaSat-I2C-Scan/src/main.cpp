/*******************************************************************************
* AmbaSat-1 I2C Scan
* 25th January 2020
* Version 1.0
* Filename: main.cpp
*
* Copyright (c) 2020 AmbaSat Ltd
* https://ambasat.com
*
* Licensed under Creative Commons Attribution-ShareAlike 3.0
*
* Use this application to scan the I2C bus for connected devices
* Based on code written by Nick Gammon
*
* ******************************************************************************/

#include <Arduino.h>
#include <Wire.h>

// ==============================================================================
void setup() {
    unsigned int response;
    
    Serial.begin (9600);

    Serial.println ();
    Serial.println ("AmbaSat-1 Scan I2C Bus");
    byte count = 0;
    
    Wire.begin();

    for (byte i = 8; i < 120; i++)
    {
        Wire.beginTransmission (i);

        response = Wire.endTransmission();      
      
        Serial.print (i, DEC);
        Serial.println ("");
        Serial.println(response);
        
        if (response == 0)
        {
            Serial.print ("Found address: ");
            Serial.print (i, DEC);
            Serial.print (" (0x");
            Serial.print (i, HEX);
            Serial.println (")");
            count++;
        } 
    } 
    
    Serial.println ("Done.");
    Serial.print ("Found ");
    Serial.print (count, DEC);
    Serial.println (" device(s).");

}

// ==============================================================================
void loop() {
}