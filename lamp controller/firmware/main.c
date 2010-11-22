#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "bitop.h"

#include "stdlib.h"
#include "stdio.h"
#include "uart.h"

#define COLOURPORT PORTA
#define RED 0
#define GREEN 1
#define BLUE 2


FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW); // This line is used to set up file access
void setup(void) {
	//Ports
	DDRA=0b00000111;
	PORTA=0b00000000;
	
	DDRB=0b00000000;
	PORTB=0b00000000;
	
	//UART
	uart_init();
	stdout = stdin = &uart_str;
	printf("Booted!\n");
}


int main(void) {
	int r,g,b,cycle;
	setup();
	
	cycle=255;
	r=0;
	g=0;
	b=0;
	
	while(1) {
		//pwm
		if(r>0) set_bit(COLOURPORT,RED);
		if(g>0) set_bit(COLOURPORT,GREEN);
		if(b>0) set_bit(COLOURPORT,BLUE);
		for(int i=0;i<cycle;i++) {
			if(i==r)
				clear_bit(COLOURPORT,RED);
			if(i==g)
				clear_bit(COLOURPORT,GREEN);
			if(i==b)
				clear_bit(COLOURPORT,BLUE);
			_delay_us(10);
		}
		if(test_bit(UCSRA, RXC)) {
			clear_bit(COLOURPORT,RED);
			clear_bit(COLOURPORT,GREEN);
			clear_bit(COLOURPORT,BLUE);
			scanf("%2x%2x%2x",&r,&g,&b);
		}
	}
}