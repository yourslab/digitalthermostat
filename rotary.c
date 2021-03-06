#include <avr/io.h>
#include <avr/interrupt.h>

#include "header/rotary.h"

extern volatile unsigned char button;
extern volatile char thres[2];
extern volatile unsigned char state; //0: 00, 1: 01, 2: 10, 3: 11

/*
  init_rotary - Initializes state and configures certain ports.
*/
void init_rotary() {
  PCICR |= (1<<PCIE2);
  PCMSK2 = 0b00001100;
  PORTD |= (1<<PD2) | (1<<PD3); // Enable pull-up for rotary encoder

  unsigned char pin_d = PIND; // Read PIND only once
  unsigned char a = (pin_d & (1<<PD2));
  unsigned char b = (pin_d & (1<<PD3));

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

/*
  ISR(PCINT2_vect) - Changes value of threshold and state depending
  on rotary encoder and button.
*/
ISR(PCINT2_vect) {
  unsigned char pin_d = PIND; // Read PIND only once
  unsigned char a = (pin_d & (1<<PD2));
  unsigned char b = (pin_d & (1<<PD3));

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