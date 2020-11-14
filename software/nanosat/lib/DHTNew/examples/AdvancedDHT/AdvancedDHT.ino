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
