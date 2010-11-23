#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "bitop.h"

#include "stdlib.h"
#include "stdio.h"
#include "uart.h"

#define COLOURPORT PORTA
#define CHMAX 3
#define RED 0
#define GREEN 1
#define BLUE 2

//! global buffers
unsigned char internalpwm[CHMAX];
volatile unsigned char pwm[CHMAX];

FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW); // This line is used to set up file access
void setup(void) {
	//Ports
	DDRA=0b00000111;
	PORTA=0b00000000;
	
	DDRB=0b00000000;
	PORTB=0b00000000;
	
	//i2c
	DDRC =0b00000011;
	PORTC=0b00000011;
	
	//UART
	uart_init(19200);
	stdout = stdin = &uart_str;
	printf("Booted!\n");
}

ISR(TIMER0_OVF_vect) {
	static unsigned char softcount=0xFF;
	if(++softcount==0) {
		internalpwm[0]=pwm[0];
		internalpwm[1]=pwm[1];
		internalpwm[2]=pwm[2];
		
		set_bit(COLOURPORT,0);
		set_bit(COLOURPORT,1);
		set_bit(COLOURPORT,2);
	}
	#define CHECKANDSTOP(bit) if(softcount==internalpwm[bit]) clear_bit(COLOURPORT,bit);
	CHECKANDSTOP(0);
	CHECKANDSTOP(1);
	CHECKANDSTOP(2);
}

int main(void) {
	setup();
	
	pwm[0]=255;
	pwm[1]=128;
	pwm[2]=10;
	
	//Prescaler = FCPU
	TCCR0|=(0<<CS02)|(0<<CS01)|(1<<CS00);
	
	//Enable Overflow Interrupt Enable
	TIMSK|=(1<<TOIE0);
	
	//Initialize Counter
	TCNT0=0;
	
	//Enable Global Interrupts
	sei();
	
	while(1) {
		int r,g,b;
		scanf("%2x%2x%2x",&r,&g,&b);
		pwm[0]=r;
		pwm[1]=g;
		pwm[2]=b;
	}
}