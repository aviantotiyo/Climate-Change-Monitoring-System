#include "ccs811.h"

Adafruit_CCS811 ccs;

void setupCCS811() {
    if (!ccs.begin()) {
        Serial.println("Gagal menginisialisasi sensor CCS811! Lanjutkan tanpa CCS811.");
    } else {
        while (!ccs.available());
    }
}

bool readCCS811(float& eco2, float& tvoc) {
    if (ccs.available()) {
        if (!ccs.readData()) {
            eco2 = ccs.geteCO2();
            tvoc = ccs.getTVOC();
            return true;
        }
    }
    return false;
}
