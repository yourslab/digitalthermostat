/*************************************************************
*
*       proj.c - Source code for EE109 Project
*       Oscar Michael Abrina (2 PM Class)
*
*************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "ds1631.h"
#include "buttons.h"
#include "lcd.h"
#include "rotary.h"
//#include "serial.h"

volatile unsigned char thres[2] = {60, 70};
short prevthres[2] = {255, 255};
short prevf;
volatile unsigned char button = 1;
char low[5];
char high[5];
char temp[5];
char r_temp[4];
char t_temp[4];
volatile unsigned char state = 0; //0: 00, 1: 01, 2: 10, 3: 11

void init_io_port(unsigned char b, unsigned char c, unsigned char d) {
  DDRB |= b;
  DDRC |= c;
  DDRD |= d;
}

short convert(unsigned char* celsius) {
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

int main(void) {
  PORTC |= (1<<PC5) | (1<<PC4); //enable pull-up for I2C
  init_io_port(0x07, 0x06, 0xF0);
  unsigned char celsius[2];
  init_rotary();
  init_buttons();
  sei();
  init_lcd();
  ds1631_init();
  ds1631_conv();

  while(1) {
    make_thres_valid();
    ds1631_temp(celsius);
    unsigned char far = convert(celsius);
    if((prevthres[0] != thres[0]) || (prevthres[1] != thres[1]) || (prevf != far)) {
      heat_or_cool(far); //turn on heater or cooler depending on temp
      clear(); //clear LCD screen
      //convert all numbers to chars
      sprintf(low, "%d", thres[0]);
      sprintf(high, "%d", thres[1]);
      sprintf(temp, "%d", far);
      //print out to LCD
      stringout("Lo:");
      stringout(low);
      stringout(" ");
      stringout("Hi:");
      stringout(high);
      moveto(0xc0); //next line
      stringout("Loc:");
      stringout(temp);
      moveto(0xd0); //remove cursor
      //These will prevent the dancing LCD screen
      prevthres[0] = thres[0];
      prevthres[1] = thres[1];
      prevf = far;
    }
  }

  while (1) {}
}