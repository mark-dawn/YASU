#ifndef SATURN_H_
#define SATURN_H_

// define pins for interface with the saturn controller
// outputs
#define TR PD0
#define TH PD1

// input 1st controller
#define DDR_0 DDRC
#define PORT_0 PORTC
#define PIN_0 PINC
// input 2nd controller
#define DDR_1 DDRB
#define PORT_1 PORTB
#define PIN_1 PINB
// positions def
#define TL 0
#define D0 1
#define D1 2
#define D2 3
#define D3 4
#define DATA_MSK 0b00011110

/* controller inputs and encoding
 * ========================================
 *   ______                       ______
 *  |  L                             R  |
 *
 *        |                (X)  (Y)  (Z)
 *      --o--              (A)  (B)  (C)
 *        |      <start>
 *
 *
 * REMEBER: TTL logic is good at sinking current
 * most of the time the controller will be in high state (buttons are grounded when pressed)
 * => set inputs with pull-ups on
 *
 * TR TH | D0 D1 D2 D3
 * -------------------
 *  0  0 |  Z  Y  X  R
 *  1  0 |  B  C  A  S
 *  0  1 |  ^  v  <  >
 *  1  1 |  -  -  -  L   <- FUCKING SEVENTH BUTTON
 *
 *  we need two bytes to encode this into the usb descriptor
 */

struct sat_controller{
    /* buffer to hold the state, ordered already to be transmitted
     * L R Z Y X C B A
     * ^ v < > 0 0 0 S
     */
    uint8_t state[2];
    // func pointer for the update type. Is NULL if the controller is disconnected
    void (*upd)(uint8_t*, uint8_t);
};

// function declarations
void sat_init(struct sat_controller*);
#endif

