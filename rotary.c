#include <avr/io.h>
#include <avr/interrupt.h>

#include "rotary.h"

void init_rotary() {
  PCICR |= (1<<PCIE2);
  PCMSK2 = 0b00001100;
  PORTD |= (1<<PD2) | (1<<PD3); //enable pull-up

  unsigned char a = (PIND & (1<<PD2));
  unsigned char b = (PIND & (1<<PD3));

  //initialize the states
  if(a && b) {
    state = 3;
  } else if (a && !b) {
    state = 1;
  } else if (!a && b) {
    state = 2;
  } else {
    state = 0;
  }
}

ISR(PCINT2_vect) {
  unsigned char a = (PIND & (1<<PD2));
  unsigned char b = (PIND & (1<<PD3));

  if(state == 0) { //00
    if(b) { //00->10
      thres[button]--;
      state = 2;
    } else if(a) { //00->01
      thres[button]++;
      state = 1;
    }
  } else if(state == 1) { //01
    if(a==0) { //01->00
      thres[button]--;
      state = 0;
    } else if(b) { //01->11
      thres[button]++;
      state = 3;
    }
  } else if (state == 2) { //1
    if (a) { //10->11
      thres[button]--;
      state = 3;
    } else if (b==0) { //10->00
      thres[button]++;
      state = 0;
    }
  } else if (state == 3) { //11
    if (a==0) { //11->10
      thres[button]++;
      state = 2;
    } else if (b==0) { //11->01
      thres[button]--;
      state = 1;
    }
  }
}