#include <stdlib.h>
#include <lmic.h>
#include <Servo.h>
#include "utils.h"
#include "lora.h"
#include "thermistor.h"
#include "SR04.h"

#define LED_1 5
#define LED_2 6
#define LED_3 11
#define LED_4 12
#define TRANSMITTING_LED LED_3

#define THERMISTOR_PORT A0
#define PHOTOCELL_PORT A1
#define ULTRASONIC_TRIG_PIN 10
#define ULTRASONIC_ECHO_PIN 9
#define SERVO_PORT 13

static osjob_t sendjob;
const unsigned TX_INTERVAL = 30;

Servo servo;
SR04 sr04 = SR04(ULTRASONIC_ECHO_PIN, ULTRASONIC_TRIG_PIN);

void onEvent (ev_t ev) {
    switch (ev) {
        // data sent out
        case EV_TXCOMPLETE:
            Serial.println(" ");
            Serial.println("TX COMPLETE");
            digitalWrite(TRANSMITTING_LED, LOW);
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

    digitalWrite(TRANSMITTING_LED, HIGH);

    payload_type types[LORA_FIELD_COUNT];
    lora_payload fields[LORA_FIELD_COUNT];
    lora_prepareTypes(types);

    types[0] = PAYLOAD_TYPE_FLOAT;
    fields[0].f = thermistor_measureTemperature(THERMISTOR_PORT);

    types[1] = PAYLOAD_TYPE_INT;
    fields[1].i = analogRead(PHOTOCELL_PORT);

    types[2] = PAYLOAD_TYPE_INT;
    fields[2].i = sr04.DistanceAvg();
 
    lora_send(types, fields);
}




void setup() {
    Serial.println("SCS Edu-Kit init");
    pinMode(LED_BUILTIN, OUTPUT);

    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
    pinMode(LED_3, OUTPUT);
    pinMode(LED_4, OUTPUT);
    
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(TRANSMITTING_LED, LOW);

    servo.attach(SERVO_PORT);

    os_init();
    
    Serial.begin(9600);
    Serial.println("SETUP!");
    
    lora_init();
    do_send(&sendjob);
}

int lastServoPos = -1;
void loop() {
    // int brightness = analogRead(PHOTOCELL_PORT);
    // if (brightness < 400) {
    //     if (lastServoPos != 180) {
    //         servo.write(180);
    //         lastServoPos = 180;
    //         Serial.println("Moving servo to 180°");
    //     }
    // }
    // else {
    //     if (lastServoPos != 0) {
    //         servo.write(0);
    //         lastServoPos = 0;
    //         Serial.println("Moving servo to 0°");
    //     }
    // }

    // int distance = sr04.DistanceAvg();
    // Serial.print("Distance ");
    // Serial.print(distance);
    // Serial.println("cm");

    os_runloop_once();
}
