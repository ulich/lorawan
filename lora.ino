#define LED_4 10 // LED 4
#define LED_3 9 // 16  LED 3
#define LED_2 6 // 15  LED 2
#define LED_1 5 // LED 1


#include <stdlib.h>
#include <lmic.h>
#include "utils.h"
#include "lora_utils.h"
#include "thermistor.h"

static osjob_t sendjob;
const unsigned TX_INTERVAL = 30;


void onEvent (ev_t ev) {
    switch (ev) {
        // data sent out
        case EV_TXCOMPLETE:
            Serial.println(" ");
            Serial.println("TX COMPLETE");
            digitalWrite(LED_3, LOW);
            digitalWrite(LED_1, LOW);
            digitalWrite(LED_BUILTIN, LOW);
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

    digitalWrite(LED_BUILTIN, HIGH);
}




void setup() {
    Serial.println("SCS Edu-Kit init");
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_4, OUTPUT);
    digitalWrite(LED_4, HIGH);  // power on
    pinMode(LED_3, OUTPUT);
    pinMode(LED_2, OUTPUT);
    pinMode(LED_1, OUTPUT);
    os_init();
    
    Serial.begin(9600);
    Serial.println("SETUP!");
    
    lora_init();
    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}
