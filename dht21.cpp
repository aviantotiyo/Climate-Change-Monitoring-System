#include "dht21.h"

DHT dht(DHTPIN, DHTTYPE);

void setupDHT21() {
    dht.begin();
}

float readHumidity() {
    return dht.readHumidity();
}

float readTemperature() {
    return dht.readTemperature();
}
