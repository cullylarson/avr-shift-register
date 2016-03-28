#include <avr/io.h>
#include <util/delay.h>

#define PIN_SER      PB0 // pin 14 on register
#define PIN_RCLK     PB1 // pin 12 on register
#define PIN_SRCLK    PB2 // pin 11 on register

// NOTE: pin 10 (SRCLR') on register needs to go to VCC
// NOTE: pin 13 (OE') on register needs to go to GND

// write digital "high" to pin <pn> on portb
#define GOHI(pn) PORTB |= (1<<pn)

// write digital "low" to pin <pn> on portb
#define GOLO(pn) PORTB &= ~(1<<pn)

void put(uint8_t);
void putBit(uint8_t);
void latch();
void cycleClock();
void longDelay(uint16_t);
void setup();
uint8_t rotateLeft(uint8_t);
uint8_t rotateRight(uint8_t);

int main(void) {
    setup();
    uint8_t left = 0b11000000;
    uint8_t right = 0b00000011;

    while(1) {
        put(left | right);
        _delay_ms(100);
        left = rotateRight(left);
        right = rotateLeft(right);
    }

    return 0;
}

void setup() {
    // set output pins on portb (the only port on attiny85)
    DDRB |= (1 << PIN_SER) | (1 << PIN_SRCLK) | (1 << PIN_RCLK);
}

void longDelay(uint16_t ms) {
    for(ms /= 10; ms>0; ms--) _delay_ms(10);
}

/*
 * Shifts bits left, but wraps the left-most bit back around
 */
uint8_t rotateLeft(uint8_t toRotate) {
    uint8_t carry = (toRotate & 0b10000000) == 0b10000000;
    return (toRotate << 1 | carry);
}

/*
 * Shifts bits right, but wraps the right-most bit back around
 */
uint8_t rotateRight(uint8_t toRotate) {
    uint8_t carry = ( toRotate & 1 ) ? 0b10000000 : 0;
    return (toRotate >> 1 | carry);
}

/*
 * Writes a byte out serially to PIN_SER
 */
void put(uint8_t toPut) {
    uint8_t i;
    for(i = 0; i < 8; i++) {
        putBit(toPut & 1);
        toPut >>= 1;
    }

    latch();
}

/*
 * Writes a bit out serially to PIN_SER
 */
void putBit(uint8_t bit) {
    if(bit == 0) GOLO(PIN_SER);
    else         GOHI(PIN_SER);

    cycleClock();
}

/*
 * Tells the shift register to output all the bits it's read
 */
void latch() {
    GOHI(PIN_RCLK);
    GOLO(PIN_RCLK);
}

/*
 * Tells the shift register to read a bit
 */
void cycleClock() {
    GOHI(PIN_SRCLK);
    GOLO(PIN_SRCLK);
}
