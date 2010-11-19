#define __16f88
#include "pic/pic16f88.h"
#include "bitop.h"

# include "uart.c"

typedef unsigned int config;
config at 0x2007 __CONFIG = _CP_OFF & 
 _WDT_OFF & 
 _BODEN_OFF & 
 _PWRTE_OFF & 
 _INTRC_IO & 
 _MCLR_ON & 
 _LVP_OFF;

void intHand(void) __interrupt 0
{
   if (RCIF) {
        RCIE = 0;
        Usart_Read_Send();
        RCIE = 1;
    }
}

#define COLOURPORT PORTB
#define RED 0
#define GREEN 1
#define BLUE 3

void setup(void) {
	OSCCON=0b01110000; //8MHz

	//Ports
	TRISA=0b11111111;
	PORTA=0b00000000;
 	TRISB=0b11110100;
	PORTB=0b00000000;
	
	Usart_Init(bd9600);
	Usart_Str_tx("Booted!\n");
}

print_number(unsigned char number) {
    unsigned char digit;
    
    digit=number/100;
    number%=100;
    Usart_Write(digit+'0');
    
    digit=number/10;
    number%=10;
    Usart_Write(digit+'0');
    
    Usart_Write(number+'0');
    
    Usart_Write(' ');
}

void main(void) {
	int i;
	int t; //use for a crude delay
	
	long int r,g,b,cycle;
	long int steps,step; //used for animation
	char a;
	
	setup();
	
	cycle=255;
	r=255;
	g=255;
	b=255;
	
	steps=0;
	step=1;
	
	while(1) {
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
		if(Usart_Data_Ready()) {
			clear_bit(COLOURPORT,RED);
			clear_bit(COLOURPORT,GREEN);
			clear_bit(COLOURPORT,BLUE);
			
			r=Usart_Read();
			set_bit(COLOURPORT,RED);
			Usart_Str_tx("r");
            print_number(r);
			
			g=Usart_Read();
			set_bit(COLOURPORT,GREEN);
			Usart_Str_tx("g");
            print_number(g);
			
			b=Usart_Read();
			set_bit(COLOURPORT,BLUE);
			Usart_Str_tx("b");
            print_number(b);
			
			Usart_Str_tx(" New Colour!\n");
		}
	}
}