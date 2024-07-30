#include "dht21.h"

DHT dht(DHTPIN, DHTTYPE);

// Global variables to hold dht21 data
extern float humidity;
extern float temperature;

void setupDHT21() {
  dht.begin();
}

void readDHT21Data() {
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  ///Show Data serial monitor
  // Serial.print("Temperature: ");
  // Serial.print(temperature);
  // Serial.println(" C");

  // Serial.print("Humidity: ");
  // Serial.print(humidity);
  // Serial.println(" %");
  ///Show Data serial monitor
}
