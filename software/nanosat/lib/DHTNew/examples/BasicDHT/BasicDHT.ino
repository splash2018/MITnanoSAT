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
