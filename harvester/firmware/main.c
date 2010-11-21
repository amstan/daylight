/* main.c

2010-11-20: got LCD working! the culprits: 1. PORTB defaults to analog output
    2. 'char' defaults to signed in SDCC!
    lesson learned: explicitly specify types for portability!
*/

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


#define LED_PIN RB4

#define nop() \
        _asm\
        NOP\
        _endasm

//LCD specific defines
#define LCD_SER_PIN     RB7
#define LCD_SCK_PIN     RB6
#define LCD_RCK_PIN     RB3

// Function set command
#define FUNCTION_SET    0b00111011  // 8-bit interface, 5x8 characters, multiple lines

// Display ON/OFF Control defines
#define DON         0b00001111  /* Display on      */
#define DOFF        0b00001011  /* Display off     */
#define CURSOR_ON   0b00001111  /* Cursor on       */
#define CURSOR_OFF  0b00001101  /* Cursor off      */
#define BLINK_ON    0b00001111  /* Cursor Blink    */
#define BLINK_OFF   0b00001110  /* Cursor No Blink */

// Cursor or Display Shift defines
#define SHIFT_CUR_LEFT    0b00010011  /* Cursor shifts to the left   */
#define SHIFT_CUR_RIGHT   0b00010111  /* Cursor shifts to the right  */
#define SHIFT_DISP_LEFT   0b00011011  /* Display shifts to the left  */
#define SHIFT_DISP_RIGHT  0b00011111  /* Display shifts to the right */

//LCD variables
// Buffer of characters for the LCD: Lcd_Buf{row}[{column}]
unsigned char Lcd_Buf0[16];
unsigned char Lcd_Buf1[16];

//will not be part of library:
unsigned char Update_Lcd_Buf;

unsigned char Lcd_Ready, Lcd_Rs_Buf;
unsigned char Lcd_Cmd_Buf;

//LCD functions
void lcd_update(void);
void short_delay(void);
void long_delay(void);
void lcd_init(void);
void lcd_write_cmd(unsigned char);
void lcd_set_ddram_addr(unsigned char);
void lcd_set_cgram_addr(unsigned char);

void lcd_type_char(unsigned char, unsigned char, unsigned char);

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
    TRISA=0b11111111;
    PORTA=0b00000000;
    TRISB=0b00000000;
    PORTB=0b00000000;

    ANSEL=0;//This is needed! PORTA defaults to analog input!

    lcd_init(); //should happen before interrupts and timer settings, i think


    GIE = 1; //Enable interrupts

    //Initialize Timer0 - used for LCD refresh rate and long-term timebase
    OPTION_REG = 4; // 1:32 prescaler, giving XLCD 4.1ms for Cmd cycles
    TMR0IE = 1;
    TMR0 = 0;
}


void main(void) {
    setup();
    
    LCD_SER_PIN = 0;
    while (1) {
        if (Lcd_Ready) {
            lcd_type_char('0' + 4, 1, 1);
            lcd_type_char('0' + 5, 0, 0xff);
            lcd_type_char('0' + 6, 0, 0xff);
            lcd_type_char('0' + 7, 0, 0xff);
            lcd_update();
        }
    }
}



/*
 *  Updates the LCD controller with current data from the LCD buffer. One 
 *  character or command is updated per call. Allow time in-between calls for 
 *  the LCD controller to process commands.
 */
void lcd_update(void)
{
    static unsigned char column;
    static unsigned char row;
    
    unsigned char mask; // 8-bit-mask

    // determine the appropriate value for the XLCD shift register
    if (!Lcd_Cmd_Buf) {  // allows for external XLCD commands
        if (column > 16) { // We have reached the end of the current row
            if (row == 1) {
                lcd_set_ddram_addr(0); // Set cursor to start of 1st row
                row = 0;
            } else {
                lcd_set_ddram_addr(40); // Set cursor to start of 2nd row
                row = 1;
            }
            column = 0;
        } else {
            if (row == 0) {
                Lcd_Cmd_Buf = Lcd_Buf0[column];
            } else {
                Lcd_Cmd_Buf = Lcd_Buf1[column];
            }
            if (Lcd_Cmd_Buf == '\0') // Remove NULL terminating character
                Lcd_Cmd_Buf = ' ';
            Lcd_Rs_Buf = 1; // RS_PIN = 1 for sending data
            column++;
        }
    }

    // Update the shift registers
    LCD_RCK_PIN = 0;

    for (mask = 0b10000000; mask > 0; mask >>= 1) {
        LCD_SCK_PIN = 0;
        if (Lcd_Cmd_Buf & mask) {
            LCD_SER_PIN = 1;
        } else {
            LCD_SER_PIN = 0;
        }
        nop();
        LCD_SCK_PIN = 1;            // Shift data on the rising edge of clock
    }

    LCD_SER_PIN = Lcd_Rs_Buf;    // LCD_SER_PIN is mulitplexed to the XLCD's RS pin

    short_delay();
    LCD_RCK_PIN = 1;                // Clock the command in
    short_delay();
    LCD_RCK_PIN = 0;

    short_delay();
    short_delay();
    short_delay();
    short_delay();
    short_delay();

    Lcd_Cmd_Buf = 0; // Empty XLCD cmd buffer, ready for more
    Lcd_Ready = 0; // Unset XLCD ready flag for delay

    return;
}

void short_delay(void)
{
    nop();
    nop();
}

void long_delay(void)
{
    unsigned char millisec = 5;
    unsigned char next = 0;

    OPTION_REG = 2; // prescaler divide TMR0 rate by 8
    TMR0 = 2;  // deduct 2*8 fixed instruction cycles delay
    do  {
        next += 125;
//        clrwdt();  // needed only if watchdog is enabled
        while (TMR0 != next)   // 125 * 8 = 1000 (= 1 ms)
            ;
    } while (-- millisec != 0);

    return;
}

/*
 *  Initializes the LCD controller for 8-bit mode and clears the display. Based
 *  on the Hitachi HD44780 LCD controller.
 */
void lcd_init(void)
{
    // All control signals made low
    LCD_SER_PIN = 0;
    LCD_SCK_PIN = 0;
    LCD_RCK_PIN = 0;

    // Delay for 15ms to allow for XLCD Power on reset
    long_delay();
    long_delay();
    long_delay();

    // Setup interface to XLCD
    lcd_write_cmd (0b00110000);      // Function set cmd(8-bit interface)
        
    // Delay for at least 4.1ms
    // done in lcd_write_cmd()

    // Setup interface to XLCD
    lcd_write_cmd (0b00110000);      // Function set cmd(8-bit interface)

    // Delay for at least 100us
    // done in lcd_write_cmd();

    // Setup interface to XLCD
    lcd_write_cmd (0b00110000);      // Function set cmd(8-bit interface)

    // Set data interface width, # rows, font
    lcd_write_cmd(FUNCTION_SET);         // Function set cmd

    // Turn the display on then off
    lcd_write_cmd(DOFF&CURSOR_OFF&BLINK_OFF);        // Display OFF/Blink OFF
    lcd_write_cmd(DON&CURSOR_OFF&BLINK_OFF);           // Display ON/Blink OFF

    // Clear display
    lcd_write_cmd(0x01);             // Clear display

    // Set entry mode inc, no shift
    lcd_write_cmd(SHIFT_CUR_LEFT);   // Entry Mode

    // Set DD Ram address to 0
    lcd_set_ddram_addr(0);                // Set Display data ram address to 0

    return;
}

/*
 *  Writes a command to the LCD controller. Allow time in-between calls for the
 *  LCD controller to process commands.
 */
void lcd_write_cmd(unsigned char cmd)
{
    Lcd_Cmd_Buf = cmd;
    Lcd_Rs_Buf = 0;
    lcd_update();
    long_delay();

    return;
}

/*
 *  Sets the character generator address of the LCD controller. Allow time 
 *  in-between calls for the LCD controller to process commands.
 */
void lcd_set_cgram_addr(unsigned char addr)
{
    Lcd_Cmd_Buf = addr | 0b01000000;
    Lcd_Rs_Buf = 0;

    return;
}

/*
 *  Sets the display data address of the LCD controller. Allow time in-between 
 *  calls for the LCD controller to process commands.
 */

void lcd_set_ddram_addr(unsigned char addr)
{
    Lcd_Cmd_Buf = addr | 0b10000000;
    Lcd_Rs_Buf = 0;

    return;
}
/*
void lcd_write_int(unsigned long num, bit row, unsigned col, unsigned num_digits)
// writes the ascii representation of a number to the LCD
{
    unsigned long digit, s;

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
}*/

void lcd_type_char(unsigned char char_data, unsigned char row, unsigned char col)
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
    
    if (!row)
        Lcd_Buf0[col] = char_data;
    else
        Lcd_Buf1[col] = char_data;
    
    last_row = row;
    last_col = col;
}

