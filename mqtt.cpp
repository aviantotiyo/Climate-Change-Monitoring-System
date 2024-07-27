#include "mqtt.h"

// Pengaturan IP Broker MQTT
const char* mqtt_server = "192.168.1.14";
const int mqtt_port = 1883;
const char* mqtt_user = "admin";
const char* mqtt_password = "admin";

EthernetClient ethClient;
PubSubClient client(ethClient);

void setupMQTT() {
    client.setServer(mqtt_server, mqtt_port);
}

void reconnectMQTT() {
    while (!client.connected()) {
        Serial.print("Menghubungkan ke MQTT...");
        
        if (client.connect("ArduinoClient", mqtt_user, mqtt_password)) {
            Serial.println("terhubung");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Terhubung MQTT");
        } else {
            Serial.print("gagal, rc=");
            Serial.print(client.state());
            Serial.println(" coba lagi dalam 5 detik");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Gagal koneksi");
            lcd.setCursor(0, 1);
            lcd.print("Coba lagi...");
            delay(4000);
        }
    }
}

void publishMQTT(const char* topic, const char* message) {
    client.publish(topic, message);
}
