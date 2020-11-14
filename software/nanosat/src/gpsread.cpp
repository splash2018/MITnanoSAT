//#######################################
//#                                     #
//#     DALE PY GROUP                   #
//#     Mr Panetone                     #
//#      marcelu.phd@gmail.com          #
//#                                     #
//#     01/08/2019                      #
//#######################################
#if (HAS_GPS)

#include "globals.h"
#include "gpsread.h"

// Local logging tag
static const char TAG[] = __FILE__;

// we use NMEA $GPZDA sentence field 1 for time synchronization
// $GPZDA gives time for preceding pps pulse, but does not has a constant offset
TinyGPSPlus gps;
TinyGPSCustom gpstime(gps, "GPZDA", 1); // field 1 = UTC time
static const String ZDA_Request = "$EIGPQ,ZDA*39\r\n";
TaskHandle_t GpsTask;

#ifdef GPS_SERIAL
HardwareSerial GPS_Serial(1); // use UART #1
static uint16_t nmea_txDelay_ms =
    (tx_Ticks(NMEA_FRAME_SIZE, GPS_SERIAL) / portTICK_PERIOD_MS);
#else
static uint16_t nmea_txDelay_ms = 0;
#endif

// initialize and configure GPS
int gps_init(void) {

  int ret = 1;

  if (!gps_config()) {
    ESP_LOGE(TAG, "GPS chip initializiation error");
    return 0;
  }

#ifdef GPS_SERIAL
  GPS_Serial.begin(GPS_SERIAL);
  ESP_LOGI(TAG, "Using serial GPS");
#elif defined GPS_I2C
  Wire.begin(GPS_I2C, 400000); // I2C connect to GPS device with 400 KHz
  Wire.beginTransmission(GPS_ADDR);
  Wire.write(0x00);             // dummy write
  ret = Wire.endTransmission(); // check if chip is seen on i2c bus

  if (ret) {
    ESP_LOGE(TAG,
             "Quectel L76 GPS chip not found on i2c bus, bus error %d. "
             "Stopping GPS-Task.",
             ret);
    ret = 0;
  } else {
    ESP_LOGI(TAG, "Quectel L76 GPS chip found");
  }
#endif

  return ret;
} // gps_init()

// detect gps chipset type and configure it with device specific settings
int gps_config() {
  int rslt = 1; // success
#if defined GPS_SERIAL

  /* insert user configuration here, if needed */

#elif defined GPS_I2C

  /* insert user configuration here, if needed */

#endif
  return rslt;
}

// store current GPS location data in struct
void gps_storelocation(gpsStatus_t *gps_store) {
  if (gps.location.isUpdated() && gps.location.isValid() &&
      (gps.location.age() < 1500)) {
    gps_store->latitude = (int32_t)(gps.location.lat() * 1e6);
    gps_store->longitude = (int32_t)(gps.location.lng() * 1e6);
    gps_store->satellites = (uint8_t)gps.satellites.value();
    gps_store->hdop = (uint16_t)gps.hdop.value();
    gps_store->altitude = (int16_t)gps.altitude.meters();
  }
}

bool gps_hasfix() {
  // adapted from source:
  // https://github.com/hottimuc/Lora-TTNMapper-T-Beam/blob/master/fromV08/gps.cpp
  return (gps.location.isValid() && gps.location.age() < 4000 &&
          gps.hdop.isValid() && gps.hdop.value() <= 600 &&
          gps.hdop.age() < 4000 && gps.altitude.isValid() &&
          gps.altitude.age() < 4000);
}

// function to fetch current time from struct; note: this is costly
time_t get_gpstime(uint16_t *msec) {

  time_t time_sec = 0;

  // poll NMEA $GPZDA sentence
#ifdef GPS_SERIAL
  GPS_Serial.print(ZDA_Request);
  // wait for gps NMEA answer
  vTaskDelay(tx_Ticks(NMEA_FRAME_SIZE, GPS_SERIAL));
#elif defined GPS_I2C
  Wire.print(ZDA_Request);
#endif

  // did we get a current time?
  if (gpstime.isUpdated() && gpstime.isValid()) {

    tmElements_t tm;

    String rawtime = gpstime.value();
    uint32_t time_bcd = rawtime.toFloat() * 100;
    uint32_t delay_ms =
        gpstime.age() + nmea_txDelay_ms + NMEA_COMPENSATION_FACTOR;
    uint8_t year =
        CalendarYrToTm(gps.date.year()); // year offset from 1970 in microTime.h

    ESP_LOGD(TAG, "time [bcd]: %u", time_bcd);

    tm.Second = (time_bcd / 100) % 100;   // second
    tm.Minute = (time_bcd / 10000) % 100; // minute
    tm.Hour = time_bcd / 1000000;         // hour
    tm.Day = gps.date.day();              // day
    tm.Month = gps.date.month();          // month
    tm.Year = year;                       // year

    // add protocol delay to time with millisecond precision
    time_sec = makeTime(tm) + delay_ms / 1000;
    *msec = (delay_ms % 1000) ? delay_ms % 1000 : 0;
  }

  return timeIsValid(time_sec);

} // get_gpstime()

time_t get_gpstime(void) {
  uint16_t msec;
  return get_gpstime(&msec);
}

// GPS serial feed FreeRTos Task
void gps_loop(void *pvParameters) {

  configASSERT(((uint32_t)pvParameters) == 1); // FreeRTOS check

  while (1) {

    if (cfg.payloadmask & GPS_DATA) {
#ifdef GPS_SERIAL
      // feed GPS decoder with serial NMEA data from GPS device
      while (GPS_Serial.available()) {
        gps.encode(GPS_Serial.read());
      }
#elif defined GPS_I2C
      Wire.requestFrom(GPS_ADDR, 32); // caution: this is a blocking call
      while (Wire.available()) {
        gps.encode(Wire.read());
        delay(2); // 2ms delay according L76 datasheet
      }
#endif
    } // if

    // show NMEA data in verbose mode, useful for debugging GPS, bu tvery noisy
    // ESP_LOGV(TAG, "GPS NMEA data: passed %u / failed: %u / with fix: %u",
    //         gps.passedChecksum(), gps.failedChecksum(),
    //         gps.sentencesWithFix());

    delay(2); // yield to CPU

  } // end of infinite loop

} // gps_loop()

#endif // HAS_GPS