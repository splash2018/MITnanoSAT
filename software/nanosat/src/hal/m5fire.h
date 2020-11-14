// clang-format off
// upload_speed 921600
// board m5stack-fire

#ifndef _M5FIRE_H
#define _M5FIRE_H

#include <stdint.h>

#define HAS_LORA 1 // comment out if device shall not send data via LoRa or has no M5 RA01 LoRa module
#define LORA_SCK  SCK
#define LORA_CS   SS
#define LORA_MISO MISO
#define LORA_MOSI MOSI
#define LORA_RST  GPIO_NUM_26
#define LORA_IRQ  GPIO_NUM_36
#define LORA_IO1  GPIO_NUM_34 // must be wired by you on PCB!
#define LORA_IO2  LMIC_UNUSED_PIN


// enable only if you want to store a local paxcount table on the device
#define HAS_SDCARD  1      // this board has an SD-card-reader/writer
#define SDCARD_CS    GPIO_NUM_4
#define SDCARD_MOSI  MOSI
#define SDCARD_MISO  MISO
#define SDCARD_SCLK  SCK

// user defined sensors
//#define HAS_SENSORS 1 // comment out if device has user defined sensors

#define CFG_sx1276_radio 1 // select LoRa chip
#define BOARD_HAS_PSRAM // use if board has external PSRAM
#define DISABLE_BROWNOUT 1 // comment out if you want to keep brownout feature

#define HAS_DISPLAY 2 // TFT-LCD, support work in progess, not ready yet
//#define MY_DISPLAY_FLIP  1 // use if display is rotated
//#define BAT_MEASURE_ADC ADC1_GPIO35_CHANNEL // battery probe GPIO pin -> ADC1_CHANNEL_7
//#define BAT_VOLTAGE_DIVIDER 2 // voltage divider 100k/100k on board

#define HAS_LED NOT_A_PIN // no on board LED (?)
#define RGB_LED_COUNT 10

#define HAS_RGB_LED SmartLed rgb_led(LED_SK6812, RGB_LED_COUNT, GPIO_NUM_15) // LED_SK6812 RGB LED on GPIO15
#define HAS_BUTTON (39) // on board button A

// GPS settings
#define HAS_GPS 0 // use on board GPS
#define GPS_SERIAL 9600, SERIAL_8N1, RXD2, TXD2 // UBlox NEO 6M RX, TX
// #define GPS_INT GPIO_NUM_35 // 30ns accurary timepulse, to be external wired on pcb: shorten R12!

// Display Settings
#define MY_DISPLAY_WIDTH 320
#define MY_DISPLAY_HEIGHT 240
#define MY_DISPLAY_INVERT 1

// setting for M5 display
#define USER_SETUP_LOADED 1
#define ILI9341_DRIVER 1
#define M5STACK // needed for TFT driver

#define TFT_MISO MISO  // SPI
#define TFT_MOSI MOSI // SPI
#define TFT_SCLK SCK  // SPI
#define TFT_CS   GPIO_NUM_14  // Chip select control
#define TFT_DC   GPIO_NUM_27  // Data Command control
#define TFT_RST  GPIO_NUM_33  // Reset
#define TFT_BL   GPIO_NUM_32  // LED back-light

#define TFT_RGB_ORDER TFT_BGR  // Colour order Blue-Green-Red

//#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
//#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
//#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
//#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts
#define SMOOTH_FONT

#define SPI_FREQUENCY  40000000

#endif