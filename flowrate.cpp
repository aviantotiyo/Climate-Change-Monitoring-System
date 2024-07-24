#include "flowrate.h"
#include <Arduino.h>

volatile int flowPulseCount = 0;
float flowRate = 0.0;
float totalVolume = 0.0;

void setupFlowRateSensor() {
    pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, FALLING);
}

void flowPulseCounter() {
    flowPulseCount++;
}

void calculateFlowRate() {
    flowRate = (flowPulseCount / 7.5);
    totalVolume += (flowRate / 60.0);
    flowPulseCount = 0;
}
