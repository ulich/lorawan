#define LED_4 10 // LED 4
#define LED_3 9 // 16  LED 3
#define LED_2 6 // 15  LED 2
#define LED_1 5 // LED 1


#include <stdlib.h>
#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>


// Die Device-Adress und die Session Keys werden hier eingetragen, damit der Netzwerkserver das GerÃ¤t spÃ¤ter erkennt.
// Diese Keys mÃ¼ssen individuell fÃ¼r jede Edu-Platine eingetragen werden.
static const u4_t DEVADDR = 0x10000000; // <-- BITTE Ã„NDERN
static const String WRITE_KEY = "GQEKYVW13KBD9N1A"; //  <-- BITTE Ã„NDERN

static const PROGMEM u1_t APPSKEY[16] = { 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // <-- BITTE Ã„NDERN
static const PROGMEM u1_t NWKSKEY[16] = { 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // <-- BITTE Ã„NDERN

void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

static osjob_t sendjob;
const unsigned TX_INTERVAL = 30;

// Pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 8,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 4,
  .dio = {3, 19, LMIC_UNUSED_PIN},
};

enum payload_type {
  PAYLOAD_TYPE_UNUSED = 0x00,

  // sizes have been taken from https://learn.sparkfun.com/tutorials/data-types-in-arduino
  PAYLOAD_TYPE_BOOLEAN = 0x01,
  PAYLOAD_TYPE_BYTE = 0x02,
  PAYLOAD_TYPE_CHAR = 0x03,
  PAYLOAD_TYPE_UNSIGNEDCHAR = 0x04,
  PAYLOAD_TYPE_WORD = 0x05,
  PAYLOAD_TYPE_UNSIGNEDINT = 0x06,
  PAYLOAD_TYPE_INT = 0x07,
  PAYLOAD_TYPE_UNSIGNEDLONG = 0x08,
  PAYLOAD_TYPE_LONG = 0x09,
  PAYLOAD_TYPE_FLOAT = 0x0a
};

const int MAX_PAYLOAD_SIZE = MAX_LEN_FRAME - 13; // Max. LMIC packet length minus 13 byte LoRa Header

const int FIELD_COUNT = 8;

typedef union {
  boolean b;
  byte by;
  char c;
  unsigned char uc;
  word w;
  unsigned int ui;
  int i;
  unsigned long ul;
  long l;
  float f;

  byte bytes[4];
} lora_payload;


/* Diese Methode reagiert auf verschiedene Arten von Events. Wir reagieren hier nur auf EV_TXCOMPLETE, welches auftritt,
   wenn die Platine eine Ãœbertragung beendet hat.
*/
void onEvent (ev_t ev) {
  switch (ev) {
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


int getSizeByType(payload_type type); // declaration is needed to fig ugly bug in arduino building system with enums
int getSizeByType(payload_type type) {
  switch (type) {
    case PAYLOAD_TYPE_UNUSED:
      return 0;
      break;
    case PAYLOAD_TYPE_BOOLEAN:
    case PAYLOAD_TYPE_BYTE:
    case PAYLOAD_TYPE_CHAR:
    case PAYLOAD_TYPE_UNSIGNEDCHAR:
      return 1;
      break;
    case PAYLOAD_TYPE_WORD:
    case PAYLOAD_TYPE_UNSIGNEDINT:
    case PAYLOAD_TYPE_INT:
      return 2;
      break;
    case PAYLOAD_TYPE_UNSIGNEDLONG:
    case PAYLOAD_TYPE_LONG:
    case PAYLOAD_TYPE_FLOAT:
      return 4;
      break;
  }
  return -1;
}


void PrintHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
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


void do_send(osjob_t* j) {
  if (LMIC.opmode & OP_TXRXPEND) {
    // currently another sending is in progress
  } else {
    digitalWrite(LED_3, HIGH);


    payload_type types[FIELD_COUNT];
    lora_payload fields[FIELD_COUNT];
    for (int i = 0; i < FIELD_COUNT; i++) {
      types[i] = PAYLOAD_TYPE_UNUSED;
    }


    // define payload here
    types[0] = PAYLOAD_TYPE_FLOAT;
    fields[0].f = 19.3;

    types[1] = PAYLOAD_TYPE_INT;
    fields[1].i = 120;

    types[2] = PAYLOAD_TYPE_INT;
    fields[2].i = -13;


    byte payload[WRITE_KEY.length() + FIELD_COUNT + FIELD_COUNT * 4]; // maximum size of variable is 56, but we can only transmit 51 bytes. So we have to check before sending
    // 56 = 16 (WRITE_KEY) + 8 (Payload Designator) + 8*4 (maximum packets)
    int payloadSize = 0;

    // add WRITE_KEY to payload
    for (int i = 0; i < WRITE_KEY.length(); i++) {
      payload[i] = WRITE_KEY[i];
      payloadSize++;
    }

    // size of the designators
    for (int i = 0; i < FIELD_COUNT; i++) {
      payload[i + payloadSize] = types[i];
    }
    payloadSize += FIELD_COUNT;

    // build payload
    for (int i = 0; i < FIELD_COUNT; i++) {
      int s = getSizeByType(types[i]);
      if (s == 0) continue;

      lora_payload field = fields[i];
      for (int  j = 0; j < s; j++) {
        payload[payloadSize] = field.bytes[j];
        payloadSize++;
      }
    }

    PrintHex8(payload, payloadSize);
    if (payloadSize > MAX_PAYLOAD_SIZE) {
      Serial.println("WARNING: Payload is too big! Please use different variable types in payload (or less variables).");
      Serial.print("Only the following will be sent: ");
      payloadSize = MAX_PAYLOAD_SIZE;
    }

    PrintHex8(payload, payloadSize);
    LMIC_setTxData2(1, (uint8_t*) payload, payloadSize, 0);

    digitalWrite(LED_BUILTIN, HIGH);
  }
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
  LMIC_reset();
  Serial.begin(9600);
  Serial.println("SETUP!");
  uint8_t appskey[sizeof(APPSKEY)];
  uint8_t nwkskey[sizeof(NWKSKEY)];
  memcpy_P(appskey, APPSKEY, sizeof(APPSKEY));
  memcpy_P(nwkskey, NWKSKEY, sizeof(NWKSKEY));
  LMIC_setSession (0x1, DEVADDR, nwkskey, appskey);
  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
  LMIC_setLinkCheckMode(0);
  LMIC_setAdrMode(0);
  LMIC_setDrTxpow(DR_SF8, 14);
  do_send(&sendjob);
}


void loop() {
  os_runloop_once();
}