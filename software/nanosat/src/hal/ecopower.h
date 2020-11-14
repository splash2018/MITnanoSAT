// clang-format off
// upload_speed 921600
// board esp32dev

#ifndef _GENERIC_H
#define _GENERIC_H

#include <stdint.h>

#define HAS_LORA 1 // comment out if device shall not send data via LoRa or has no LoRa
#define CFG_sx1276_radio 1 // select LoRa chip

//#define DISABLE_BROWNOUT 1 // comment out if you want to keep brownout feature

#define HAS_DISPLAY 1
//#define MY_DISPLAY_FLIP  1 // use if display is rotated
#define BAT_MEASURE_ADC ADC1_GPIO35_CHANNEL // battery probe GPIO pin -> ADC1_CHANNEL_7
#define BAT_VOLTAGE_DIVIDER ((82.0+220.0)/82.0) // 82k + 220k 1%
//#define BAT_MEASURE_EN   EXT_POWER_SW  // Turn power on for measurement

#define EXT_POWER_SW    15            // Switch VDD on pin JP10, needed for RTC DS3231
#define EXT_POWER_ON    0
#define EXT_POWER_OFF   1

#define HAS_LED (2) // on board green LED
#define HAS_TWO_LED (12) // on board red LED
//#define HAS_BUTTON (0) // on board button -> don't use, is same as RTC_INT!

// Pins for I2C interface of OLED Display
#define MY_DISPLAY_SDA SDA
#define MY_DISPLAY_SCL SCL
#define MY_DISPLAY_RST NOT_A_PIN

// Settings for on board DS3231 RTC chip
// note: to use RTC_INT, capacitor 100nF next to red LED must be removed to sharpen interrupt signal slope
// and setting EXT_POWER_ON is needed (this is done in main.cpp)
#define HAS_RTC MY_DISPLAY_SDA, MY_DISPLAY_SCL // SDA, SCL
#define RTC_INT GPIO_NUM_0 //

// Settings for IF482 interface
#define HAS_IF482 9600, SERIAL_7E1, GPIO_NUM_3, GPIO_NUM_1 // RX, TX

// Settings for DCF77 interface
//#define HAS_DCF77 GPIO_NUM_14 // JP8 #13
//#define DCF77_ACTIVE_LOW 1

// Pins for LORA chip SPI interface, reset line and interrupt lines
#define LORA_SCK  SCK 
#define LORA_CS   SS
#define LORA_MISO MISO
#define LORA_MOSI MOSI
#define LORA_RST  (17)
#define LORA_IRQ  (16)
#define LORA_IO1  (14) // JP8 #13 to be external wired
#define LORA_IO2  LMIC_UNUSED_PIN

#endif