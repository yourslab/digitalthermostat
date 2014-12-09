/*************************************************************
*
*       proj.c - Source code for EE109 Project
*       Oscar Michael Abrina (2 PM Class)
*
*************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "header/lcd.h"
#include "header/ds1631.h"
#include "header/buttons.h"
#include "header/rotary.h"
#include "header/serial.h"

volatile char thres[2] = {70, 80};
volatile unsigned char button = 1;
volatile unsigned char state = 0; // 0: 00, 1: 01, 2: 10, 3: 11

/*
  init_io_port - Sets DDR of IO ports depending on desired values.
*/
void init_io_port(unsigned char b, unsigned char c, unsigned char d) {
  DDRB |= b;
  DDRC |= c;
  DDRD |= d;
}

/*
  convert - Converts Celsius to Fahrenheit.
*/
char convert(unsigned char* celsius) {
  // Use appropriate formula depending on second degree of Celsius
  if(celsius[1]==0x80) {
    // +1 simply because conversion function is linear
    return ((celsius[0])*4/5 + celsius[0] + 32)+1;
  } else {
    return celsius[0]*4/5 + celsius[0] + 32;
  }
}

/*
  heat_or_cool - Turns on heater or aircon depending on local temp.
*/
void heat_or_cool(unsigned char f) {
  if(f>thres[1]) {
    PORTC |= 0x04; //turn on aircon (green LED)
  } else {
    PORTC &= ~(0x04); //turn off aircon (green LED)
  } 
  if(f<thres[0]) {
    PORTC |= 0x02; //turn on heater (red LED)
  } else {
    PORTC &= ~(0x02); //turn off heater (red LED)
  }
}

/*
  make_thres_valid - Makes sure that upper threshold doesn't go
  below lower threshold and vice versa.
*/
void make_thres_valid() {
  if(thres[0] > thres[1] && !button) {
    thres[0] = thres[1];
  }
  if(thres[1] < thres[0] && button) {
    thres[1] = thres[0];
  }
}

/*
  init_all - Calls all init functions and configures ports
  outside functionality of other .c files.
*/
void init_all() {
  PORTC |= (1<<PC5) | (1<<PC4); // Enable pull-up for I2C
  PORTB &= ~(1<<PB3); // Enable tri-state buffer
  init_io_port(0x0F, 0x06, 0xF0);
  init_rotary();
  init_buttons();
  init_lcd();
  ds1631_init();
  ds1631_conv();
  init_serial();
}

/*
  set_labels - Prints labels for LCD screen.
*/
void set_labels() {
  stringout("Lo:");
  moveto(0x80+9);
  stringout("Hi:");
  moveto(0xc0);
  stringout("LC:");
  moveto(0xc0+8);
  stringout("RMT:");
}

int main() {
  char rmtfar; // Stores current temperature from a remote Arduino
  char buff[5]; // Buffer for printing to LCD later
  char prevf = -128; // For checking if temp changed
  char prevthres[2] = {-128, -128}; // For checking if threshold changed
  unsigned char celsius[2]; // For storing temp in Celsius

  init_all();
  sei(); // Enable global interrupts

  set_labels();

  while(1) {
    make_thres_valid();
    ds1631_temp(celsius);
    char far = convert(celsius);
    
    // Low threshold changed
    if(prevthres[0] != thres[0]) {
      moveto(0x80+3);
      sprintf(buff, "%d  ", thres[0]);
      stringout(buff);
      prevthres[0] = thres[0];
    }
    // High threshold changed
    if(prevthres[1] != thres[1]) {
      moveto(0x80+12);
      sprintf(buff, "%d  ", thres[1]);
      stringout(buff);
      prevthres[1] = thres[1];
    }
    // Local temperature changed
    if(prevf != far) {
      moveto(0xc0+3);
      sprintf(buff, "%d", far);
      stringout(buff);
      tx_temp(far);
      prevf = far;
    }
    // Serial rx byte received
    if(got_byte()) {
      get_rx_temp(&rmtfar);
      moveto(0xc0+12);
      sprintf(buff, "%d", rmtfar);
      stringout(buff);
    }
    moveto(0xd0); // Remove cursor

    heat_or_cool(far);
  }

  while (1) {}
}