// clang-format off
// upload_speed 115200
// board ttgo-lora32-v1

#ifndef _TTGOV1_H
#define _TTGOV1_H

#include <stdint.h>

// Hardware related definitions for TTGOv1 board

#define HAS_LORA 1       // comment out if device shall not send data via LoRa
#define CFG_sx1276_radio 1

#define HAS_DISPLAY 1 // OLED-Display on board
//#define MY_DISPLAY_FLIP  1 // uncomment this for rotated display
#define HAS_LED LED_BUILTIN
#define LED_ACTIVE_LOW 1  // Onboard LED is active when pin is LOW
#define HAS_BUTTON KEY_BUILTIN

// enable only if you want to store a local paxcount table on the device
#define HAS_SDCARD  1      // this board has an SD-card-reader/writer
// Pins for SD-card
#define SDCARD_CS    (13)
#define SDCARD_MOSI  (15)
#define SDCARD_MISO  (2)
#define SDCARD_SCLK  (14)

// Pins for I2C interface of OLED Display
#define MY_DISPLAY_SDA (4)
#define MY_DISPLAY_SCL (15)
#define MY_DISPLAY_RST (16)

// This board reports back the wrong I2C address, so we overwrite it here
#define MY_DISPLAY_ADDR 0x3C

// Pins for LORA chip SPI interface come from board file, we need some
// additional definitions for LMIC
#define LORA_IO1  (33)
#define LORA_IO2  (32)  // ref.:  https://www.thethingsnetwork.org/forum/t/big-esp32-sx127x-topic-part-3/18436

#endif
