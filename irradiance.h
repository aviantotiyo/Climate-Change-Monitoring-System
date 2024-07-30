#ifndef IRRADIANCE_H
#define IRRADIANCE_H

const float R1 = 30.0;   // Nilai resistor R1 dalam kΩ
const float R2 = 7.5;    // Nilai resistor R2 dalam kΩ
const float luasPanel = 0.26 * 0.35;  // Meter persegi

void setupIrradiance();
float calculateIrradiance();

#endif
