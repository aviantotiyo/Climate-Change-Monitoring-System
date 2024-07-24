#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Ethernet.h>
#include <NTPClient.h>
#include <EthernetUdp.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include "dht21.h"
#include "mqtt.h"
#include "ccs811.h"
#include "flowrate.h"
#include "ina219.h"
#include <SimpleModbusMaster.h>

// Alamat I2C LCD
#define I2C_ADDR 0x3F
#define LCD_COLS 16
#define LCD_ROWS 2

// Inisialisasi objek LCD
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

// Pengaturan MAC Address untuk modul W5100
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Pengaturan NTP
EthernetUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", 25200, 60000);

// Luas panel surya dalam meter persegi
const float luasPanel = 0.26 * 0.35;  // Meter persegi

// Inisialisasi sensor INA219
INA219Sensor ina219Sensor(30.0, 7.5, luasPanel);  // Gunakan nilai resistor yang sama seperti sebelumnya

// Variabel untuk melacak hari sebelumnya
int previousDay = -1;

// Variabel untuk melacak waktu untuk pembaruan flow rate per menit
unsigned long lastFlowUpdate = 0;

// PZEM017 Modbus settings
#define baud 9600
#define timeout 1000
#define polling 400 // the scan rate
#define retry_count 50
#define TxEnablePin 2
#define TOTAL_NO_OF_REGISTERS 8

enum {
  PACKET1,
  TOTAL_NO_OF_PACKETS // leave this last entry
};

Packet packets[TOTAL_NO_OF_PACKETS];
unsigned int regs[TOTAL_NO_OF_REGISTERS];

void setup() {
    Serial.begin(9600);

    // Inisialisasi LCD
    lcd.begin(LCD_COLS, LCD_ROWS);
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Starting...");

    SPI.begin();
    if (Ethernet.begin(mac) == 0) {
        Serial.println("Gagal mengkonfigurasi Ethernet menggunakan DHCP");
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("Ethernet Error");
        while (true);
    }

    Serial.print("Alamat IP Arduino: ");
    Serial.println(Ethernet.localIP());
    lcd.setCursor(0, 1);
    lcd.print("IP: ");
    lcd.print(Ethernet.localIP());

    timeClient.begin();
    timeClient.forceUpdate();

    setupDHT21();
    setupMQTT();
    setupFlowRateSensor();
    setupCCS811();

    // Inisialisasi sensor INA219
    if (!ina219Sensor.begin()) {
        Serial.println("Failed to find INA219 chip");
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print("INA219 Error");
        while (1) { delay(10); }
    }

    // Configure Modbus for PZEM017
    Serial1.begin(baud); // Start Serial1 for PZEM017
    modbus_construct(&packets[PACKET1], 1, READ_INPUT_REGISTERS, 0, TOTAL_NO_OF_REGISTERS, 0);
    modbus_configure(&Serial1, baud, SERIAL_8N2, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS, regs);
}

void loop() {
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();
    timeClient.update();

    unsigned long epochTime = timeClient.getEpochTime();
    int currentDay = day(epochTime);

    if (currentDay != previousDay) {
        totalVolume = 0.0;
        previousDay = currentDay;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Reset Volume");
    }

    calculateFlowRate();

    // Perbarui flowminutes setiap 1 menit
    if (millis() - lastFlowUpdate >= 60000) {
        updateFlowMinutes();
        lastFlowUpdate = millis();
    }

    if (minute(epochTime) % 5 == 0 && second(epochTime) == 0) {
        float h = readHumidity();
        float t = readTemperature();
        if (isnan(h) || isnan(t)) {
            Serial.println("Gagal membaca dari sensor DHT");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("DHT Error");
            return;
        }

        float eco2 = NAN;
        float tvoc = NAN;
        if (readCCS811(eco2, tvoc)) {
            Serial.println("Gagal membaca dari sensor CCS811");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("CCS811 Error");
        }

        ina219Sensor.readValues();
        
        float busVoltage = ina219Sensor.getBusVoltage();
        float current_mA = ina219Sensor.getCurrent_mA();
        float power_mW = ina219Sensor.getPower_mW();
        float voltage_solar_panel = ina219Sensor.getVoltageSolarPanel();
        float power_watt = ina219Sensor.getPowerWatt();
        float irradiance = ina219Sensor.getIrradiance();

        String dateTimeString = getDateTimeString(epochTime);
        Serial.print("Kelembaban: ");
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Suhu: ");
        Serial.print(t);
        Serial.print(" *C\t");
        Serial.print("Flow Rate: ");
        Serial.print(flowminutes);
        Serial.print(" L/min\t");
        Serial.print("Total Volume: ");
        Serial.print(totalVolume, 2);
        Serial.print(" L\t");
        if (!isnan(eco2) && !isnan(tvoc)) {
            Serial.print("eCO2: ");
            Serial.print(eco2);
            Serial.print(" ppm\t");
            Serial.print("TVOC: ");
            Serial.print(tvoc);
            Serial.print(" ppb\t");
        }
        Serial.print("Waktu: ");
        Serial.println(dateTimeString);
        
        Serial.print("Irradiance:        "); Serial.print(irradiance); Serial.println(" W/m^2");
        Serial.print("Power (Calculated): "); Serial.print(power_watt); Serial.println(" W");
        Serial.print("busVoltage: "); Serial.print(busVoltage); Serial.println(" V");
        Serial.print("current_mA: "); Serial.print(current_mA); Serial.println(" mA");
        Serial.print("power_mW: "); Serial.print(power_mW); Serial.println(" mW");
        Serial.print("v_solar_panel: "); Serial.print(voltage_solar_panel); Serial.println(" V");

        // PZEM017 Reading
        modbus_update();
        Serial.print("Voltage: ");
        Serial.println(regs[0] / 100.0);
        Serial.print("Current: ");
        Serial.println(regs[1] / 1000.0);

        unsigned long power = (regs[2] | (regs[3] << 16));
        Serial.print("Power: ");
        Serial.println(power / 10.0);

        unsigned long energy = (regs[4] | (regs[5] << 16));
        Serial.print("Energy: ");
        Serial.println(energy);

        Serial.print("High Voltage Alarm Status: ");
        Serial.println(regs[6]);

        Serial.print("Low Voltage Alarm Status: ");
        Serial.println(regs[7]);

        String payload = createJsonPayload(h, t, flowminutes, totalVolume, eco2, tvoc, dateTimeString, irradiance, power_watt);
        publishMQTT("sensor_data", payload.c_str());

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("T: ");
        lcd.print(t);
        lcd.print(" H: ");
        lcd.print(h);
        lcd.setCursor(0, 1);
        lcd.print("V: ");
        lcd.print(totalVolume, 1);
        lcd.print(" L");

        delay(1000);
    }
}

String getDateTimeString(unsigned long epochTime) {
    char buffer[20];
    sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d", year(epochTime), month(epochTime), day(epochTime), hour(epochTime), minute(epochTime), second(epochTime));
    return String(buffer);
}

String createJsonPayload(float humidity, float temperature, float flowRate, float totalVolume, float eco2, float tvoc, String dateTime, float irradiance, float power_watt) {
    DynamicJsonDocument doc(1024);
    doc["hum"] = humidity;
    doc["temp"] = temperature;
    doc["flowRate"] = flowRate;
    doc["totalVolume"] = totalVolume;
    doc["eco2"] = eco2;
    doc["tvoc"] = tvoc;
    doc["updated_at"] = dateTime;
    doc["irradiance"] = irradiance;
    doc["power_irr"] = power_watt;
    doc["voltage"] = regs[0] / 100.0;  // Assuming regs[0] is in hundredths of a volt
    doc["current"] = regs[1] / 1000.0;  // Assuming regs[1] is in milliamps
    doc["power"] = (regs[2] | (regs[3] << 16)) / 10.0; // Assuming power is in tenths of a watt
    doc["energy"] = (regs[4] | (regs[5] << 16));  // Energy is typically in watt-hours

    String jsonString;
    serializeJson(doc, jsonString);
    return jsonString;
}