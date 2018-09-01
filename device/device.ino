#include <stdlib.h>
#include <lmic.h>
#include "utils.h"
#include "lora.h"
#include "thermistor.h"

#define LED_3 11

static osjob_t sendjob;
const unsigned TX_INTERVAL = 30;


void onEvent (ev_t ev) {
    switch (ev) {
        // data sent out
        case EV_TXCOMPLETE:
            Serial.println(" ");
            Serial.println("TX COMPLETE");
            digitalWrite(LED_3, LOW);
            os_setTimedCallback(&sendjob, os_getTime() + sec2osticks(TX_INTERVAL), do_send);
            break;
        default:
            break;
    }
}

void do_send(osjob_t* job) {
    if (lora_isSending()) {
        return;
    }

    digitalWrite(LED_3, HIGH);

    payload_type types[LORA_FIELD_COUNT];
    lora_payload fields[LORA_FIELD_COUNT];
    lora_prepareTypes(types);

    types[0] = PAYLOAD_TYPE_FLOAT;
    fields[0].f = thermistor_measureTemperature(A0);

    lora_send(types, fields);
}




void setup() {
    Serial.println("SCS Edu-Kit init");
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_3, OUTPUT);

    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(LED_3, LOW);

    os_init();
    
    Serial.begin(9600);
    Serial.println("SETUP!");
    
    lora_init();
    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}
