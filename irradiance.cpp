#include "irradiance.h"
#include <Arduino.h>

// Global variables to hold irradiance data
extern float irradiance;
extern float power_watt;

void setupIrradiance() {
  // No specific setup needed for irradiance calculation
}

float calculateIrradiance() {
  // Read solar panel voltage from analog pin
  float voltage_solar_panel = analogRead(A0) * 5.0 / 1023.0; // Read ADC value and convert to voltage

  // Calculate power of solar panel
   power_watt = voltage_solar_panel * voltage_solar_panel / (R2 / (R1 + R2));

  // Convert power to irradiance (Watt/m^2)
   irradiance = power_watt / luasPanel;

  ///Show Data serial monitor
  // Serial.print("Solar Panel Voltage: ");
  // Serial.print(voltage_solar_panel);
  // Serial.println(" V");

  // Serial.print("Power (Calculated): ");
  // Serial.print(power_watt);
  // Serial.println(" W");

  // Serial.print("Irradiance:        ");
  // Serial.print(irradiance);
  // Serial.println(" W/m^2");
  ///EndShow Data serial monitor

  return irradiance;
}
