#define __16f88
#include "pic/pic16f88.h"
#include "bitop.h"

typedef unsigned int config;
config at 0x2007 __CONFIG = _CP_OFF & 
 _WDT_OFF & 
 _BODEN_OFF & 
 _PWRTE_OFF & 
 _INTRC_IO & 
 _MCLR_ON & 
 _LVP_OFF;

void delay(int count) {
	int i,j;
	for(i=0;i<count;i++)
		for(j=0;j<10000;j++);
}

void main(void) {
	//initialize the ports
	OSCCON=0b01110010; //Speed up the cpu so the delay code is not slow
	
	TRISA=0b11111111;
	PORTA=0b00000000;
	
	TRISB=0b11111101;
	PORTB=0x00000010;
	
	while(1) {
		delay(5);
		toggle_bit(PORTB,1);
	}
}