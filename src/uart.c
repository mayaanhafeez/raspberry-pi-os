
/*TXD0 UART0 transmit data
RXD0 UART0 recieve data
CTS0 UART0 clear to send data
RTS0 UART0 request to send 

1. Write to GPPUD to set the required control signal (i.e. Pull-up or Pull-Down or neither
to remove the current Pull-up/down)
2. Wait 150 cycles – this provides the required set-up time for the control signal
3. Write to GPPUDCLK0/1 to clock the control signal into the GPIO pads you wish to
modify – NOTE only the pads which receive a clock will be modified, all others will
retain their previous state.
4. Wait 150 cycles – this provides the required hold time for the control signal
5. Write to GPPUD to remove the control signal
6. Write to GPPUDCLK0/1 to remove the clock
0x 7E20 0004 GPFSEL1 GPIO SELECT FUNCTION 1, should be at alt0

selector=get32(GPFSEL1);
  selector &=~(7<<12); #clear gpio14
  selector |= 4<<12; #4= 100 alt0
  selector &=~(7<<15); #clear gpio15
  selector |= 4<<15; #4= 100 alt0


UARTDR: 7:0 DATA
UARTFR: 5 TXFF
UARTIBRD: 15:0 Baud rate divisor
UARTFBRD: 5:0 decimal part of BRD
0x4 for 115200 bitrate (for integer part) 
UARTLCR_H: 6:5 word length, 4 enable FIFO (FEN)

UARTCR:
9: RXE
8: TXE
0: UARTEN
Disable the UART.
Wait for the end of transmission or reception of the current character.
Flush the transmit FIFO by setting the FEN bit to 0 in the Line Control Register, UARTLCR_H.
Reprogram the UARTCR Register.
Enable the UART.


*/
/*
#include "utils.h"
#include "peripherals/gpio.h"
#include "peripherals/uart.h"

void uart_send(char c) {
  while (1){
    if (!(get32(UART_FR)&0b100000)) break;
  }
  put32(UART_DR, c);
}

char uart_recv(void) {
  while (1){
    if (!(get32(UART_FR)&0b10000)) break;
  }
  return get32(UART_DR)&0xFF;
}

void uart_send_string(char* str){
for (int i=0; str[i]!='\0';i++){
    uart_send((char)str[i]);
  }
}
void uart_init(void) {
  put32(UART_CR, get32(UART_CR)&~1); //disable UART
  unsigned int selector;
  selector=get32(GPFSEL1);
  selector &=~(7<<12); //clear gpio14
  selector |= 4<<12; //4= 100 alt0
  selector &=~(7<<15); //clear gpio15
  selector |= 4<<15; //4= 100 alt0
  put32(GPFSEL1, selector);

  put32(GPPUD,0);
  delay(150);
  put32(GPPUDCLK0, (1<<14)|(1<<15));
  delay (150);
  put32(GPPUDCLK0,0);

  put32(UART_LCRH, get32(UART_LCRH)&~16); //flush FIFO
  put32(UART_IBRD, 4); //baud rate 115200
  put32(UART_FBRD, 0);
  put32(UART_ICR, 0x7FF); //clear interupts
  put32(UART_LCRH, get32(UART_LCRH)|112); //set bit 5:6 to 11 (word length to 8 bits) + enable FIFO
  put32(UART_CR, get32(UART_CR)|1|512|256); //enable UART+txe+rxe
}
*/
