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

#define COLOURPORT PORTB
#define RED 0
#define GREEN 1
#define BLUE 2

int abs(int x) {
	if(x<0) x*=-1;
	return x;
}

void main(void) {
	int i;
	int t; //use for a crude delay
	
	long int r,g,b,cycle;
	long int steps,step; //used for animation
	
	//initialize the ports
	OSCCON=0b01110000; //Speed up the cpu so the delay code is not slow
	
	TRISA=0b11111111;
	PORTA=0b00000000;
	
	TRISB=0b11111000;
	PORTB=0b00000000;
	
	cycle=255;
	r=255;
	g=255;
	b=255;
	
	steps=0;
	step=1;
	
	while(1) {
		//animate
		steps+=step;
		if ((steps==1023)||(steps==0))
			step*=-1;
		
		r=steps/4;
		
		//pwm
		if(r>0) set_bit(COLOURPORT,RED);
		if(g>0) set_bit(COLOURPORT,GREEN);
		if(b>0) set_bit(COLOURPORT,BLUE);
		for(i=0;i<cycle;i++) {
			if(i==r)
				clear_bit(COLOURPORT,RED);
			if(i==g)
				clear_bit(COLOURPORT,GREEN);
			if(i==b)
				clear_bit(COLOURPORT,BLUE);
			for(t=0;t<5;t++);
		}
	}
}