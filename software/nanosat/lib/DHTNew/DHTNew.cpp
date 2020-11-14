#include "DHTNew.h"

DHT::DHT(uint8_t pin, DHTModel_t model) {
    _pin = pin;
    _model = model;
}

void DHT::begin(void) {
    pinMode(_pin, INPUT_PULLUP);
    _lastReadTime = millis() - getMinimumSamplingPeriod();  // to trigger a reading
}

// Get the temperature from the previous reading
float DHT::getTemperature(bool asFahrenheit) {
    if (asFahrenheit)
        return toFahrenheit(_temperature);

    return _temperature;
}

// Read and returns the temperature
float DHT::readTemperature(bool asFahrenheit, bool force) {
    read(force);
    return getTemperature(asFahrenheit);
}

// Get the humidity from the previous reading
float DHT::getHumidity() {
    return _humidity;
}

// Read and returns the humidity
float DHT::readHumidity(bool force) {
    read(force);
    return getHumidity();
}

const char* DHT::getErrorString() {
    switch ( _error ) {
    case DHT_ERROR_TIMEOUT_START:
        return "timeout_start";

    case DHT_ERROR_TIMEOUT_DATA:
        return "timeout_data";

    case DHT_ERROR_CHECKSUM:
        return "checksum";

    default:
        return "none";
    }
}

float DHT::toFahrenheit(float c) {
    return c * 1.8 + 32;
}

float DHT::toCelsius(float f) {
    return (f - 32) * 0.55555;
}

float DHT::computeHeatIndex(float temperature, float humidity, bool isFahrenheit) {
    // Using both Rothfusz and Steadman's equations
    // http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
    float hi;

    if (!isFahrenheit)
        temperature = toCelsius(temperature);

    hi = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) + (humidity * 0.094));

    if (hi > 79) {
        hi = -42.379 +
             2.04901523 * temperature +
             10.14333127 * humidity +
             -0.22475541 * temperature*humidity +
             -0.00683783 * pow(temperature, 2) +
             -0.05481717 * pow(humidity, 2) +
             0.00122874 * pow(temperature, 2) * humidity +
             0.00085282 * temperature*pow(humidity, 2) +
             -0.00000199 * pow(temperature, 2) * pow(humidity, 2);

        if((humidity < 13) && (temperature >= 80.0) && (temperature <= 112.0))
            hi -= ((13.0 - humidity) * 0.25) * sqrt((17.0 - abs(temperature - 95.0)) * 0.05882);

        else if((humidity > 85.0) && (temperature >= 80.0) && (temperature <= 87.0))
            hi += ((humidity - 85.0) * 0.1) * ((87.0 - temperature) * 0.2);
    }

    return isFahrenheit ? hi : toFahrenheit(hi);
}

// Returns true if a reading attempt was made (successfull or not)
bool DHT::read(bool force) {
    // don't read more than every getMinimumSamplingPeriod() milliseconds
    unsigned long currentTime = millis();
    if (!force && ((currentTime - _lastReadTime) < getMinimumSamplingPeriod())) {
        return false;
    }

    // reset lastReadTime, temperature and humidity
    _lastReadTime = currentTime;
    _temperature = NAN;
    _humidity = NAN;

    // send start signal
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    if (_model == DHT_MODEL_DHT11)
        delay(18); // [18-20]ms
    else
        delayMicroseconds(800); // [0.8-20]ms

    // init data
    uint8_t data[5] = {0};

    // begin of time critical code
    noInterrupts();

    // start reading the data line
    pinMode(_pin, INPUT_PULLUP);
    delayMicroseconds(20);  // [20-200]us

    // 80us low time + 80us high time + 10us error margin
    if (!pulseIn(_pin, HIGH, 170)) {
        _error = DHT_ERROR_TIMEOUT_START;
        interrupts();
        return true;
    }

    // read the 40 bits (5 bytes) of data
    for (uint8_t i = 0; i < sizeof(data) * 8; i++) {
        // 50us low time + [26-70]us high time + 10us error margin
        uint8_t pulse = pulseIn(_pin, HIGH, 150);

        if (!pulse) {
            _error = DHT_ERROR_TIMEOUT_DATA;
            interrupts();
            return true;
        }

        data[i/8] <<= 1;
        if (pulse > 50)  // this is a 1
            data[i/8] |= 1;
    }
    // end of time critical code
    interrupts();

    // verify checksum
    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        _error = DHT_ERROR_CHECKSUM;
        return true;
    }

    // we made it
    _error = DHT_ERROR_NONE;

    // convert data to actual temperature and humidity
    switch (_model) {
    case DHT_MODEL_DHT11:
        _temperature = data[2];
        _humidity = data[0];
        break;
    case DHT_MODEL_DHT22:
    case DHT_MODEL_DHT21:
        _temperature = data[2] & 0x7F;
        _temperature *= 256;
        _temperature += data[3];
        _temperature *= 0.1;
        if (data[2] & 0x80) {
            _temperature *= -1;
        }
        _humidity = data[0];
        _humidity *= 256;
        _humidity += data[1];
        _humidity *= 0.1;
        break;
    }

    return true;
}
