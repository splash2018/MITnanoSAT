//#######################################
//#                                     #
//#     DALE PY GROUP                   #
//#     Mr Panetone                     #
//#      marcelu.phd@gmail.com          #
//#                                     #
//#     01/08/2019                      #
//#######################################
/*


Copyright  2018 Oliver Brandmueller <ob@sysadm.in>
Copyright  2018 Klaus Wilting <verkehrsrot@arcor.de>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

NOTE:
Parts of the source files in this repository are made available under different
licenses. Refer to LICENSE.txt file in repository for more details.

//////////////////////// ESP32-Paxcounter \\\\\\\\\\\\\\\\\\\\\\\\\\

// Tasks and timers:

Task          Core  Prio  Purpose
-------------------------------------------------------------------------------
ledloop       0     3     blinks LEDs
spiloop       0     2     reads/writes data on spi interface
IDLE          0     0     ESP32 arduino scheduler -> runs wifi sniffer

lmictask      1     2     MCCI LMiC LORAWAN stack
clockloop     1     4     generates realtime telegrams for external clock
timesync_proc 1     3     processes realtime time sync requests
irqhandler    1     1     cyclic tasks (i.e. displayrefresh) triggered by timers
gpsloop       1     1     reads data from GPS via serial or i2c
lorasendtask  1     1     feeds data from lora sendqueue to lmcic
IDLE          1     0     ESP32 arduino scheduler -> runs wifi channel rotator

Low priority numbers denote low priority tasks.

NOTE: Changing any timings will have impact on time accuracy of whole code.
So don't do it if you do not own a digital oscilloscope.

// ESP32 hardware timers
-------------------------------------------------------------------------------
0	displayIRQ -> display refresh -> 40ms (DISPLAYREFRESH_MS)
1 ppsIRQ -> pps clock irq -> 1sec
3	MatrixDisplayIRQ -> matrix mux cycle -> 0,5ms (MATRIX_DISPLAY_SCAN_US)


// Interrupt routines
-------------------------------------------------------------------------------

fired by hardware
DisplayIRQ      -> esp32 timer 0  -> irqHandlerTask (Core 1)
CLOCKIRQ        -> esp32 timer 1  -> ClockTask (Core 1)
ButtonIRQ       -> external gpio  -> irqHandlerTask (Core 1)
PMUIRQ          -> PMU chip gpio  -> irqHandlerTask (Core 1)

fired by software (Ticker.h)
TIMESYNC_IRQ    -> timeSync()     -> irqHandlerTask (Core 1)
CYCLIC_IRQ      -> housekeeping() -> irqHandlerTask (Core 1)
SENDCYCLE_IRQ   -> sendcycle()    -> irqHandlerTask (Core 1)
BME_IRQ         -> bmecycle()     -> irqHandlerTask (Core 1)


// External RTC timer (if present)
-------------------------------------------------------------------------------
triggers pps 1 sec impulse

*/

// Basic Config
#include "main.h"



//--- VARS ---news
String rssi = "RSSI --";
String snrr = "SNR --";
String packSize = "--";
String packet;
String pulseStatus = "";
static const uint8_t LED_W = 25;

unsigned int counter = 0;
long msgCount = 0; // count of outgoing messages
int interval = 2000; // interval between sends
long lastSendTime = 0;
int ledState = LOW; // ledState used to set the LED
unsigned long previousMillis = 0; // will store last time LED was updated
uint64_t chipid;

//--------------------------
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

// the OLED used  U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8( clock= 15,  data= 4,  reset= 16);
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(15, 4, 16);

//create an BMP180 object using the I2C interface
BMP180I2C bmp180(I2C_ADDRESS);

// i2c
Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0();

// You can also use software SPI
//Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(13, 12, 11, 10, 9);
// Or hardware SPI! In this case, only CS pins are passed in
//Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0(10, 9);

const char *AP_SSID = "Andrea";
const char *AP_PWD = "450Ab6606";

// wifi
WiFiMulti wifiMulti;

// HTTP CLIENT
HTTPClient http;

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

String receivedText;
String receivedRssi;
uint8_t T[20] = {"Temperature"};

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
struct Displays
{
  String line0;
  String line1;
  String line2;
  String line3;
  String line4;
  String line5;
  String line6;
};
Displays displayLine;

struct Protocol
{
  String rssi;
  String snr;
  String packet;
  String courseGps;
  String numberOfSatellites;
  String course;
  String ssid;
  String password;
  String api;
  String MyDeviceId;
  String RadioMessageReceived;
  String BlueMessageReceived;
  String RadioMessageSended;
  String RadioMessageINI;
  String RadioMessageFIN;
  String RadioMessageLOG;
  String RadioMessageLOC;

  String deviceId;
  String latitude;
  String longitude;
  String lastlatitude;
  String lastlongitude;
  String datestr;
  String timestr;
  String cicles;
  String command;
  String datos;
  String inits;
  String idMotorista;
  String idGuia;
  String statuss;
  String lats;
  String ie;
  String message;
  String tipo;

  char EEPflag;
  unsigned int generalCounter;

  String lengths;
};
Protocol protocolo;

struct Sensors
{
  String sensorId;
  String devType;
  String devId;
  String devName;
  String devPrivateKey;
  String devPublicKey;
  String devSaveLog;
  String devStatus;
  String devUserId;
  String devDate;
  String devTime;
  String devLong;
  String devLati;
  String devPres;
  String devTem1;
  String devTem2;
  String devTem3;
  String devTem4;
  String devTem5;
  String devHumi;
  String devAceX;
  String devAceY;
  String devAceZ;
  String devMagX;
  String devMagY;
  String devMagZ;
  String devGyrX;
  String devGyrY;
  String devGyrZ;
  String devPuls;
  String devOxim;
  String devMapX;
  String devMapY;
  String devMapZ;
  String devSeco;
  String devSno2;
  String devSnh3;
  String devLigh;
  String devSoun;
  String devAna1;
  String devAna2;
  String devAna3;
  String devBut1;
  String devBut2;
  String devBut3;
  String devBut4;
  String devBut5;
  String devLed1;
  String devLed2;
  String devLed3;
  String devLed4;
  String devLed5;

  String lengths;
};
Sensors sensors;

struct Flags
{
  bool initializeRoutes;
  bool finalizeRoutes;
  bool initCommunication;
  bool validGps;
  bool validSerialRadioDataReceived;
  bool restart;
  bool receivedGps;
  bool receivedRadio;
  bool receivedBlue;
  bool sendRadio;
  bool initGateway;

  bool inBluetooth;
  bool timerIntEnabled;
  bool inTimerInt;
  bool inTask1Int;
  bool inTask2Int;
  bool inTaskRadioReceiver;
  bool inGpsRead;
  bool LORAerror;
  bool WIFIerror;
  bool SERIALerror;
  bool BMPerror;
  bool DHTerror;
  bool LSMerror;
};
Flags flags;
//--------------------------------------------------------------------------------------------

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void loopPAX(void);
void setupPAX(void);
void sendSerialDebug(String dadus);
void postDataToServer(void);
String serialJSON(void);
String deserialJSON(char responses[]);
String getChipId(void);
void printDISPLAYS(void);
void DHTS(void);
void initVARS(void);
void initDEVICES(void);
void initSERIALS(void);
void setupWIFIS(void);
void initLORAS(void);
void initDISPLAYS(void);
void initBMP180(void);
void initLSM9DS(void);
void initDHTS(void);
void setupSensor9DS(void);
void LSM9DS(void);
void BMP180(void);
void Scanner(void);
void loras(void);
String receiverLora(void);
void senderLora(String);
String createProtocolLora(String);
String recoverProtocolLora(String);

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// ---------------------------------------------
// ---------------------------------------------


// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//-------------------------------------------------------------
void sendSerialDebug(String dadus)
{
  if (HAS_SERIAL_DEBUG)
  {

    Serial.println(dadus); // debug
  }
}
//-------------------------------------------------------------

void postDataToServer(void)
{

  Serial.println("Posting JSON data to server...");
  // Block until we are able to connect to the WiFi access point
  if (wifiMulti.run() == WL_CONNECTED)
  {
    // ECHO API2
    // http.begin("https://httpbin.org/anything");
    // local
    http.begin(API3);
    // REMOTE
    // #define API3 "http://209.208.97.67:3333/api/v0/sensors"
    // http.begin(API3);

    http.addHeader("Content-Type", "application/json");
    /*
    StaticJsonDocument<200> doc;
    // Add values in the document
    //
    doc["sensor"] = "gps";
    doc["time"] = 1351824120;

    // Add an array.
    //
    JsonArray data = doc.createNestedArray("data");
    data.add(48.756080);
    data.add(2.302038);

    String requestBody;
    serializeJson(doc, requestBody);
*/
    String requestBody = serialJSON();
    int httpResponseCode = http.POST(requestBody);

    if (httpResponseCode > 0)
    {

      String response = http.getString();
      // deserialJSON(response);
      Serial.println(httpResponseCode);
      Serial.println(response);
    }
    else
    {

      Serial.printf("Error occurred while sending HTTP POST: %s\n", http.errorToString(httpResponseCode).c_str());
    }
  }
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

String serialJSON(void)
{
  // TODO: send SNR RSSI BATERY
  String jsonBody = "";

  StaticJsonDocument<MAX_JSON_BUFFER> doc;
  doc["sensorId"] = "";
  doc["devId"] = "";
  doc["devTem1"] = "";
  doc["devTem2"] = "";
  doc["devTem3"] = "";
  doc["devAceX"] = "";
  doc["devAceY"] = "";
  doc["devAceZ"] = "";
  doc["devMagX"] = "";
  doc["devMagY"] = "";
  doc["devMagZ"] = "";
  doc["devGyrX"] = "";
  doc["devGyrY"] = "";
  doc["devGyrZ"] = "";
  doc["devHumi"] = "";
  doc["devPres"] = "";

  doc["sensorId"] = String(sensors.sensorId);
  doc["devId"] = String(sensors.devId);
  doc["devTem1"] = String(sensors.devTem1);
  doc["devTem2"] = String(sensors.devTem2);
  doc["devTem3"] = String(sensors.devTem3);
  doc["devAceX"] = String(sensors.devAceX);
  doc["devAceY"] = String(sensors.devAceY);
  doc["devAceZ"] = String(sensors.devAceZ);
  doc["devMagX"] = String(sensors.devMagX);
  doc["devMagY"] = String(sensors.devMagY);
  doc["devMagZ"] = String(sensors.devMagZ);
  doc["devGyrX"] = String(sensors.devGyrX);
  doc["devGyrY"] = String(sensors.devGyrY);
  doc["devGyrZ"] = String(sensors.devGyrZ);
  doc["devHumi"] = String(sensors.devHumi);
  doc["devPres"] = String(sensors.devPres);

  serializeJson(doc, jsonBody); // serialize

  //sendSerialDebug(jsonBody);

  return jsonBody;
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

String deserialJSON(char responses[])
{
  String returno = "";
  DynamicJsonDocument docr(1000);

  DeserializationError error = deserializeJson(docr, responses);
  if (error)
  {
    sendSerialDebug("deserializationError " + String(error.c_str()));
  }
  else
  { // ok
    ///*
    const char *devId = docr["devId"];
    sensors.devId = String(devId);

    //*/
    sendSerialDebug("Device id= " + sensors.devId);
  }
  return returno;
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

String getChipId(void)
{
  uint64_t chipid = ESP.getEfuseMac(); // T he chip ID is essentially its MAC
                                       // address(length: 6 bytes).
  // erial.printf("ESP32 Chip ID = %04X", (uint16_t)(chipid >> 32)); // print
  // High 2 bytes Serial.printf("%08X\n", (uint32_t)chipid); // print Low
  // 4bytes. B4:E6:2D:86:33:BF  -->> e6b4862dbd33 24:0A:C4:8E:AE:86  -->>
  // a248ec484ae
  unsigned long long0 = (unsigned long)(((chipid >> 16) & 0xFFFF0000) >> 16);
  unsigned long long1 = (unsigned long)((chipid & 0xFFFF0000) >> 16);
  unsigned long long2 = (unsigned long)((chipid & 0x0000FFFF));
  String hex = ""; // DEVICENAME;
  hex += String(long2, HEX) + String(long1, HEX) +
         String(long0, HEX); // six octets
  return (hex);
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void printDISPLAYS(void)
{
  char line1[64];
  char line2[64];
  char line3[64];
  char line4[64];
  char line5[64];
  char line6[64];
  char line7[64];
  char line8[64];
  char line9[64];

  u8x8.drawString(0, 1, "CILU ");

  sensors.devId.toCharArray(line9, 64);
  u8x8.drawString(4, 1, line9);

  u8x8.drawString(0, 2, "TEMP:");

  sensors.devTem1.toCharArray(line1, 64);
  u8x8.drawString(6, 2, line1);

  u8x8.drawString(0, 3, "HUMI:");

  sensors.devHumi.toCharArray(line2, 64);
  u8x8.drawString(6, 3, line2);

  u8x8.drawString(0, 4, "PRES:");

  sensors.devPres.toCharArray(line3, 64);
  u8x8.drawString(6, 4, line3);

  u8x8.drawString(0, 5, "TEM2:");

  sensors.devTem2.toCharArray(line4, 64);
  u8x8.drawString(6, 5, line4);

  u8x8.drawString(0, 6, "AX:");
  u8x8.drawString(6, 6, "AY:");
  u8x8.drawString(11, 6, "AZ:");
  sensors.devAceX.toCharArray(line5, 64);
  sensors.devAceY.toCharArray(line6, 64);
  sensors.devAceZ.toCharArray(line7, 64);

  u8x8.drawString(0, 7, line5);
  u8x8.drawString(6, 7, line6);
  u8x8.drawString(11, 7, line7);

  delay(100);
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void DHTS(void)
{
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println(F("Error reading temperature!"));
  }
  else
  {
    sensors.devTem1 = String(event.temperature);
    Serial.print(F("TemperatureUP: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    Serial.println(F("Error reading humidity!"));
  }
  else
  {
    sensors.devHumi = String(event.relative_humidity);
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void initVARS(void)
{
  sensors.sensorId = "";
  sensors.devType = "";
  sensors.devId = "";
  sensors.devName = "";
  sensors.devPrivateKey = "";
  sensors.devPublicKey = "";
  sensors.devSaveLog = "";
  sensors.devStatus = "";
  sensors.devUserId = "";
  sensors.devDate = "";
  sensors.devTime = "";
  sensors.devLong = "";
  sensors.devLati = "";
  sensors.devPres = "";
  sensors.devTem1 = "";
  sensors.devTem2 = "";
  sensors.devTem3 = "";
  sensors.devTem4 = "";
  sensors.devTem5 = "";
  sensors.devHumi = "";
  sensors.devAceX = "";
  sensors.devAceY = "";
  sensors.devAceZ = "";
  sensors.devMagX = "";
  sensors.devMagY = "";
  sensors.devMagZ = "";
  sensors.devGyrX = "";
  sensors.devGyrY = "";
  sensors.devGyrZ = "";
  sensors.devPuls = "";
  sensors.devOxim = "";
  sensors.devMapX = "";
  sensors.devMapY = "";
  sensors.devMapZ = "";
  sensors.devSeco = "";
  sensors.devSno2 = "";
  sensors.devSnh3 = "";
  sensors.devLigh = "";
  sensors.devSoun = "";
  sensors.devAna1 = "";
  sensors.devAna2 = "";
  sensors.devAna3 = "";
  sensors.devBut1 = "";
  sensors.devBut2 = "";
  sensors.devBut3 = "";
  sensors.devBut4 = "";
  sensors.devBut5 = "";
  sensors.devLed1 = "";
  sensors.devLed2 = "";
  sensors.devLed3 = "";
  sensors.devLed4 = "";
  sensors.devLed5 = "";

  sensors.lengths = "";

  flags.initializeRoutes = false;
  flags.finalizeRoutes = false;
  flags.initCommunication = false;
  flags.validGps = false;
  flags.validSerialRadioDataReceived = false;
  flags.restart = false;
  flags.receivedGps = false;
  flags.receivedRadio = false;
  flags.receivedBlue = false;
  flags.sendRadio = false;
  flags.initGateway = false;

  flags.inBluetooth = false;
  flags.timerIntEnabled = false;
  flags.inTimerInt = false;
  flags.inTask1Int = false;
  flags.inTask2Int = false;
  flags.inTaskRadioReceiver = false;
  flags.inGpsRead = false;
  flags.LORAerror = false;
  flags.WIFIerror = false;
  flags.SERIALerror = false;
  flags.BMPerror = false;
  flags.DHTerror = false;
  flags.LSMerror = false;
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void initDEVICES(void)
{
  sensors.devId = getChipId();
  sensors.devType = DEVICE_TYPE;
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
void initSERIALS(void)
{

  Serial.begin(115200);
  while (!Serial)
    ; //if just the the basic function, must connect to a computer
  delay(1000);
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void initWIFIS(void)
{
  wifiMulti.addAP(AP_SSID, AP_PWD);
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void initLORAS(void)
{

  SPI.begin(5, 19, 27, 18);
  LoRa.setPins(SS, RST, DI0);

  Serial.println("LoRa Receiver");
  //u8x8.drawString(0, 1, "LoRa Receiver");

  if (!LoRa.begin(BAND))
  {
    Serial.println("Starting LoRa failed!");
    // u8x8.drawString(0, 1, "Starting LoRa failed!");
    // while (1);
    flags.LORAerror = true;
  }
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void initDISPLAYS(void)
{

  u8x8.begin();
  delay(500);

  u8x8.setFont(u8x8_font_chroma48medium8_r);
  delay(500);
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void initBMP180(void)
{
  //begin() initializes the interface, checks the sensor ID and reads the calibration parameters.
  if (!bmp180.begin())
  {
    Serial.println("begin() failed. check your BMP180 Interface and I2C Address.");
    // while (1);
    flags.BMPerror = true;
  }
  else
  {

    //reset sensor to default parameters.
    bmp180.resetToDefaults();

    //enable ultra high resolution mode for pressure measurements
    bmp180.setSamplingMode(BMP180MI::MODE_UHR);
  }
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void initLSM9DS(void)
{
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM9DS0. Check your wiring!");
    //while (1);
    flags.LSMerror = true;
  }
  else
  {
    Serial.println("Found LSM9DS0 9DOF");
  }
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void initDHTS(void)
{
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("°C"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("°C"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("%"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("%"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void setupSensor9DS(void)
{
  if (!flags.LSMerror)
  {

    // 1.) Set the accelerometer range
    lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
    //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_4G);
    //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_6G);
    //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_8G);
    //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_16G);

    // 2.) Set the magnetometer sensitivity
    lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
    //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_4GAUSS);
    //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_8GAUSS);
    //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_12GAUSS);

    // 3.) Setup the gyroscope
    lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
    //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_500DPS);
    //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_2000DPS);
  }
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void LSM9DS(void)
{
  if (!flags.LSMerror)
  {

    lsm.read();

    Serial.print("Accel X: ");
    sensors.devAceX = String((int)lsm.accelData.x);
    Serial.print((int)lsm.accelData.x);
    Serial.print(" ");
    Serial.print("Y: ");
    sensors.devAceY = String((int)lsm.accelData.y);
    Serial.print((int)lsm.accelData.y);
    Serial.print(" ");
    Serial.print("Z: ");
    sensors.devAceZ = String((int)lsm.accelData.z);
    Serial.println((int)lsm.accelData.z);
    Serial.print("");
    Serial.print("Mag X: ");
    sensors.devMagX = String((int)lsm.magData.x);
    Serial.print((int)lsm.magData.x);
    Serial.print(" ");
    Serial.print("Y: ");
    sensors.devMagY = String((int)lsm.magData.y);
    Serial.print((int)lsm.magData.y);
    Serial.print(" ");
    Serial.print("Z: ");
    sensors.devMagZ = String((int)lsm.magData.z);
    Serial.println((int)lsm.magData.z);
    Serial.print("");
    Serial.print("Gyro X: ");
    sensors.devGyrX = String((int)lsm.gyroData.x);
    Serial.print((int)lsm.gyroData.x);
    Serial.print(" ");
    Serial.print("Y: ");
    sensors.devGyrY = String((int)lsm.gyroData.y);
    Serial.print((int)lsm.gyroData.y);
    Serial.print(" ");
    Serial.print("Z: ");
    sensors.devGyrZ = String((int)lsm.gyroData.z);
    Serial.println((int)lsm.gyroData.z);
    Serial.println(" ");
    Serial.print("TempF: ");
    sensors.devTem3 = String((int)lsm.temperature);
    Serial.print((int)lsm.temperature);
    Serial.println("");
    delay(200);
  } // error in lsm
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void BMP180(void)
{
  // put your main code here, to run repeatedly:
  // delay(1000);

  if (!flags.BMPerror)
  {
    //start a temperature measurement
    if (!bmp180.measureTemperature())
    {
      Serial.println("could not start temperature measurement, is a measurement already running?");
      return;
    }

    //wait for the measurement to finish. proceed as soon as hasValue() returned true.
    do
    {
      delay(100);
    } while (!bmp180.hasValue());

    Serial.print("Temperature: ");
    sensors.devTem2 = String(bmp180.getTemperature());
    Serial.print(bmp180.getTemperature());
    Serial.println(" degC");

    //start a pressure measurement. pressure measurements depend on temperature measurement, you should only start a pressure
    //measurement immediately after a temperature measurement.
    if (!bmp180.measurePressure())
    {
      Serial.println("could not start perssure measurement, is a measurement already running?");
      return;
    }

    //wait for the measurement to finish. proceed as soon as hasValue() returned true.
    do
    {
      delay(100);
    } while (!bmp180.hasValue());

    Serial.print("Pressure: ");
    sensors.devPres = String(bmp180.getPressure());
    Serial.print(bmp180.getPressure());
    Serial.println(" Pa");
  } // end bmp error
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Scanner(void)
{
  Serial.println();
  Serial.println("I2C scanner. Scanning ...");
  byte count = 0;

  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission(i);       // Begin I2C transmission Address (i)
    if (Wire.endTransmission() == 0) // Receive 0 = success (ACK response)
    {
      Serial.print("Found address: ");
      Serial.print(i, DEC);
      Serial.print(" (0x");
      Serial.print(i, HEX); // PCF8574 7 bit address
      Serial.println(")");
      count++;
    }
  }
  Serial.print("Found ");
  Serial.print(count, DEC); // numbers of devices
  Serial.println(" device(s).");
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void loras(void)
{

  if (!flags.LORAerror)
  {
    // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
      // received a packet
      Serial.print("Received packet '");
      // u8x8.drawString(0, 4, "PacketID");

      // read packet
      while (LoRa.available())
      {
        receivedText = (char)LoRa.read();
        Serial.print(receivedText);
        char currentid[64];
        receivedText.toCharArray(currentid, 64);
        //   u8x8.drawString(9, 4, currentid);
      }

      // print RSSI of packet
      Serial.print("' with RSSI ");
      Serial.println(LoRa.packetRssi());
      // u8x8.drawString(0, 5, "PacketRS");
      receivedRssi = LoRa.packetRssi();
      char currentrs[64];
      receivedRssi.toCharArray(currentrs, 64);
      // u8x8.drawString(9, 5, currentrs);
    }
    delay(100);
  } // end lora error
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

String receiverLora(void)
{
  String returno = "";

  return returno;
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void senderLora(String)
{
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

String createProtocolLora(String)
{
  String returno = "";

  return returno;
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

String recoverProtocolLora(String)
{
  String returno = "";

  return returno;
}
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

/* end Aruino LOOP ------------------------------------------------------------ */
 void setupSenderLor(void)
 {
#if defined(LOLSEND)

     pinMode(HAS_LED, OUTPUT);

     Serial.begin(BAUDRATE);
     while (!Serial);

     Serial.println();
     Serial.println("LoRa Sender Test");

     SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_SS);
     LoRa.setPins(PIN_SPI_SS, RST, DIO0);
     if (!LoRa.begin(BAND))
     {
         Serial.println("Starting LoRa failed!");
         while (1);
     }
     //LoRa.onReceive(cbk);
     //  LoRa.receive();
     Serial.println("init ok");

     delay(500);

#endif
 }
 //----------------------------------------------------------// 
 void loraSenderDisplay(void)
{
    u8x8.setCursor(0, 0);
    u8x8.print("Sending packet:");
    u8x8.setCursor(0, 1);
    String counters = String(counter);
    u8x8.print(counters);
}
//----------------------------------------------------------//

void loopSenderLor(void)
{
#if defined(LOLSEND)

    // send packet
    LoRa.beginPacket();
    LoRa.print("JCunion ");
    LoRa.print(counter);
    LoRa.endPacket();

    counter++;
    digitalWrite(HAS_LED, HIGH); // turn the LED on (HIGH is the voltage level)
    delay(10);                 // wait for a second
    digitalWrite(HAS_LED, LOW);  // turn the LED off by making the voltage LOW
    delay(1000);                 // wait for a second

#endif    
 }
 //----------------------------------------------------------//
 void loraReceiverDisplay(void)
 {
#if defined(LOLREC)

     u8x8.setCursor(0, 0);
     u8x8.print(rssi);
     u8x8.setCursor(0, 1);
     u8x8.print("Bytes Rec.");
     u8x8.setCursor(0, 2);
     u8x8.print(packSize);
     u8x8.setCursor(0, 3);
     u8x8.print(packet);

 #endif    
 }
 //----------------------------------------------------------//
 void cbk(int packetSize)
 {
#if defined(LOLREC)

     packet = "";
     packSize = String(packetSize, DEC);
     for (int i = 0; i < packetSize; i++)
     {
         packet += (char)LoRa.read();
     }
     rssi = "RSSI " + String(LoRa.packetRssi(), DEC);
     //loraData();
#endif

#if defined(LOLDUPLEX)

     packet = "";
     packSize = String(packetSize, DEC);
     for (int i = 0; i < packetSize; i++)
     {
         packet += (char)LoRa.read();
     }
     rssi = "RSSI " + String(LoRa.packetRssi(), DEC);
     //loraData();
#endif
 }
 //----------------------------------------------------------//
 void setupReceiverlor(void)
 {
 #if defined(LOLREC)

     Serial.begin(BAUDRATE);
     while (!Serial);

     Serial.println();
     Serial.println("LoRa Receiver Callback");
     SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_SS);
     LoRa.setPins(PIN_SPI_SS, RST, DIO0);
     if (!LoRa.begin(BAND))
     {
         Serial.println("Starting LoRa failed!");
         while (1);
     }
     //LoRa.onReceive(cbk);
     LoRa.receive();
     Serial.println("init ok");
     
     delay(500);
 #endif    
 }
 //----------------------------------------------------------//
 void loopReceiverLor(void)
 {
 #if defined(LOLREC)

     int packetSize = LoRa.parsePacket();
     if (packetSize)
     {
         cbk(packetSize);
     }
     delay(10);
 #endif    
 }

 //--------------------------------------------------------------------

 //--------------------------------------------------------------------

 void blinks(void)
 {
     // check to see if it's time to blink the LED; that is, if the difference
     // between the current time and last time you blinked the LED is bigger than
     // the interval at which you want to blink the LED.
     unsigned long currentMillis = millis();

     if (currentMillis - previousMillis >= interval)
     {
         // save the last time you blinked the LED
         previousMillis = currentMillis;

         // if the LED is off turn it on and vice-versa:
         if (ledState == LOW)
         {
             ledState = HIGH;
         }
         else
         {
             ledState = LOW;
             pulseStatus = "";
         }

         // set the LED with the ledState of the variable:
         digitalWrite(LED_W, ledState);
     }
     /*
 digitalWrite(LED_W, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_W, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);    
  */
     // wait for a second
 }

 //--------------------------------------------------------------------

 void loraDuplexDisplay(void)
 {
 #if defined(LOLDUPLEX)
     u8x8.setCursor(4, 0);
     u8x8.print("                ");
     u8x8.print(rssi);
     u8x8.setCursor(0, 1);
     u8x8.print("Bytes Rec=");
     u8x8.setCursor(10, 1);
     u8x8.print(packSize);
     u8x8.setCursor(0, 2);
     u8x8.print("Message");
     u8x8.setCursor(0, 4);
     u8x8.print("snr");
     u8x8.setCursor(3, 4);
     u8x8.print(snrr);
     u8x8.setCursor(0, 6);
     u8x8.print("                ");
     u8x8.print(packet);
#endif
 }
 //------------------------------------------------------
 //------------------------------------------------------
 void sendMessage(String outgoing)
 {
  #if defined(LOLDUPLEX)

     LoRa.beginPacket();   // start packet
     LoRa.print(outgoing); // add payload
     LoRa.endPacket();     // finish packet and send it
     msgCount++;           // increment message ID
 #endif
 }
 //------------------------------------------------------
 void onReceive(int packetSize)
 {
 #if defined(LOLDUPLEX)

     if (packetSize == 0)
         return; // if there's no packet, return
     packet = "";

     // read packet header bytes:
     String incoming = "";

     while (LoRa.available())
     {
         incoming += (char)LoRa.read();
     }

     Serial.println("Message: " + incoming);
     Serial.println("RSSI: " + String(LoRa.packetRssi()));
     Serial.println("SNR: " + String(LoRa.packetSnr()));
     Serial.println();

     packet = incoming;
     packSize = String(packetSize, DEC);
     rssi = "RSSI " + String(LoRa.packetRssi(), DEC);
     snrr = String(LoRa.packetSnr(), DEC);

#endif
 }
 //------------------------------------------------------
 void setupDuplex(void)
 {
 #if defined(LOLDUPLEX)

     //serial
     Serial.begin(115200); // initialize serial
     while (!Serial);

     Serial.println("LoRa Duplex - Set spreading factor");
     SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_SS);
     LoRa.setPins(PIN_SPI_SS, RST, DIO0);

     if (!LoRa.begin(BAND))
     {
        // while (1);
         Serial.println("lora erro");
     }
     
     delay(1000);
     //LoRa.onReceive(cbk);
     // LoRa.receive();

     Serial.println("init ok");

    //VERY IMPORTANT FOR LONG RANGE
     LoRa.setSpreadingFactor(12); // ranges from 6-12,default 7 see API docs
     Serial.println("LoRa init succeeded.");

     getChipId();
 #endif
 }
 //------------------------------------------------------
 void loopDuplex(void)
 {
 #if defined(LOLDUPLEX)

     counter++;

     if (millis() - lastSendTime > interval)
     {
         String message = "JCcount "; // send a message
         message += String(msgCount);
         sendMessage(message);
         Serial.println("Sending " + message);
         lastSendTime = millis();        // timestamp the message
         interval = random(2000) + 1000; // 1-3 seconds
     }

         // parse for a packet, and call onReceive with the result:
         onReceive(LoRa.parsePacket());

 #endif
 }

 //----------------------------------------------------------//
 //----------------------------------------------------------//
// ---------------------------------------------
// ---------------------------------------------
void setupSensor()
{
  // setupPax();
  // Serial Port
  initVARS(); // clear all

  initSERIALS();      // serialport
                      // Display
  initDISPLAYS();     // oled
                      // INIT I2C
  Wire.begin(21, 22); // sda= GPIO_21 /scl= GPIO_22
                      // I2C
                      // 0X1D = DISPLAY
                      // 0X6B = LSM9DS FLORA ACCEL
                      // 0X77 = BMP180

#if (MODE_GATEWAY == 0) // NODE
  initLORAS();          // lora

  // DHT 11 init
  initBMP180(); // bmp180
  // lsm9ds init
  initLSM9DS(); // lsm9ds
  // DHT 11 init
  initDHTS(); // dht11
#endif

#if (MODE_GATEWAY == 0) // gatway

  delay(4000);
  initWIFIS(); // wifi
#endif

  initDEVICES(); // take device id
}
// ---------------------------------------------

void loopSensor()
{
  // loopPax();
  //Scanner ();
  // loras();
  String dadusRecived = "";
  String dadusSendServer = "";

#if (HAS_OLED) // display
  printDISPLAYS();
#endif // #if (HAS_OLED) // GATEWAY

  // ----------------------------------------------------

#if (MODE_GATEWAY == 0) // GATEWAY
  dadusRecived = receiverLora();
  if (dadusRecived != "")
  {
    // have dadus
    dadusSendServer = recoverProtocolLora(dadusRecived);
    postDataToServer();
  }
  postDataToServer();
#endif // #if (MODE_GATEWAY == 0)

  // ----------------------------------------------------

#if (MODE_GATEWAY == 0) // NOde
  BMP180();
  LSM9DS();
  DHTS();
#endif // #if (MODE_GATEWAY == 0)



}
// ---------------------------------------------
// ---------------------------------------------

void setup(){
setupDuplex();
}

void loop(){
loopDuplex();

}