#ifndef DHTNEW_H
#define DHTNEW_H

#include "Arduino.h"

// DHTModel_t
typedef enum {
    DHT_MODEL_DHT11 = 11,
    DHT_MODEL_DHT22 = 22,
    DHT_MODEL_DHT21 = 21,
    DHT_MODEL_AM2302 = DHT_MODEL_DHT22, // Packaged DHT22
    DHT_MODEL_AM2301 = DHT_MODEL_DHT21, // Packaged DHT21
    DHT_MODEL_RHT03 = DHT_MODEL_DHT22  // Equivalent to DHT22
} DHTModel_t;

// DHTError_t
typedef enum {
    DHT_ERROR_NONE = 0,
    DHT_ERROR_TIMEOUT_START,
    DHT_ERROR_TIMEOUT_DATA,
    DHT_ERROR_CHECKSUM
} DHTError_t;

class DHT {
  public:
    // init
    DHT(uint8_t pin, DHTModel_t model);
    void begin(void);

    // core api
    float getTemperature(bool asFahrenheit=false);
    float readTemperature(bool asFahrenheit=false, bool force=false);
    float getHumidity();
    float readHumidity(bool force=false);
    bool read(bool force=false);
    DHTModel_t getModel() {
        return _model;
    }
    DHTError_t getError() {
        return _error;
    };
    const char* getErrorString();

    // utilities
    int getMinimumSamplingPeriod() {
        return _model == DHT_MODEL_DHT11 ? 1000 : 2000;
    }
    int8_t getNumberOfDecimalsTemperature() {
        return _model == DHT_MODEL_DHT11 ? 0 : 1;
    };
    int8_t getLowerBoundTemperature() {
        return _model == DHT_MODEL_DHT11 ? 0 : -40;
    };
    int8_t getUpperBoundTemperature() {
        return _model == DHT_MODEL_DHT11 ? 50 : 125;
    };
    int8_t getNumberOfDecimalsHumidity() {
        return 0;
    };
    int8_t getLowerBoundHumidity() {
        return _model == DHT_MODEL_DHT11 ? 20 : 0;
    };
    int8_t getUpperBoundHumidity() {
        return _model == DHT_MODEL_DHT11 ? 90 : 100;
    };
    float toFahrenheit(float);
    float toCelsius(float);
    float computeHeatIndex(float temperature, float humidity, bool isFahrenheit=false);

  protected:
    uint8_t _pin;
    float _temperature;
    float _humidity;

  private:
    DHTModel_t _model;
    DHTError_t _error;
    unsigned long _lastReadTime;
};

#endif
