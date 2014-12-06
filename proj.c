/*************************************************************
*
*       proj.c - Source code for EE109 Project
*       Oscar Michael Abrina (2 PM Class)
*
*************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lcd.h"
#include "ds1631.h"
#include "buttons.h"
#include "rotary.h"
#include "serial.h"

volatile char thres[2] = {70, 80};
volatile unsigned char button = 1;
volatile unsigned char state = 0; //0: 00, 1: 01, 2: 10, 3: 11

void init_io_port(unsigned char b, unsigned char c, unsigned char d) {
  DDRB |= b;
  DDRC |= c;
  DDRD |= d;
}

char convert(unsigned char* celsius) {
  //use appropriate formula depending on second degree of Celsius
  if(celsius[1]==0x80) {
    return (celsius[0]+1)*4/5 + celsius[0] + 32;
  } else {
    return celsius[0]*4/5 + celsius[0] + 32;
  }
}

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

void make_thres_valid() {
  if(thres[0] > thres[1] && !button) {
    thres[0] = thres[1];
  }
  if(thres[1] < thres[0] && button) {
    thres[1] = thres[0];
  }
}

void init_all() {
  PORTC |= (1<<PC5) | (1<<PC4); //enable pull-up for I2C
  init_io_port(0x07, 0x06, 0xF0);
  init_rotary();
  init_buttons();
  init_lcd();
  ds1631_init();
  ds1631_conv();
  init_serial();
}

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
  char buff[5]; //buffer for printing to LCD later
  char prevf = -128; //for checking if temp changed
  char prevrmtf = -128; //for checking if prev changed
  char prevthres[2] = {-128, -128}; //for checking if threshold changed
  unsigned char celsius[2]; //for storing temp in Celsius

  init_all();
  sei();

  //Print all needed labels
  set_labels();

  while(1) {
    make_thres_valid();
    ds1631_temp(celsius);
    char far = convert(celsius);
    char rmtfar = rx_temp();

    if(prevthres[0] != thres[0]) {
      moveto(0x80+3);
      sprintf(buff, "%d  ", thres[0]);
      stringout(buff);
      prevthres[0] = thres[0];
    }
    if(prevthres[1] != thres[1]) {
      moveto(0x80+12);
      sprintf(buff, "%d  ", thres[1]);
      stringout(buff);
      prevthres[1] = thres[1];
    }
    if(prevf != far) {
      moveto(0xc0+3);
      sprintf(buff, "%d", far);
      stringout(buff);
      tx_temp(far);
      prevf = far;
    }
    if(prevrmtf != rmtfar && rmtfar != -128) {
      moveto(0xc0+12);
      sprintf(buff, "%d", rmtfar);
      stringout(buff);
      prevrmtf = rmtfar;
    }
    moveto(0xd0); //remove cursor
    heat_or_cool(far); //turn on heater or cooler depending on temp
  }

  while (1) {}
}