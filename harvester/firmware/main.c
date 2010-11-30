/* main.c

2010-11-20
    got LCD working! the culprits: 1. PORTB defaults to analog output
    2. 'char' defaults to signed in SDCC!
    lesson learned: explicitly specify types for portability!
2010-11-21
    moved LCD into it's own library with cleaned interface
2010-11-22
    added i2c code; resolved issue with code pages?
2010-11-29
    fixed i2c code to avoid read-modify-write issue!

*/

#define __16f88
#include "pic/pic16f88.h"

#include "bitop.h"
#include "lcd.h"
#include "i2c.h"

typedef unsigned int config;
config at 0x2007 __CONFIG = _CP_OFF & 
 _WDT_OFF & 
 _BODEN_OFF & 
 _PWRTE_OFF & 
 _INTRC_IO & 
 _MCLR_ON & 
 _LVP_OFF;


#define LED_PIN RB4

#define nop() \
        _asm\
        nop\
        _endasm


//will not be part of lcd library:
unsigned char Update_Lcd_Buf;
unsigned char Lcd_Ready;

void lcd_type_char(unsigned char, unsigned char, unsigned char);
void lcd_write_int(unsigned int, unsigned char, unsigned char, unsigned char);


void intHand(void) __interrupt 0
{
    static unsigned char led_count;

    if (TMR0IE && TMR0IF) {
        Lcd_Ready = 1;
        if (led_count > 49) {
            LED_PIN = !LED_PIN;
            led_count = 0;
            Update_Lcd_Buf = 1;
        } else {
            led_count++;
        }

        TMR0IF=0;
    }
}

void setup(void) {
    OSCCON=0b01110000; //8MHz

    //Ports
    TRISA=0b00000000;
    PORTA=0b00000000;
    TRISB=0b00000000;
    PORTB=0b00000000;

    CMCON=0b00000111; //Turn off comparator on RA port
    //shouldn't be needed due to POR defaults
    
    ANSEL=0;//This is needed! PORTA defaults to analog input!

    lcd_init(); //should happen before interrupts and timer settings, i think


    GIE = 1; //Enable interrupts

    //Initialize Timer0 - used for LCD refresh rate and long-term timebase
    OPTION_REG = 4; // 1:32 prescaler, giving XLCD 4.1ms for Cmd cycles
    TMR0IE = 1;
    TMR0 = 0;
}


void main(void) {
    unsigned char i;//, temp;
    unsigned int sen_long, sen_medm, sen_shrt;
    setup();

    //turn on
    i2c_start();
    i2c_tx(0b01110010);
    i2c_tx(0x00|0b10100000);
    i2c_tx(0b00000011);
    i2c_stop();
    
    //set timing
    i2c_start();
    i2c_tx(0b01110010);
    i2c_tx(0x01|0b10100000);
    i2c_tx(0b00000010);
    i2c_stop();
    //set gain
    i2c_start();
    i2c_tx(0b01110010);
    i2c_tx(0x07|0b10100000);
    i2c_tx(0b00100000);
    i2c_stop();

            
    while (1) {

        if (Lcd_Ready) {
            //clear LCD
            for (i=0; i < 32; i++)
                lcd_type_char(' ', 0, 0xff);
            
            i2c_start();
            i = i2c_tx(0b01110010);
            i2c_tx(0x10|0b10100000);
            i2c_start();
            i2c_tx(0b01110011);
            
            sen_medm = i2c_rx(1);
            sen_medm += 256 * i2c_rx(1);
            sen_long = i2c_rx(1);
            sen_long += 256 * i2c_rx(1);
            sen_shrt = i2c_rx(1);
            sen_shrt += 256 * i2c_rx(0);
            i2c_stop();

            
            lcd_write_int(i, 1, 0, 1);
            lcd_write_int(sen_long, 0, 0, 5);
            lcd_write_int(sen_medm, 0, 6, 5);
            lcd_write_int(sen_shrt, 1, 2, 5);
            
            /*lcd_type_char('H', 1, 3 + LED_PIN);
            lcd_type_char('e', 0, 0xff);
            lcd_type_char('l', 0, 0xff);
            lcd_type_char('l', 0, 0xff);
            lcd_type_char('o', 0, 0xff);
            lcd_type_char(' ', 0, 0xff);
            lcd_type_char('W', 0, 0xff);
            lcd_type_char('o', 0, 0xff);
            lcd_type_char('r', 0, 0xff);
            lcd_type_char('l', 0, 0xff);
            lcd_type_char('d', 0, 0xff);
            lcd_type_char('!', 0, 0xff);*/
            
            lcd_update(0, 0);//update from buffer
            Lcd_Ready = 0; // Unset LCD ready flag for delay

        }
    }
}




void lcd_write_int(unsigned int num, unsigned char row, unsigned char col, unsigned char num_digits)
// writes the ascii representation of a number to the LCD
{
    unsigned int digit, s;

    switch (num_digits) {
        case 4: goto four_digits;
        case 3: goto three_digits;
        case 2: goto two_digits;
        case 1: goto one_digit;
    }

//five_digits:
    digit = num / 10000;
    s = digit * 10000;
    num = num - s;
    lcd_type_char('0' + digit, row, col);
    col = 0xff; // next digit's position is auto handled by lcd_typ_char()
    
four_digits:
    digit = num / 1000;
    s = digit * 1000;
    num = num - s;
    lcd_type_char('0' + digit, row, col);
    col = 0xff;

three_digits:
    digit = num / 100;
    s = 100 * digit;
    num = num - s;
    lcd_type_char('0' + digit, row, col);
    col = 0xff;

two_digits:
    digit = num / 10;
    s = digit * 10;
    num = num - s;
    lcd_type_char('0' + digit, row, col);
    col = 0xff;

one_digit:
    lcd_type_char('0' + num, row, col);
}

void lcd_type_char(unsigned char char_buf, unsigned char row, unsigned char col)
{
    static unsigned char last_row;
    static unsigned char last_col;
    
    if (col > 15) {     // don't use absolute pos
        row = last_row;
        col = last_col + 1;
        if (col > 15) { // column overflow
            col = 0;
            row = !row;
        }
    }
    
    Lcd_Buf[row][col] = char_buf;
    
    last_row = row;
    last_col = col;
}

