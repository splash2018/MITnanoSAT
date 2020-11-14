#ifndef _GLOBALS_H
#define _GLOBALS_H

// The mother of all embedded development...
#include <Arduino.h>

// Time functions
#include "microTime.h"
#include <Timezone.h>
#include <RtcDateTime.h>
#include <Ticker.h>

// std::set for unified array functions
#include <set>
#include <array>
#include <algorithm>
#include "mallocator.h"
#include <bsec.h>

// sniffing types
#define MAC_SNIFF_WIFI 0
#define MAC_SNIFF_BLE 1

// bits in payloadmask for filtering payload data
#define GPS_DATA (0x01)
#define ALARM_DATA (0x02)
#define MEMS_DATA (0x04)
#define COUNT_DATA (0x08)
#define SENSOR1_DATA (0x10)
#define SENSOR2_DATA (0x20)
#define SENSOR3_DATA (0x40)
#define BATT_DATA (0x80)

// bits in configmask for device runmode control
#define GPS_MODE (0x01)
#define ALARM_MODE (0x02)
#define BEACON_MODE (0x04)
#define UPDATE_MODE (0x08)
#define FILTER_MODE (0x10)
#define ANTENNA_MODE (0x20)
#define BLE_MODE (0x40)
#define SCREEN_MODE (0x80)

// length of display buffer for lmic event messages
#define LMIC_EVENTMSG_LEN 17

// I2C bus access control
#define I2C_MUTEX_LOCK()                                                       \
  (xSemaphoreTake(I2Caccess, pdMS_TO_TICKS(DISPLAYREFRESH_MS)) == pdTRUE)
#define I2C_MUTEX_UNLOCK() (xSemaphoreGive(I2Caccess))

enum sendprio_t { prio_low, prio_normal, prio_high };
enum timesource_t { _gps, _rtc, _lora, _unsynced };

enum runmode_t {
  RUNMODE_POWERCYCLE = 0,
  RUNMODE_NORMAL,
  RUNMODE_WAKEUP,
  RUNMODE_UPDATE
};

// Struct holding devices's runtime configuration
typedef struct {
  uint8_t loradr;      // 0-15, lora datarate
  uint8_t txpower;     // 2-15, lora tx power
  uint8_t adrmode;     // 0=disabled, 1=enabled
  uint8_t screensaver; // 0=disabled, 1=enabled
  uint8_t screenon;    // 0=disabled, 1=enabled
  uint8_t countermode; // 0=cyclic unconfirmed, 1=cumulative, 2=cyclic confirmed
  int16_t rssilimit;   // threshold for rssilimiter, negative value!
  uint8_t sendcycle;   // payload send cycle [seconds/2]
  uint8_t wifichancycle; // wifi channel switch cycle [seconds/100]
  uint8_t blescantime;   // BLE scan cycle duration [seconds]
  uint8_t blescan;       // 0=disabled, 1=enabled
  uint8_t wifiscan;      // 0=disabled, 1=enabled
  uint8_t wifiant;       // 0=internal, 1=external (for LoPy/LoPy4)
  uint8_t vendorfilter;  // 0=disabled, 1=enabled
  uint8_t rgblum;        // RGB Led luminosity (0..100%)
  uint8_t monitormode;   // 0=disabled, 1=enabled
  uint8_t runmode;       // 0=normal, 1=update
  uint8_t payloadmask;   // bitswitches for payload data
  char version[10];      // Firmware version
  uint8_t
      bsecstate[BSEC_MAX_STATE_BLOB_SIZE + 1]; // BSEC state for BME680 sensor
} configData_t;

// Struct holding payload for data send queue
typedef struct {
  uint8_t MessageSize;
  uint8_t MessagePort;
  sendprio_t MessagePrio;
  uint8_t Message[PAYLOAD_BUFFER_SIZE];
} MessageBuffer_t;

typedef struct {
  int32_t latitude;
  int32_t longitude;
  uint8_t satellites;
  uint16_t hdop;
  int16_t altitude;
} gpsStatus_t;

typedef struct {
  float iaq;             // IAQ signal
  uint8_t iaq_accuracy;  // accuracy of IAQ signal
  float temperature;     // temperature signal
  float humidity;        // humidity signal
  float pressure;        // pressure signal
  float raw_temperature; // raw temperature signal
  float raw_humidity;    // raw humidity signal
  float gas;             // raw gas sensor signal
} bmeStatus_t;

typedef struct {
  float pm10;
  float pm25;
} sdsStatus_t;

extern std::set<uint16_t, std::less<uint16_t>, Mallocator<uint16_t>> macs;
extern std::array<uint64_t, 0xff>::iterator it;
extern std::array<uint64_t, 0xff> beacons;

extern configData_t cfg;                       // current device configuration
extern char lmic_event_msg[LMIC_EVENTMSG_LEN]; // display buffer
extern uint8_t volatile channel;               // wifi channel rotation counter
extern uint8_t batt_level;                     // display value
extern uint16_t volatile macs_total, macs_wifi, macs_ble; // display values
extern bool volatile TimePulseTick; // 1sec pps flag set by GPS or RTC
extern timesource_t timeSource;
extern hw_timer_t *displayIRQ, *matrixDisplayIRQ, *ppsIRQ;
extern SemaphoreHandle_t I2Caccess;
extern TaskHandle_t irqHandlerTask, ClockTask;
extern TimerHandle_t WifiChanTimer;
extern Timezone myTZ;
extern RTC_DATA_ATTR runmode_t RTC_runmode;

#endif