
//#######################################
//#                                     #
//#     DALE PY GROUP                   #
//#     Mr Panetone                     #
//#      marcelu.phd@gmail.com          #
//#                                     #
//#     01/08/2019                      #
//#######################################
// routines for writing data to an SD-card, if present

#if (HAS_SDCARD)

// Local logging tag
static const char TAG[] = __FILE__;

#include "sdcard.h"

static bool useSDCard;

static void createFile(void);

File fileSDCard;

bool sdcard_init() {
  ESP_LOGD(TAG, "looking for SD-card...");
  useSDCard = SD.begin(SDCARD_CS, SDCARD_MOSI, SDCARD_MISO, SDCARD_SCLK);
  if (useSDCard)
    createFile();
  else
    ESP_LOGD(TAG, "SD-card not found");
  return useSDCard;
}

void sdcardWriteData(uint16_t noWifi, uint16_t noBle) {
  static int counterWrites = 0;
  char tempBuffer[12 + 1];
  time_t t = now();
#if (HAS_SDS011)
  sdsStatus_t sds;
#endif

  if (!useSDCard)
    return;

  ESP_LOGD(TAG, "writing to SD-card");
  sprintf(tempBuffer, "%02d.%02d.%4d,", day(t), month(t), year(t));
  fileSDCard.print(tempBuffer);
  sprintf(tempBuffer, "%02d:%02d:%02d,", hour(t), minute(t), second(t));
  fileSDCard.print(tempBuffer);
  sprintf(tempBuffer, "%d,%d", noWifi, noBle);
  fileSDCard.print(tempBuffer);
#if (HAS_SDS011)
  sds011_store(&sds);
  sprintf(tempBuffer, ",%5.1f,%4.1f", sds.pm10, sds.pm25);
  fileSDCard.print(tempBuffer);
#endif
  fileSDCard.println();

  if (++counterWrites > 2) {
    // force writing to SD-card
    ESP_LOGD(TAG, "flushing data to card");
    fileSDCard.flush();
    counterWrites = 0;
  }
}

void createFile(void) {
  char bufferFilename[8 + 1 + 3 + 1];

  useSDCard = false;

  for (int i = 0; i < 100; i++) {
    sprintf(bufferFilename, SDCARD_FILE_NAME, i);
    ESP_LOGD(TAG, "SD: looking for file <%s>", bufferFilename);
    bool fileExists = SD.exists(bufferFilename);
    if (!fileExists) {
      ESP_LOGD(TAG, "SD: file does not exist: opening");
      fileSDCard = SD.open(bufferFilename, FILE_WRITE);
      if (fileSDCard) {
        ESP_LOGD(TAG, "SD: name opened: <%s>", bufferFilename);
        fileSDCard.print(SDCARD_FILE_HEADER);
#if (HAS_SDS011)
        fileSDCard.print(SDCARD_FILE_HEADER_SDS011);
#endif
        fileSDCard.println();
        useSDCard = true;
        break;
      }
    }
  }
  return;
}

#endif // (HAS_SDCARD)
