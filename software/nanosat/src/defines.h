
//#######################################
//#                                     #
//#     DALE PY GROUP                   #
//#     Mr Panetone                     #
//#      marcelu.phd@gmail.com          #
//#                                     #
//#     01/08/2019                      #
//#######################################


#ifndef DEFINES_H
#define DEFINES_H


//#######################################
//#                                     #
//#      D E F I N E S                  # /
//#                                     #
//#######################################


// -------------ELECTRONIC  --------------------
#define MODE_GATEWAY 0 // 0=NODE 

#define HAS_WIFI 0		   // 0=WIFI init off 1=WIFI on 
#define HAS_BLUE 1		   // 0=bluetooth init off 1=bluetooth on 
#define HAS_TASKS 1		   // 0=TASKS init off 1=TASKS on 
#define HAS_TIMER 0		   // 0=TIMER init off 1=TIMER on 
#define RESET_EEP 0 // 1=erase all EEP 0=normal mode
#define HAS_SERIAL_DEBUG 1 // 1= with 0=none
#define HAS_OLED 1 // 1= with 0=none display oled

#define MAX_JSON_BUFFER 1024 // buffer to json

// BLUETOOTH

#define SERVICE_UUID "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#endif // DEFINES_H