#include <Ethernet.h>
#include <NTPClient.h>
#include <EthernetUdp.h>
#include <LiquidCrystal_I2C.h>
#include <TimeLib.h>
#include <ArduinoJson.h>
#include "dht21.h"
#include "mqtt.h"
#include "ccs811.h"
#include "flowrate.h"

// Alamat I2C LCD
#define I2C_ADDR 0x3F

// Ukuran LCD (jumlah kolom dan baris)
#define LCD_COLS 16
#define LCD_ROWS 2

// Inisialisasi objek LCD
LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLS, LCD_ROWS);

// Pengaturan MAC Address untuk modul W5100
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Pengaturan NTP
EthernetUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", 25200, 60000);

// Variabel untuk melacak hari sebelumnya
int previousDay = -1;

void setup() {
    Serial.begin(9600);

    lcd.begin(LCD_COLS, LCD_ROWS);
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Starting...");

    SPI.begin();
    if (Ethernet.begin(mac) == 0) {
        Serial.println("Gagal mengkonfigurasi Ethernet menggunakan DHCP");
        while (true);
    }

    Serial.print("Alamat IP Arduino: ");
    Serial.println(Ethernet.localIP());
    lcd.setCursor(0, 1);
    lcd.print(Ethernet.localIP());

    timeClient.begin();
    timeClient.forceUpdate();

    setupDHT21();
    setupMQTT();
    setupFlowRateSensor();
    setupCCS811();
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

    if (minute(epochTime) % 5 == 0 && second(epochTime) == 0) {
        float h = readHumidity();
        float t = readTemperature();
        if (isnan(h) || isnan(t)) {
            Serial.println("Gagal membaca dari sensor DHT");
            return;
        }

        float eco2 = NAN;
        float tvoc = NAN;
        if (readCCS811(eco2, tvoc)) {
            Serial.println("Gagal membaca dari sensor CCS811");
        }

        String dateTimeString = getDateTimeString(epochTime);
        Serial.print("Kelembaban: ");
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Suhu: ");
        Serial.print(t);
        Serial.print(" *C\t");
        Serial.print("Flow Rate: ");
        Serial.print(flowRate);
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

        String payload = createJsonPayload(h, t, flowRate, totalVolume, eco2, tvoc, dateTimeString);
        publishMQTT("sensor/data", payload.c_str());

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

String createJsonPayload(float h, float t, float flowRate, float totalVolume, float eco2, float tvoc, const String& dateTimeString) {
    DynamicJsonDocument doc(1024);
    doc["humidity"] = h;
    doc["temperature"] = t;
    doc["flowRate"] = flowRate;
    doc["totalVolume"] = totalVolume;
    doc["eco2"] = eco2;
    doc["tvoc"] = tvoc;
    doc["timestamp"] = dateTimeString;
    String output;
    serializeJson(doc, output);
    return output;
}
