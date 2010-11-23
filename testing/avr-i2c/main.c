#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "bitop.h"

#include "stdlib.h"
#include "stdio.h"
#include "uart.h"

#include "i2cmaster.h"

#define COLOURPORT PORTA
#define RED 0
#define GREEN 1
#define BLUE 2

#define I2C_ADD 0b01110010
unsigned char i2c_get(unsigned char reg) {
	char ret;
	ret = i2c_start(I2C_ADD+I2C_WRITE);
	if ( ret ) {
		i2c_stop();
		COLOURPORT=1<<RED;
		printf("i2c_start error, %d\n",ret);
	} else {
		i2c_write(reg); //location
		i2c_rep_start(I2C_ADD+I2C_READ);
		ret = i2c_readNak(); //read the data
		i2c_stop();
	}
	return ret;
}

int i2c_put(unsigned char reg,unsigned char data) {
	char ret;
	ret = i2c_start(I2C_ADD+I2C_WRITE);
	if ( ret ) {
		i2c_stop();
		COLOURPORT=1<<RED;
		printf("i2c_start error, %d\n",ret);
	} else{
		i2c_write(reg);  //location
		i2c_write(data); //data
		i2c_stop();
	}
	return ret;
}


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
	setup();
	i2c_init();
	
	//i2c_put(0x00,0x00);
	
	for(unsigned char i=0x00;i<=0x07;i++) {
		printf("address %d, data %d\n\n",i,i2c_get(i));
	}
}