//#######################################
//#                                     #
//#     DALE PY GROUP                   #
//#     Mr Panetone                     #
//#      marcelu.phd@gmail.com          #
//#                                     #
//#     01/08/2019                      #
//#######################################
// clang-format off
// upload_speed 921600
// board heltec_wifi_lora_32

#ifndef _HELTEC_H
#define _HELTEC_H

#include <stdint.h>

// Hardware related definitions for Heltec V1 LoRa-32 Board

//#define HAS_BME 1 // Enable BME sensors in general
//#define HAS_BME680 GPIO_NUM_21, GPIO_NUM_22 // SDA, SCL
//#define BME680_ADDR BME680_I2C_ADDR_PRIMARY // connect SDIO of BME680 to GND

#define HAS_LORA 1 // comment out if device shall not send data via LoRa
#define CFG_sx1276_radio 1

#define HAS_DISPLAY 1 // OLED-Display on board
#define HAS_LED LED_BUILTIN                           // white LED on board
#define HAS_BUTTON KEY_BUILTIN                        // button "PROG" on board

// Pins for I2C interface of OLED Display
#define MY_DISPLAY_SDA SDA_OLED
#define MY_DISPLAY_SCL SCL_OLED
#define MY_DISPLAY_RST RST_OLED

// Pins for LORA chip SPI interface come from board file, we need some
// additional definitions for LMIC
#define LORA_IRQ DIO0
#define LORA_IO1 DIO1
#define LORA_IO2 DIO2
#define LORA_SCK SCK
#define LORA_MISO MISO
#define LORA_MOSI MOSI
#define LORA_RST RST_LoRa
#define LORA_CS SS

// ############################## CILUS ###############pragma endregion


// #define SS      GPIO_NUM_18 //
#define RST     GPIO_NUM_14 // 
#define DI0     GPIO_NUM_26 // 
#define DHTPIN GPIO_NUM_17     // Digital pin connected to the DHT sensor 
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#endif