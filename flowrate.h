#ifndef FLOWRATE_H
#define FLOWRATE_H

#define FLOW_SENSOR_PIN 3

extern volatile int flowPulseCount;
extern float flowRate;
extern float totalVolume;

void setupFlowRateSensor();
void flowPulseCounter();
void calculateFlowRate();

#endif
