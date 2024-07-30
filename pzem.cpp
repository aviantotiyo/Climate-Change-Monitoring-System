#include "pzem.h"

Packet packets[TOTAL_NO_OF_PACKETS];
unsigned int regs[TOTAL_NO_OF_REGISTERS];

// Extern global variables to hold PZEM data
extern float voltage;
extern float current;
extern float power;
extern unsigned long energy;

void setupPZEM() {
  Serial1.begin(9600);       // Start Serial1 for communication with PZEM-017
  modbus_construct(&packets[PACKET1], 1, READ_INPUT_REGISTERS, 0, TOTAL_NO_OF_REGISTERS, 0);
  modbus_configure(&Serial1, baud, SERIAL_8N2, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS, regs);
}

void readPZEMData() {
  modbus_update();

   voltage = regs[0] / 100.0; // Assuming voltage is stored in regs[0] and needs conversion
  current = regs[1] / 1000.0; // Assuming current is stored in regs[1] and needs conversion
  power = (regs[2] | (regs[3] << 16)) / 10.0; // Assuming power needs conversion
  energy = (regs[4] | (regs[5] << 16)); // Assuming energy is in Wh

  // Print PZEM data to Serial Monitor for debugging
  // Serial.print("Voltage: ");
  // Serial.println(voltage);

  // Serial.print("Current: ");
  // Serial.println(current);

  // Serial.print("Power: ");
  // Serial.println(power);

  // Serial.print("Energy: ");
  // Serial.println(energy);

  // High voltage alarm status
  // Serial.print("High Voltage Alarm Status: ");
  // Serial.println(regs[6]);

  // // Low voltage alarm status
  // Serial.print("Low Voltage Alarm Status: ");
  // Serial.println(regs[7]);
}
