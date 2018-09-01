#include "utils.h"

void PrintHex8(uint8_t *data, uint8_t length) { // prints 8-bit data in hex with leading zeroes
    Serial.print("0x");
      
    for (int i = 0; i < length; i++) {
        if (data[i] < 0x10) {
            Serial.print("0");
        }
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.print("\n");
}
