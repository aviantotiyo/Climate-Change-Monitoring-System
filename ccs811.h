#ifndef CCS811_H
#define CCS811_H

#include <Wire.h>
#include <Adafruit_CCS811.h>

extern Adafruit_CCS811 ccs;

void setupCCS811();
bool readCCS811(float& eco2, float& tvoc);

#endif
