#ifndef _LORA_H_
#define _LORA_H_

#include <Arduino.h>

const int LORA_FIELD_COUNT = 8;

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

void lora_init();
void lora_prepareTypes(payload_type *types);
void lora_send(payload_type* types, lora_payload *fields);
boolean lora_isSending();
int lora_getSizeByType(payload_type type);

#endif
