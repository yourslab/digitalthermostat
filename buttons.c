#include <avr/io.h>
#include <avr/interrupt.h>

#include "buttons.h"

void init_buttons() {
  PCICR |= (1<<PCIE0);
  PCMSK0 = 0b00110000;
  PORTB |= (1<<PB5) | (1<<PB4); //enable pull-up
}

ISR(PCINT0_vect) {
  unsigned char red = !(PINB & (1<<PB4));
  unsigned char green = !(PINB & (1<<PB5));
  if(red && !green) {
    button = 0;
  } else if(green && !red) {
    button = 1;
  }
}