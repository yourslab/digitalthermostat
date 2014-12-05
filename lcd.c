#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"

void writenibble(unsigned char);

/*
  init_lcd - Do various things to initialize the LCD display
*/
void init_lcd() {
  _delay_ms(15);              // Delay at least 15ms

  writenibble(0x03);          // Use writenibble to send 0011
  _delay_ms(5);               // Delay at least 4msec

  writenibble(0x03);          // Use writenibble to send 0011
  _delay_us(120);             // Delay at least 100usec

  writenibble(0x03);          // Use writenibble to send 0011
  
  writenibble(0x02);          // Use writenibble to send 0010
  _delay_ms(2);               // Function Set: 4-bit interface
  
  writebyte(0x28, 0x00);      // Function Set: 4-bit interface, 2 lines
  _delay_ms(2);

  writebyte(0x0F, 0x00);      // Display and cursor on
  _delay_ms(2);

  clear();                    // Clear everything
  PORTB |= 0x04;              // Enable backlight
}

void clear() {
  writebyte(0x01, 0x00);      // Clear LCD
}

/*
  stringout - Print the contents of the character string "str"
  at the current cursor position.
*/
void stringout(char *str) {
  while(*str != '\0') {
    writebyte(*str, 0x01);
    str++;
  }
}

/*
  moveto - Move the cursor to the postion "pos"
*/
void moveto(unsigned char pos) {
  writebyte(pos, 0x00);
}

/*
  writebyte - Output a byte to the LCD display instruction register.
*/
void writebyte(unsigned char x, unsigned char rs) {
  //clear previous value in register
  PORTB &= 0xFE;
  //choose register
  PORTB |= rs;

  //set upper and lower bits
  unsigned char upper = ((x & 0xF0) >> 4);
  unsigned char lower = (x & 0x0F);

  //write upper bits
  writenibble(upper);
  //write lower bits
  writenibble(lower);

  //delay to finish
  _delay_ms(2);
}

/*
  writenibble - Output four bits from "x" to the display
*/
void writenibble(unsigned char x) {
  x = (x << 4);
  PORTD &= 0x0F;
  PORTD |= x;

  //enable signal
  PORTB |= 0x02;
  _delay_us(1);
  PORTB &= ~(0x02);
}