#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "bitop.h"

#include "stdlib.h"
#include "stdio.h"
#include "uart.h"

#include "light.h"

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
	
	//i2c
	DDRC =0b00000011;
	PORTC=0b00000011;
	
	//UART
	uart_init(19200);
	stdout = stdin = &uart_str;
	printf("Booted!\n");
}


int main(void) {
	int r,g,b,cycle;
	setup();
	
	if(light_init()==0)
		printf("Light Sensor started.\n");
	else {
		set_bit(COLOURPORT,RED);
		printf("Error with I2C to lightsensor");
	}
	
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
		//sensor
		if(light_ready()) {
			r=light_level(LIGHT_RED);
			g=light_level(LIGHT_GREEN);
			b=light_level(LIGHT_BLUE);
			printf("light r%d g%d b%d c%d\n",r,g,b,light_level(LIGHT_CLEAR));
		}
	}
}