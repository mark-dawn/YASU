#include <inttypes.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usbconfig.h"
#include "usbdrv.h"
#include "Saturn.h"
#include "comm.h"

 #define F_CPU 12000000UL

int __attribute__((OS_main)) main(void) {
    cli();
    // variables
    PORTD = 0x00;
    struct sat_controller cntr[2];
    // init routines
    sat_init(cntr);
    comm_init();
    // turn on interrupts
    sei();
    // begin main loop
    while(1){
        _delay_ms(10);
        for(uint8_t i = 0; i<2; i++) {
            if (cntr[i].upd) {cntr[i].upd(cntr[i].state, i);}
        }
        comm_data(cntr[0].state, cntr[1].state);
    }
}
