#include "lora.h"

#include <lmic.h>
#include <hal/hal.h>
#include "lora_credentials.h"
#include "utils.h"

// code based on http://edukit.thingsos.de/lora/lorasketch/

const int MAX_PAYLOAD_SIZE = MAX_LEN_FRAME - 13; // Max. LMIC packet length minus 13 byte LoRa Header

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 8,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {3, 19, LMIC_UNUSED_PIN},
};

void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

void lora_init() {
    uint8_t appskey[sizeof(APPSKEY)];
    uint8_t nwkskey[sizeof(NWKSKEY)];
    memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
    memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));

    LMIC_reset();
    LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
    LMIC_setLinkCheckMode(0);
    LMIC_setAdrMode(0);
    LMIC_setDrTxpow(DR_SF8, 14);
}

boolean lora_isSending() {
    return LMIC.opmode & OP_TXRXPEND;
}

void lora_prepareTypes(payload_type *types) {
    for (int i = 0; i < LORA_FIELD_COUNT; i++) {
        types[i] = PAYLOAD_TYPE_UNUSED;
    }
}

void lora_send(payload_type* types, lora_payload *fields) {

    byte payload[WRITE_KEY.length() + LORA_FIELD_COUNT + LORA_FIELD_COUNT * 4]; // maximum size of variable is 56, but we can only transmit 51 bytes. So we have to check before sending
    // 56 = 16 (WRITE_KEY) + 8 (Payload Designator) + 8*4 (maximum packets)
    int payloadSize = 0;

    // add WRITE_KEY to payload
    for (int i = 0; i < WRITE_KEY.length(); i++) {
        payload[i] = WRITE_KEY[i];
        payloadSize++;
    }

    // size of the designators
    for (int i = 0; i < LORA_FIELD_COUNT; i++) {
        payload[i + payloadSize] = types[i];
    }
    payloadSize += LORA_FIELD_COUNT;

    // build payload
    for (int i = 0; i < LORA_FIELD_COUNT; i++) {
        int s = lora_getSizeByType(types[i]);
        if (s == 0) continue;

        lora_payload field = fields[i];
        for (int  j = 0; j < s; j++) {
            payload[payloadSize] = field.bytes[j];
            payloadSize++;
        }
    }

    Serial.print("Send payload: ");
    PrintHex8(payload, payloadSize);
    if (payloadSize > MAX_PAYLOAD_SIZE) {
        payloadSize = MAX_PAYLOAD_SIZE;
        
        Serial.println("WARNING: Payload is too big! Please use different variable types in payload (or less variables).");
        Serial.print("Only the following will be sent: ");
        PrintHex8(payload, payloadSize);
    }

    LMIC_setTxData2(1, (uint8_t*) payload, payloadSize, 0);
}

int lora_getSizeByType(payload_type type) {
    switch (type) {
        case PAYLOAD_TYPE_UNUSED:
            return 0;
        case PAYLOAD_TYPE_BOOLEAN:
        case PAYLOAD_TYPE_BYTE:
        case PAYLOAD_TYPE_CHAR:
        case PAYLOAD_TYPE_UNSIGNEDCHAR:
            return 1;
        case PAYLOAD_TYPE_WORD:
        case PAYLOAD_TYPE_UNSIGNEDINT:
        case PAYLOAD_TYPE_INT:
            return 2;
        case PAYLOAD_TYPE_UNSIGNEDLONG:
        case PAYLOAD_TYPE_LONG:
        case PAYLOAD_TYPE_FLOAT:
            return 4;
    }
    return -1;
}
