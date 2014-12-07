#include <avr/io.h>
#include <avr/interrupt.h>

#include "header/buttons.h"

extern volatile unsigned char button;

/*
  init_buttons - Configures certain ports.
*/
void init_buttons() {
  PCICR |= (1<<PCIE0);
  PCMSK0 = 0b00110000;
  PORTB |= (1<<PB5) | (1<<PB4); // Enable pull-up
}

/*
  ISR(PCINT0_vect) - Checks to see if button was pressed
  and changes button state variable accordingly.
*/
ISR(PCINT0_vect) {
  unsigned char red = !(PINB & (1<<PB4));
  unsigned char green = !(PINB & (1<<PB5));
  if(red && !green) {
    button = 0;
  } else if(green && !red) {
    button = 1;
  }
}