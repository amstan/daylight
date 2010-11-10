#include <pic16f88.h>
#include "bitop.h"

void delay(int count) {
	int i,j;
	for(i=0;i<count;i++)
		for(j=0;j<10000;j++);
}

void main(void) {
	//initialize the ports
	TRISB=0b1111111;
	TRISA=0b1111110;
	PORTA=0b0000000;
	PORTB=0b0000000;
	
	while(1) {
		delay(10);
		toggle_bit(PORTA,0);
	}
}