#include "ina219.h"

INA219Sensor::INA219Sensor(float r1, float r2, float panelArea)
    : R1(r1), R2(r2), luasPanel(panelArea), busVoltage(0), current_mA(0), power_mW(0), voltage_solar_panel(0), power_watt(0), irradiance(0) {}

bool INA219Sensor::begin() {
    return ina219.begin();
}

void INA219Sensor::readValues() {
    busVoltage = ina219.getBusVoltage_V();
    current_mA = ina219.getCurrent_mA();
    power_mW = busVoltage * current_mA;
    voltage_solar_panel = analogRead(A0) * 4.5 / 1023.0; // Baca nilai ADC dan konversi ke tegangan
    power_watt = voltage_solar_panel * voltage_solar_panel / (R2 / (R1 + R2));
    irradiance = power_watt / luasPanel;
}

float INA219Sensor::getBusVoltage() const {
    return busVoltage;
}

float INA219Sensor::getCurrent_mA() const {
    return current_mA;
}

float INA219Sensor::getPower_mW() const {
    return power_mW;
}

float INA219Sensor::getVoltageSolarPanel() const {
    return voltage_solar_panel;
}

float INA219Sensor::getPowerWatt() const {
    return power_watt;
}

float INA219Sensor::getIrradiance() const {
    return irradiance;
}
