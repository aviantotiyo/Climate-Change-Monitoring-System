#include <Ethernet.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <NTPClient.h>
#include <EthernetUdp.h>
#include <TimeLib.h>
#include "pzem.h"
#include "irradiance.h"
#include "ccs811.h"
#include "dht21.h"
#include <ArduinoJson.h>

// Update these with values suitable for your network.
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
const char* mqtt_server = "192.168.1.14";
const int mqtt_port = 1883;
const char* mqtt_user = "admin";
const char* mqtt_password = "admin";

EthernetClient ethClient;
PubSubClient client(ethClient);
EthernetUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600); // GMT+7

// Global variables to hold CCS811 data
float co2 = 0;
float tvoc = 0;
// Global variables to hold dht21 data
float humidity = 0;
float temperature = 0;
// Global variables to hold irradiance data
float irradiance;
float power_watt;
// Global variables to hold PZEM data
float voltage = 0;
float current = 0;
float power = 0;
unsigned long energy = 0;

// Time control 120000 (2menit) 300000 (5menit)
unsigned long previousEpochTime = 0;

void setupEthernet() {
  // Start the Ethernet connection using DHCP
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // No point in carrying on, so do nothing forevermore:
    for(;;);
  }
  delay(1500); // Allow the hardware to sort itself out
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient", mqtt_user, mqtt_password)) {
      // Once connected, update the time
      timeClient.update();
      unsigned long epochTime = timeClient.getEpochTime();
      setTime(epochTime);

      // Format the time in DATETIME format
      char dateTime[20];
      sprintf(dateTime, "%04d-%02d-%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());
      
      Serial.print("connected - ");
      Serial.println(dateTime);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();  // Use default pins for SDA (20) and SCL (21) on Arduino Mega

  setupPZEM();
  setupIrradiance();
  setupCCS811();
  setupDHT21();

  setupEthernet();
  client.setServer(mqtt_server, mqtt_port);
  timeClient.begin();

  // Connect to MQTT broker
  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Get current time
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();

  // Check if it's time to send data (120) : 2 menit, (300) = 5 menit)
  if (epochTime % 300 == 0 && epochTime != previousEpochTime) {
    previousEpochTime = epochTime;

    // Read sensor data
    readCCS811Data();
    readDHT21Data();
    calculateIrradiance();
    readPZEMData();

    // Format the time in DATETIME format
    setTime(epochTime);
    char dateTime[20];
    sprintf(dateTime, "%04d-%02d-%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());

    // Create a JSON document
    StaticJsonDocument<200> doc;
    doc["hum"] = humidity;
    doc["temp"] = temperature;
    doc["eco2"] = co2;
    doc["tvoc"] = tvoc;
    doc["updated_at"] = dateTime;
    doc["irradiance"] = irradiance;
    doc["power_irr"] = power_watt;
    doc["voltage"] = voltage;     
    doc["current"] = current;     
    doc["power"] = power;         
    doc["energy"] = energy;       
    
    // Serialize JSON to a string
    char jsonBuffer[256];
    size_t n = serializeJson(doc, jsonBuffer);

    // Debugging: Print JSON data to Serial Monitor
    Serial.print("JSON Data: ");
    Serial.println(jsonBuffer);

    // Publish JSON data to MQTT broker
    client.publish("sensor_data", jsonBuffer, n);
  }

  delay(1000); // Small delay to avoid flooding the loop
}