#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include "Saturn.h"

void std_update(uint8_t* buf, uint8_t num) {
    uint8_t tmp;
    volatile uint8_t* pin = num ? &(PIN_1) : &(PIN_0);
    // L---1
    PORTD |= (1<<TR) | (1<<TH);
    _delay_us(4);
    buf[0] = ((~(*pin))<<3) & 0b10000000;
    // SACB1
    PORTD &= ~(1<<TH);
    _delay_us(4);
    tmp = ~(*pin);
    buf[1] = (tmp>>4) & 0b00000001;
    buf[0] |= (tmp>>3) & 0b00000001;
    buf[0] |= (tmp<<0) & 0b00000110;
    //buf[0] |= (tmp<<4) & 0b10000000;
    // RXYZ1
    PORTD &= ~(1<<TR);
    _delay_us(4);
    tmp = ~(*pin);
    buf[0] |= (tmp<<0) & 0b00001000;
    buf[0] |= (tmp<<4) & 0b00100000;
    buf[0] |= (tmp<<2) & 0b01010000;
    //buf[0] |= (tmp<<2) & 0b00000100;
    // <>v^1
    PORTD |= 1<<TH;
    _delay_us(4);
    tmp = ~(*pin);
    switch (tmp & 0b00000110) {
        case 2:
            buf[1] |= 0b11000000;
            break;
        case 4:
            buf[1] |= 0b01000000;
            break;
    }
    switch (tmp & 0b00011000) {
        case 8:
            buf[1] |= 0b00110000;
            break;
        case 16:
            buf[1] |= 0b00010000;
            break;
    }
    return;
}

void sat_init(struct sat_controller* dev) {
    // port config in
    DDR_0 &= ~(
             (1<<TL) |
             (1<<D0) |
             (1<<D1) |
             (1<<D2) |
             (1<<D3) );
    DDR_1 &= ~(
             (1<<TL) |
             (1<<D0) |
             (1<<D1) |
             (1<<D2) |
             (1<<D3) );
    // set pull-ups to 0 at first
    PORT_0 = 0x00;
    PORT_1 = 0x00;
    // port config out
    DDRD |= (1<<TR) | (1<<TH);
    // struct init
    for(uint8_t i = 0; i < 2; i++) {
        dev[i].state[0] = 0x00;
        dev[i].state[1] = 0x00;
    }
    if (PIN_0 & (1<<TL)) {
        dev[0].upd = &std_update;
        PORT_0 = (1<<TL) |
                 (1<<D0) |
                 (1<<D1) |
                 (1<<D2) |
                 (1<<D3);
    }
    else {
        dev[0].upd = NULL;
    }
    if (PIN_1 & (1<<TL)) {
        dev[1].upd = &std_update;
        PORT_1 = (1<<TL) |
                 (1<<D0) |
                 (1<<D1) |
                 (1<<D2) |
                 (1<<D3);
    }
    else {
        dev[1].upd = NULL;
    }
    return;
}
