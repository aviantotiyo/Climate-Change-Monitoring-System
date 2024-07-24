#ifndef MQTT_H
#define MQTT_H

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

extern PubSubClient client;
extern LiquidCrystal_I2C lcd;

void setupMQTT();
void reconnectMQTT();
void publishMQTT(const char* topic, const char* message);

#endif
