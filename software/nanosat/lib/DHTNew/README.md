DHTNew
======

The new library for your DHT temperature and humidity sensor.

DHTNew features:

* Consistent API
* Fast and clean implementation
* Reliable (welcome to no NAN's land !)
* Arduino and ESP8266 friendly
* Decoupled sensor reading
* Precise error reporting

Usage
-----

```cpp
#include <DHTNew.h>

DHT dht(2, DHT_MODEL_DHT22);  // set your pin and model here

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // wait for the minimum sampling period
  delay(dht.getMinimumSamplingPeriod());

  // print values (this triggers a sensor reading)
  Serial.print("Temperature: ");
  Serial.println("dht.readTemperature()");
  Serial.print("Humidity: ");
  Serial.println(dht.readHumidity());
  }
}
```

Complete Usage
--------------

```cpp
#include <DHTNew.h>

DHT dht(2, DHT_MODEL_DHT22);  // set your pin and model here
unsigned long dhtReadings = 0;
unsigned long dhtErrors = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // make a reading attempt (i.e. returns true every getMinimumSamplingPeriod() ms minimum)
  if (dht.read()) {
    dhtReadings++;
    Serial.println("Reading done");

    // report errors
    if (dht.getError() != DHT_ERROR_NONE) {
      dhtErrors++;
      Serial.print("Error: ");
      Serial.println(dht.getErrorString());
      return;
    }

    // print values
    Serial.print("Temperature: ");
    Serial.println(dht.getTemperature());
    Serial.print("Humidity: ");
    Serial.println(dht.getHumidity());

    // print error rate
    Serial.print("Error rate: ")
    Serial.print(dhtErrors);
    Serial.print("/");
    Serial.println(dhtReadings);
  }
}
```

Credits
-------
Although this library has been rewritten from the ground-up, I got inspiration from many sources:

* [markruys's arduino-DHT](https://github.com/markruys/arduino-DHT)
* [adafruit's DHT-sensor-library](https://github.com/adafruit/DHT-sensor-library)
* [matthijskooijman's DHT-sensor-library](https://github.com/matthijskooijman/DHT-sensor-library)

Many thanks to them !

And of course datasheets...

* [Sparkfun's DHT22](https://www.sparkfun.com/datasheets/Sensors/Temperature/DHT22.pdf)
* [Adafruit's DHT22](https://cdn-shop.adafruit.com/datasheets/Digital+humidity+and+temperature+sensor+AM2302.pdf)
* [Aosong's DHT22](http://akizukidenshi.com/download/ds/aosong/AM2302.pdf): very clean one!
* [D-Robotics UK's DHT11](http://www.micropik.com/PDF/dht11.pdf)
