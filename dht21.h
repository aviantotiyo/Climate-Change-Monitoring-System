#ifndef DHT21_H
#define DHT21_H

#include <DHT.h>

#define DHTPIN 2      // Pin untuk DHT21
#define DHTTYPE DHT21 // Tipe sensor DHT

void setupDHT21();
void readDHT21Data();

#endif
