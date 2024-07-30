#ifndef PZEM_H
#define PZEM_H

#include <SimpleModbusMaster.h>

#define baud 9600
#define timeout 1000
#define polling 400 // the scan rate
#define retry_count 50

#define TxEnablePin 2
#define TOTAL_NO_OF_REGISTERS 8

enum
{
  PACKET1,
  TOTAL_NO_OF_PACKETS // leave this last entry
};

extern Packet packets[TOTAL_NO_OF_PACKETS];
extern unsigned int regs[TOTAL_NO_OF_REGISTERS];

void setupPZEM();
void readPZEMData();

#endif
