#ifndef DHT21_H
#define DHT21_H

#include <DHT.h>

// Pengaturan pin untuk sensor DHT21
#define DHTPIN 2
#define DHTTYPE DHT21
extern DHT dht;

void setupDHT21();
float readHumidity();
float readTemperature();

#endif
