//#######################################
//#                                     #
//#     DALE PY GROUP                   #
//#     Mr Panetone                     #
//#      marcelu.phd@gmail.com          #
//#                                     #
//#     01/08/2019                      #
//#######################################
// Basic Config
#if (HAS_LORA)
#include "lorawan.h"

// Local logging Tag
static const char TAG[] = "lora";

#if CLOCK_ERROR_PROCENTAGE > 7
#warning CLOCK_ERROR_PROCENTAGE value in lmic_config.h is too high; values > 7 will cause side effects
#endif

#if (TIME_SYNC_LORAWAN)
#ifndef LMIC_ENABLE_DeviceTimeReq
#define LMIC_ENABLE_DeviceTimeReq 1
#endif
#endif

// variable keep its values after restart or wakeup from sleep
RTC_NOINIT_ATTR u4_t RTCnetid, RTCdevaddr;
RTC_NOINIT_ATTR u1_t RTCnwkKey[16], RTCartKey[16];
RTC_NOINIT_ATTR int RTCseqnoUp, RTCseqnoDn;

QueueHandle_t LoraSendQueue;
TaskHandle_t lmicTask = NULL, lorasendTask = NULL;

class MyHalConfig_t : public Arduino_LMIC::HalConfiguration_t {

public:
  MyHalConfig_t(){};

  // set SPI pins to board configuration, pins may come from pins_arduino.h
  virtual void begin(void) override {
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  }

  // virtual void end(void) override

  // virtual ostime_t setModuleActive(bool state) override
};

static MyHalConfig_t myHalConfig{};

// LMIC pin mapping for Hope RFM95 / HPDtek HPD13A transceivers
static const lmic_pinmap myPinmap = {
    .nss = LORA_CS,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = LORA_RST == NOT_A_PIN ? LMIC_UNUSED_PIN : LORA_RST,
    .dio = {LORA_IRQ, LORA_IO1,
            LORA_IO2 == NOT_A_PIN ? LMIC_UNUSED_PIN : LORA_IO2},
    .rxtx_rx_active = LMIC_UNUSED_PIN,
    .rssi_cal = 10,
    .spi_freq = 8000000, // 8MHz
    .pConfig = &myHalConfig};

void lora_setupForNetwork(bool preJoin) {

  if (preJoin) {

#if CFG_LMIC_US_like
    // in the US, with TTN, it saves join time if we start on subband 1
    // (channels 8-15). This will get overridden after the join by
    // parameters from the network. If working with other networks or in
    // other regions, this will need to be changed.
    LMIC_selectSubBand(1);
#elif CFG_LMIC_EU_like
    // settings for TheThingsNetwork
    // Enable link check validation
    LMIC_setLinkCheckMode(true);
#endif

  } else {
    // set data rate adaptation according to saved setting
    LMIC_setAdrMode(cfg.adrmode);
    // set data rate and transmit power to stored device values if no ADR
    if (!cfg.adrmode)
      LMIC_setDrTxpow(assertDR(cfg.loradr), cfg.txpower);
    // show current devaddr
    ESP_LOGI(TAG, "DEVaddr: 0x%08X | Network ID: 0x%06X | Network Type: %d",
             LMIC.devaddr, LMIC.netid & 0x001FFFFF, LMIC.netid & 0x00E00000);
    ESP_LOGI(TAG, "RSSI: %d | SNR: %d", LMIC.rssi, (LMIC.snr + 2) / 4);
    ESP_LOGI(TAG, "Radio parameters: %s | %s | %s",
             getSfName(updr2rps(LMIC.datarate)),
             getBwName(updr2rps(LMIC.datarate)),
             getCrName(updr2rps(LMIC.datarate)));
    // store LMIC keys and counters in RTC memory
    LMIC_getSessionKeys(&RTCnetid, &RTCdevaddr, RTCnwkKey, RTCartKey);
  }
}

// DevEUI generator using devices's MAC address
void gen_lora_deveui(uint8_t *pdeveui) {
  uint8_t *p = pdeveui, dmac[6];
  int i = 0;
  esp_efuse_mac_get_default(dmac);
  // deveui is LSB, we reverse it so TTN DEVEUI display
  // will remain the same as MAC address
  // MAC is 6 bytes, devEUI 8, set first 2 ones
  // with an arbitrary value
  *p++ = 0xFF;
  *p++ = 0xFE;
  // Then next 6 bytes are mac address reversed
  for (i = 0; i < 6; i++) {
    *p++ = dmac[5 - i];
  }
}

/* new version, does it with well formed mac according IEEE spec, but is
breaking change
// DevEUI generator using devices's MAC address
void gen_lora_deveui(uint8_t *pdeveui) {
  uint8_t *p = pdeveui, dmac[6];
  ESP_ERROR_CHECK(esp_efuse_mac_get_default(dmac));
  // deveui is LSB, we reverse it so TTN DEVEUI display
  // will remain the same as MAC address
  // MAC is 6 bytes, devEUI 8, set middle 2 ones
  // to an arbitrary value
  *p++ = dmac[5];
  *p++ = dmac[4];
  *p++ = dmac[3];
  *p++ = 0xfe;
  *p++ = 0xff;
  *p++ = dmac[2];
  *p++ = dmac[1];
  *p++ = dmac[0];
}
*/

// Function to do a byte swap in a byte array
void RevBytes(unsigned char *b, size_t c) {
  u1_t i;
  for (i = 0; i < c / 2; i++) {
    unsigned char t = b[i];
    b[i] = b[c - 1 - i];
    b[c - 1 - i] = t;
  }
}

// LMIC callback functions
void os_getDevKey(u1_t *buf) {
#ifndef LORA_ABP
  memcpy(buf, APPKEY, 16);
#endif
}

void os_getArtEui(u1_t *buf) {
#ifndef LORA_ABP
  memcpy(buf, APPEUI, 8);
  RevBytes(buf, 8); // TTN requires it in LSB First order, so we swap bytes
#endif
}

void os_getDevEui(u1_t *buf) {
#ifndef LORA_ABP
  int i = 0, k = 0;
  memcpy(buf, DEVEUI, 8); // get fixed DEVEUI from loraconf.h
  for (i = 0; i < 8; i++) {
    k += buf[i];
  }
  if (k) {
    RevBytes(buf, 8); // use fixed DEVEUI and swap bytes to LSB format
  } else {
    gen_lora_deveui(buf); // generate DEVEUI from device's MAC
  }

// Get MCP 24AA02E64 hardware DEVEUI (override default settings if found)
#ifdef MCP_24AA02E64_I2C_ADDRESS
  get_hard_deveui(buf);
  RevBytes(buf, 8); // swap bytes to LSB format
#endif
#endif
}

void get_hard_deveui(uint8_t *pdeveui) {
  // read DEVEUI from Microchip 24AA02E64 2Kb serial eeprom if present
#ifdef MCP_24AA02E64_I2C_ADDRESS

  uint8_t i2c_ret;

  // Init this just in case, no more to 100KHz
  Wire.begin(SDA, SCL, 100000);
  Wire.beginTransmission(MCP_24AA02E64_I2C_ADDRESS);
  Wire.write(MCP_24AA02E64_MAC_ADDRESS);
  i2c_ret = Wire.endTransmission();

  // check if device was seen on i2c bus
  if (i2c_ret == 0) {
    char deveui[32] = "";
    uint8_t data;

    Wire.beginTransmission(MCP_24AA02E64_I2C_ADDRESS);
    Wire.write(MCP_24AA02E64_MAC_ADDRESS);
    Wire.endTransmission();

    Wire.requestFrom(MCP_24AA02E64_I2C_ADDRESS, 8);
    while (Wire.available()) {
      data = Wire.read();
      sprintf(deveui + strlen(deveui), "%02X ", data);
      *pdeveui++ = data;
    }
    ESP_LOGI(TAG, "Serial EEPROM found, read DEVEUI %s", deveui);
  } else
    ESP_LOGI(TAG, "Could not read DEVEUI from serial EEPROM");

  // Set back to 400KHz to speed up OLED
  Wire.setClock(400000);
#endif // MCP 24AA02E64
}

#if (VERBOSE)

// Display OTAA keys
void showLoraKeys(void) {
  // LMIC may not have used callback to fill
  // all EUI buffer so we do it here to a temp
  // buffer to be able to display them
  uint8_t buf[32];
  os_getDevEui((u1_t *)buf);
  printKey("DevEUI", buf, 8, true);
  os_getArtEui((u1_t *)buf);
  printKey("AppEUI", buf, 8, true);
  os_getDevKey((u1_t *)buf);
  printKey("AppKey", buf, 16, false);
}

#endif // VERBOSE

// LMIC send task
void lora_send(void *pvParameters) {
  configASSERT(((uint32_t)pvParameters) == 1); // FreeRTOS check

  MessageBuffer_t SendBuffer;

  while (1) {

    // postpone until we are joined if we are not
    while (!LMIC.devaddr) {
      vTaskDelay(pdMS_TO_TICKS(500));
    }

    // fetch next or wait for payload to send from queue
    if (xQueueReceive(LoraSendQueue, &SendBuffer, portMAX_DELAY) != pdTRUE) {
      ESP_LOGE(TAG, "Premature return from xQueueReceive() with no data!");
      continue;
    }

    // attempt to transmit payload
    else {

      switch (LMIC_setTxData2_strict(SendBuffer.MessagePort, SendBuffer.Message,
                                     SendBuffer.MessageSize,
                                     (cfg.countermode & 0x02))) {

      case LMIC_ERROR_SUCCESS:
        // save current Fcnt to RTC RAM
        RTCseqnoUp = LMIC.seqnoUp;
        RTCseqnoDn = LMIC.seqnoDn;

#if (TIME_SYNC_LORASERVER)
        // if last packet sent was a timesync request, store TX timestamp
        if (SendBuffer.MessagePort == TIMEPORT)
          // store LMIC time when we started transmit of timesync request
          timesync_store(osticks2ms(os_getTime()), timesync_tx);
#endif

        ESP_LOGI(TAG, "%d byte(s) sent to LORA", SendBuffer.MessageSize);
        break;
      case LMIC_ERROR_TX_BUSY:   // LMIC already has a tx message pending
      case LMIC_ERROR_TX_FAILED: // message was not sent
        // ESP_LOGD(TAG, "LMIC busy, message re-enqueued"); // very noisy
        vTaskDelay(pdMS_TO_TICKS(1000 + random(500))); // wait a while
        lora_enqueuedata(&SendBuffer); // re-enqueue the undelivered message
        break;
      case LMIC_ERROR_TX_TOO_LARGE:    // message size exceeds LMIC buffer size
      case LMIC_ERROR_TX_NOT_FEASIBLE: // message too large for current
                                       // datarate
        ESP_LOGI(TAG,
                 "Message too large to send, message not sent and deleted");
        // we need some kind of error handling here -> to be done
        break;
      default: // other LMIC return code
        ESP_LOGE(TAG, "LMIC error, message not sent and deleted");

      } // switch
    }
    delay(2); // yield to CPU
  }
}

void lora_stack_reset() {
  LMIC_reset(); // reset LMIC MAC
}

esp_err_t lora_stack_init(bool do_join) {
  assert(SEND_QUEUE_SIZE);
  LoraSendQueue = xQueueCreate(SEND_QUEUE_SIZE, sizeof(MessageBuffer_t));
  if (LoraSendQueue == 0) {
    ESP_LOGE(TAG, "Could not create LORA send queue. Aborting.");
    return ESP_FAIL;
  }
  ESP_LOGI(TAG, "LORA send queue created, size %d Bytes",
           SEND_QUEUE_SIZE * sizeof(MessageBuffer_t));

  // start lorawan stack
  ESP_LOGI(TAG, "Starting LMIC...");
  xTaskCreatePinnedToCore(lmictask,   // task function
                          "lmictask", // name of task
                          4096,       // stack size of task
                          (void *)1,  // parameter of the task
                          2,          // priority of the task
                          &lmicTask,  // task handle
                          1);         // CPU core

#ifdef LORA_ABP
  // Pass ABP parameters to LMIC_setSession
  lora_stack_reset();
  uint8_t appskey[sizeof(APPSKEY)];
  uint8_t nwkskey[sizeof(NWKSKEY)];
  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
  LMIC_setSession(NETID, DEVADDR, nwkskey, appskey);
  // These parameters are defined as macro in loraconf.h
  setABPParamaters();
#else
  // Start join procedure if not already joined,
  // lora_setupForNetwork(true) is called by eventhandler when joined
  // else continue current session
  if (do_join) {
    if (!LMIC_startJoining())
      ESP_LOGI(TAG, "Already joined");
  } else {
    lora_stack_reset();
    LMIC_setSession(RTCnetid, RTCdevaddr, RTCnwkKey, RTCartKey);
    LMIC.seqnoUp = RTCseqnoUp;
    LMIC.seqnoDn = RTCseqnoDn;
  }
#endif
  // start lmic send task
  xTaskCreatePinnedToCore(lora_send,      // task function
                          "lorasendtask", // name of task
                          3072,           // stack size of task
                          (void *)1,      // parameter of the task
                          1,              // priority of the task
                          &lorasendTask,  // task handle
                          1);             // CPU core

  return ESP_OK;
}

void lora_enqueuedata(MessageBuffer_t *message) {
  // enqueue message in LORA send queue
  BaseType_t ret = pdFALSE;
  MessageBuffer_t DummyBuffer;
  sendprio_t prio = message->MessagePrio;

  switch (prio) {
  case prio_high:
    // clear some space in queue if full, then fallthrough to prio_normal
    if (uxQueueSpacesAvailable(LoraSendQueue) == 0) {
      xQueueReceive(LoraSendQueue, &DummyBuffer, (TickType_t)0);
      ESP_LOGW(TAG, "LORA sendqueue purged, data is lost");
    }
  case prio_normal:
    ret = xQueueSendToFront(LoraSendQueue, (void *)message, (TickType_t)0);
    break;
  case prio_low:
  default:
    ret = xQueueSendToBack(LoraSendQueue, (void *)message, (TickType_t)0);
    break;
  }
  if (ret != pdTRUE) {
    snprintf(lmic_event_msg + 14, LMIC_EVENTMSG_LEN - 14, "<>");
    ESP_LOGW(TAG, "LORA sendqueue is full");
  } else {
    // add Lora send queue length to display
    snprintf(lmic_event_msg + 14, LMIC_EVENTMSG_LEN - 14, "%2u",
             uxQueueMessagesWaiting(LoraSendQueue));
  }
}

void lora_queuereset(void) { xQueueReset(LoraSendQueue); }

// LMIC lorawan stack task
void lmictask(void *pvParameters) {
  configASSERT(((uint32_t)pvParameters) == 1);

  // setup LMIC stack
  os_init_ex(&myPinmap); // initialize lmic run-time environment

  // register a callback for downlink messages and lmic events.
  // We aren't trying to write reentrant code, so pUserData is NULL.
  // LMIC_reset() doesn't affect callbacks, so we can do this first.
  LMIC_registerRxMessageCb(myRxCallback, NULL);
  LMIC_registerEventCb(myEventCallback, NULL);
  // to come with future LMIC version
  // LMIC_registerBattLevelCb(myBattLevelCb, NULL);

  // Reset the MAC state. Session and pending data transfers will be
  // discarded.
  lora_stack_reset();

// This tells LMIC to make the receive windows bigger, in case your clock is
// faster or slower. This causes the transceiver to be earlier switched on,
// so consuming more power. You may sharpen (reduce) CLOCK_ERROR_PERCENTAGE
// in src/lmic_config.h if you are limited on battery.
#ifdef CLOCK_ERROR_PROCENTAGE
  LMIC_setClockError(CLOCK_ERROR_PROCENTAGE * MAX_CLOCK_ERROR / 1000);
#endif

  while (1) {
    os_runloop_once(); // execute lmic scheduled jobs and events
    delay(2);          // yield to CPU
  }
} // lmictask

// lmic event handler
void myEventCallback(void *pUserData, ev_t ev) {

  // using message descriptors from LMIC library
  static const char *const evNames[] = {LMIC_EVENT_NAME_TABLE__INIT};
  // get current length of lora send queue
  uint8_t const msgWaiting = uxQueueMessagesWaiting(LoraSendQueue);

  // get current event message
  if (ev < sizeof(evNames) / sizeof(evNames[0]))
    snprintf(lmic_event_msg, LMIC_EVENTMSG_LEN, "%-16s",
             evNames[ev] + 3); // +3 to strip "EV_"
  else
    snprintf(lmic_event_msg, LMIC_EVENTMSG_LEN, "LMIC event %-4u ", ev);

  // process current event message
  switch (ev) {

  case EV_TXCOMPLETE:
    // -> processed in lora_send()
    break;

  case EV_RXCOMPLETE:
    // -> processed in myRxCallback()
    break;

  case EV_JOINING:
    // do the network-specific setup prior to join.
    lora_setupForNetwork(true);
    break;

  case EV_JOINED:
    // do the after join network-specific setup.
    lora_setupForNetwork(false);
    break;

  case EV_JOIN_FAILED:
    // must call LMIC_reset() to stop joining
    // otherwise join procedure continues.
    lora_stack_reset();
    break;

  case EV_JOIN_TXCOMPLETE:
    // replace descriptor from library with more descriptive term
    snprintf(lmic_event_msg, LMIC_EVENTMSG_LEN, "%-16s", "JOIN_WAIT");
    break;

  default:
    break;
  }

  // add Lora send queue length to display
  if (msgWaiting)
    snprintf(lmic_event_msg + 14, LMIC_EVENTMSG_LEN - 14, "%2u", msgWaiting);

  // print event
  ESP_LOGD(TAG, "%s", lmic_event_msg);
}

uint8_t myBattLevelCb(void *pUserData) {

  // set the battery value to send by LMIC in MAC Command
  // DevStatusAns. Available defines in lorabase.h:
  //   MCMD_DEVS_EXT_POWER   = 0x00, // external power supply
  //   MCMD_DEVS_BATT_MIN    = 0x01, // min battery value
  //   MCMD_DEVS_BATT_MAX    = 0xFE, // max battery value
  //   MCMD_DEVS_BATT_NOINFO = 0xFF, // unknown battery level
  // we calculate the applicable value from MCMD_DEVS_BATT_MIN to
  // MCMD_DEVS_BATT_MAX from bat_percent value

  uint8_t lmic_batt_level;
  uint8_t const batt_percent = read_battlevel();

  if (batt_percent == 0)
    lmic_batt_level = MCMD_DEVS_BATT_NOINFO;
  else

#ifdef HAS_PMU
      if (pmu.isVBUSPlug())
    lmic_batt_level = MCMD_DEVS_EXT_POWER;
#else
      if (batt_percent >= 100)
    lmic_batt_level = MCMD_DEVS_EXT_POWER;
#endif // HAS_PMU

  else
    lmic_batt_level =
        batt_percent / 100.0 * (MCMD_DEVS_BATT_MAX - MCMD_DEVS_BATT_MIN + 1);

  return lmic_batt_level;
}

// event EV_RXCOMPLETE message handler
void myRxCallback(void *pUserData, uint8_t port, const uint8_t *pMsg,
                  size_t nMsg) {

  // display amount of received data
  if (nMsg)
    ESP_LOGI(TAG, "Received %u byte(s) of payload on port %u", nMsg, port);
  else if (port)
    ESP_LOGI(TAG, "Received empty message on port %u", port);

  switch (port) {

// decode mac messages if we want to print those
#if (VERBOSE)
  case MACPORT:
    // decode downlink MAC commands
    if (LMIC.dataBeg)
      mac_decode(LMIC.frame, LMIC.dataBeg, true);
    // decode uplink MAC commands
    if (LMIC.pendMacLen)
      mac_decode(LMIC.pendMacData, LMIC.pendMacLen, false);
    break; // do not fallthrough to default, we are done
#endif

  // rcommand received -> call interpreter
  case RCMDPORT:
    rcommand(pMsg, nMsg);

// timeserver answer -> call timesync processor
#if (TIME_SYNC_LORASERVER)
  case TIMEPORT:
    // get and store gwtime from payload
    timesync_serverAnswer(const_cast<uint8_t *>(pMsg), nMsg);
#endif

    // decode any piggybacked downlink MAC commands if we want to print those
  default:
#if (VERBOSE)
    if (LMIC.dataBeg > 1)
      mac_decode(LMIC.frame, LMIC.dataBeg - 1, true);
#endif // VERBOSE

    break;
  } // switch
}

/*
// event EV_TXCOMPLETE message handler
void myTxCallback(void *pUserData, int fSuccess) {

  uint8_t *const pMsg = (uint8_t *)pUserData;

  // LMIC did successful transmit data
  if (fSuccess) {
    RTCseqnoUp = LMIC.seqnoUp;
    RTCseqnoDn = LMIC.seqnoDn;
  } else {
    // LMIC could not transmit data
    // -> error handling yet to come
  }
}
*/

const char *getSfName(rps_t rps) {
  const char *const t[] = {"FSK",  "SF7",  "SF8",  "SF9",
                           "SF10", "SF11", "SF12", "SF?"};
  return t[getSf(rps)];
}

const char *getBwName(rps_t rps) {
  const char *const t[] = {"BW125", "BW250", "BW500", "BW?"};
  return t[getBw(rps)];
}

const char *getCrName(rps_t rps) {
  const char *const t[] = {"CR 4/5", "CR 4/6", "CR 4/7", "CR 4/8"};
  return t[getCr(rps)];
}

#if (VERBOSE)
// decode LORAWAN MAC message
void mac_decode(const uint8_t cmd[], const uint8_t cmdlen, bool is_down) {

  if (!cmdlen)
    return;

  uint8_t foundcmd[cmdlen], cursor = 0;

  // select CID resolve table
  const mac_t *p;
  p = is_down ? MACdn_table : MACup_table;
  const int tablesize = is_down ? MACdn_tSize : MACup_tSize;
  const String MACdir = is_down ? "-->" : "<--";

  while (cursor < cmdlen) {

    // get number of commands in CID table
    int i = tablesize;

    // lookup cmd in CID table
    while (i--) {
      if (cmd[cursor] == (p + i)->cid) { // lookup command in CID table
        cursor++;                        // strip 1 byte CID
        if ((cursor + (p + i)->params) <= cmdlen) {
          memmove(foundcmd, cmd + cursor,
                  (p + i)->params); // strip opcode from cmd array
          cursor += (p + i)->params;
          ESP_LOGD(TAG, "%s %s", MACdir, (p + i)->cmdname);
        } else
          ESP_LOGD(TAG, "%s MAC command 0x%02X with missing parameter(s)",
                   MACdir, (p + i)->cid);
        break;   // command found -> exit table lookup loop
      }          // end of command validation
    }            // end of command table lookup loop
    if (i < 0) { // command not found -> skip it
      ESP_LOGD(TAG, "%s Unknown MAC command 0x%02X", MACdir, cmd[cursor]);
      cursor++;
    }
  } // command parsing loop

} // mac_decode()
#endif // VERBOSE

#endif // HAS_LORA