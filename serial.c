#include <avr/io.h>
#include "serial.h"

#define FOSC 16000000 // Clock frequency
#define BAUD 9600 // Baud rate used
#define MYUBRR FOSC/16/BAUD-1 // Value for UBRR0

void init_serial() {
  UBRR0 = MYUBRR; // Set baud rate
  UCSR0B |= (1 << TXEN0 | 1 << RXEN0);
  UCSR0C = (3 << UCSZ00);
}

char rx_char() {
  //if complete flag is high
  if (UCSR0A & (1 << RXC0)) {
    return UDR0;
  } else {
    return '\0';
  }
}

void tx_char(char ch) {
  //wait for transmitter data register empty
  while ((UCSR0A & (1<<UDRE0)) == 0) {}
  UDR0 = ch;
}

char rx_temp() {
  char r_temp[4];
  unsigned char flag = 0;
  unsigned char i;
  for(i=0; i<4; i++) {
    r_temp[i] = rx_char();
    //validate chars (-128 is extremely unlikely)
    if(r_temp[i] == '\0') {
      return -128;
    }
    if(i == 0) {
      if(r_temp[i] != '+' && r_temp[i] != '-') {
        return -128;
      }
    } else if(i >= 1 && i <= 3) {
      if(r_temp[i] < '0' || r_temp[i] > '9') {
        return -128;
      }
    }
  }

  //convert string to number
  char rmt = (r_temp[1]-'0')*100 + (r_temp[2]-'0')*10 + (r_temp[3]-'0');
  if(r_temp[0] == '-') {
    rmt = 0-rmt;
  }
  return rmt;
}



void tx_temp(char far) {
  char t_temp[5];
  if(far>=0) {
    t_temp[0] = '+';
  } else {
    t_temp[0] = '-';
  }
  t_temp[1] = (far/100)+'0';
  t_temp[2] = (far/10%10)+'0';
  t_temp[3] = (far%10)+'0';

  unsigned char i;
  for(i=0; i<4; i++) {
    tx_char(t_temp[i]);
  }
}