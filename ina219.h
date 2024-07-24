#ifndef INA219_H
#define INA219_H

#include <Wire.h>
#include <Adafruit_INA219.h>

class INA219Sensor {
public:
    INA219Sensor(float r1, float r2, float panelArea);
    bool begin();
    void readValues();
    float getBusVoltage() const;
    float getCurrent_mA() const;
    float getPower_mW() const;
    float getVoltageSolarPanel() const;
    float getPowerWatt() const;
    float getIrradiance() const;

private:
    Adafruit_INA219 ina219;
    const float R1;
    const float R2;
    const float luasPanel;
    float busVoltage;
    float current_mA;
    float power_mW;
    float voltage_solar_panel;
    float power_watt;
    float irradiance;
};

#endif
