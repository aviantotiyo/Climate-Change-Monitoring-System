#include "flowrate.h"
#include <Arduino.h>

volatile int flowPulseCount = 0;
float flowRate = 0.0;
float totalVolume = 0.0;
float flowminutes = 0.0;

void setupFlowRateSensor() {
    pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowPulseCounter, FALLING);
}

void flowPulseCounter() {
    flowPulseCount++;
}

void calculateFlowRate() {
    // Menghitung flow rate dalam L/min
    flowRate = (flowPulseCount / 4.5);  // Sesuaikan jika diperlukan
    
    // Akumulasi total volume berdasarkan flow rate
    totalVolume += (flowRate / 60.0);  // Volume dalam liter
    
    // Reset pulse count
    flowPulseCount = 0;
}

void updateFlowMinutes() {
    flowminutes = flowRate;
    // Reset flow rate to start counting for the next minute
    flowRate = 0;
}
