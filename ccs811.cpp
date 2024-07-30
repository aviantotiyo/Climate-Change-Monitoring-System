#include "ccs811.h"

Adafruit_CCS811 ccs811;

// Global variables to hold CCS811 data
extern float co2;
extern float tvoc;

void setupCCS811() {
  if (!ccs811.begin()) {
    Serial.println("Failed to start CCS811 sensor! Please check your wiring.");
    while (1); // Halt execution if sensor is not found
  }

  // Wait for the sensor to be ready
  while (!ccs811.available());
}

void readCCS811Data() {
  if (ccs811.available()) {
    if (!ccs811.readData()) {
       co2 = ccs811.geteCO2();
       tvoc = ccs811.getTVOC();

      // Print CCS811 data
      // Serial.print("CO2: ");
      // Serial.print(co2);
      // Serial.print(" ppm, TVOC: ");
      // Serial.print(tvoc);
      // Serial.println(" ppb");
      // EndPrint CCS811 data
    } else {
      Serial.println("Error reading data from CCS811 sensor!");
    }
  } else {
    Serial.println("CCS811 sensor not available");
  }
}
