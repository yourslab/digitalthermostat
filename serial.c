#include <avr/io.h>
#include <avr/interrupt.h>

#include "header/serial.h"

#define FOSC 16000000 // Clock frequency
#define BAUD 9600 // Baud rate used
#define MYUBRR FOSC/16/BAUD-1 // Value for UBRR0

char rx_char();
void tx_char(char ch);

/*
  init_serial - Sets the BUAD rate and other configurations
*/
void init_serial() {
  UBRR0 = MYUBRR; // Set baud rate
  UCSR0B |= (1 << TXEN0 | 1 << RXEN0);
  UCSR0C = (3 << UCSZ00);
}

/*
  got_byte - Checks if there is an rx byte available.
*/
unsigned char got_byte() {
  // If complete flag is high
  if (UCSR0A & (1 << RXC0)) {
    return 1;
  } else {
    return 0;
  }
}

/*
  rx_char - Waits until an rx byte can be received
*/
char rx_char() {
 // Wait for receive complete flag to go high
 while (!(UCSR0A & (1 << RXC0))) {}
 return UDR0;
}

/*
  tx_char - Transmits a tx byte for another Arduino
*/
void tx_char(char ch) {
  // Wait for transmitter data register empty
  while ((UCSR0A & (1<<UDRE0)) == 0) {}
  UDR0 = ch;
}

/*
  get_rx_temp - Gets rx bytes, converts to valid value, then
  passes it to a variable.
*/
void get_rx_temp(char* rmtfar) {
  cli(); // That way, no interrupts while receiving char
  char r_temp[4];
  unsigned char i;

  for(i=0; i<4; i++) {
    r_temp[i] = rx_char();
    // Validate chars received
    if(i == 0) {
      if(r_temp[i] != '+' && r_temp[i] != '-') {
        return;
      }
    } else {
      if(r_temp[i] < '0' || r_temp[i] > '9') {
        return;
      }
    }
  }
  
  // Convert all chars to a valid number
  char rmt = (r_temp[1]-'0')*100 + (r_temp[2]-'0')*10 + (r_temp[3]-'0');
  if(r_temp[0] == '-') {
    rmt = 0-rmt;
  }
  *rmtfar = rmt;
  sei(); // Re-enable the global interrupts
}

/*
  tx_temp - Converts local temp to bytes and passes it to 
  tx_char(char ch) function.
*/
void tx_temp(char far) {
  char t_temp[4];
  if(far>=0) {
    t_temp[0] = '+';
  } else {
    t_temp[0] = '-';
  }
  t_temp[1] = (far/100)+'0'; // Hundreds digit
  t_temp[2] = (far/10%10)+'0'; // Tens digit
  t_temp[3] = (far%10)+'0'; // Ones digit

  unsigned char i;
  for(i=0; i<4; i++) {
    tx_char(t_temp[i]);
  }
}