#include "serial.h"

UCSR0B |= (1 << TXEN0 | 1 << RXEN0);
UCSR0C = (3 << UCSZ00);

char rx_char() {
  // Wait for receive complete flag to go high
  while ( !(UCSR0A & (1 << RXC0)) ) {}
  return UDR0;
}

void tx_char(char ch) {
  // Wait for transmitter data register empty
  while ((UCSR0A & (1<<UDRE0)) == 0) {}
  UDR0 = ch;
}

void get_remote_temp() {
  for(int i=0; i<4; i++) {
    r_temp[i] = rx_char();
  }
}

void put_temp(short far) {
  if(far>=0) {
    t_temp[0] = '+';
  } else {
    t_temp[0] = '-';
  }
  t_temp[1]
}