//#######################################
//#                                     #
//#     DALE PY GROUP                   #
//#     Mr Panetone                     #
//#      marcelu.phd@gmail.com          #
//#                                     #
//#     01/08/2019                      #
//#######################################
#ifndef _MAIN_H
#define _MAIN_H

#include <esp_spi_flash.h>   // needed for reading ESP32 chip attributes
#include <esp_event_loop.h>  // needed for Wifi event handler
#include <esp32-hal-timer.h> // needed for timers
#include <esp_coexist.h>     // needed for showing coex sw version

#include "globals.h"
#include "reset.h"
#include "i2c.h"
#include "blescan.h"
#include "wifiscan.h"
#include "configmanager.h"
#include "cyclic.h"
#include "beacon_array.h"
#include "ota.h"
#include "irqhandler.h"
#include "spislave.h"
#include "sensor.h"
#include "lorawan.h"
#include "timekeeper.h"

// news libs
// news libs CILUS
#include <U8x8lib.h>
#include <LoRa.h>
#include <Wire.h>
#include <BMP180I2C.h>
#include <SPI.h>
#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Sensor.h>  // not used in this demo but required!

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiMulti.h>

#include "Arduino.h"
#include "string.h"
#include "ArrayList.h"
#include "config.h"

#endif