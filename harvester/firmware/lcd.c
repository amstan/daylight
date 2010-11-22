
#include "lcd.h"

// Character Buffer: Lcd_Buf[row][column]
unsigned char Lcd_Buf[1][16];

/*
Updates the LCD controller with current data from the LCD buffer. One 
character or command is updated per call. Allow time in-between calls for 
the LCD controller to process commands.
*/
void lcd_update(unsigned char buf, unsigned char rs)
{
    static unsigned char column;
    static unsigned char row;
    
    unsigned char mask; // 8-bit-mask

    // determine the appropriate value for the XLCD shift register
    if (buf == 0) {  // allows for external XLCD commands
        if (column > 16) { // We have reached the end of the current row
            if (row == 1) {
                // Set cursor to start of 1st row
                buf = SET_DDRAM_ADDR|0;
                rs = 0; // RS_PIN = 0 for instruction
                row = 0;
            } else {
                // Set cursor to start of 2nd row
                buf = SET_DDRAM_ADDR|40;
                rs = 0;
                row = 1;
            }
            column = 0;
        } else {
            buf = Lcd_Buf[row][column];
            if (buf == '\0') // Remove NULL terminating character
                buf = ' ';
            rs = 1; // RS_PIN = 1 for data
            column++;
        }
    }

    // Update the shift registers
    LCD_Enable_PIN = 0;

    for (mask = 0b10000000; mask > 0; mask >>= 1) {
        LCD_Clock_PIN = 0;
        if (buf & mask) {
            LCD_DataRS_PIN = 1;
        } else {
            LCD_DataRS_PIN = 0;
        }
        // Shift data on the rising edge of clock
        short_delay();
        LCD_Clock_PIN = 1;
    }

    // LCD_DataRS_PIN is mulitplexed to the XLCD's RS pin
    LCD_DataRS_PIN = rs;

    // Clock the command in
    short_delay();
    LCD_Enable_PIN = 1;
    short_delay();
    LCD_Enable_PIN = 0;

    short_delay();
    short_delay();
    short_delay();
    short_delay();
    short_delay();

    return;
}

void short_delay(void)
{
    _asm
    nop
    nop
    _endasm;
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
Initializes the LCD controller for 8-bit mode and clears the display. Based 
on the Hitachi HD44780 LCD controller.
*/
void lcd_init(void)
{
    // All control signals made low
    LCD_DataRS_PIN = 0;
    LCD_Clock_PIN = 0;
    LCD_Enable_PIN = 0;

    // Delay for 15ms to allow for XLCD Power on reset
    long_delay();
    long_delay();
    long_delay();

    // Setup interface to XLCD
    lcd_write_cmd (0b00110000);     // Function set cmd(8-bit interface)
        
    // Delay for at least 4.1ms
    // done in lcd_write_cmd()

    // Setup interface to XLCD
    lcd_write_cmd (0b00110000);     // Function set cmd(8-bit interface)

    // Delay for at least 100us
    // done in lcd_write_cmd();

    // Setup interface to XLCD
    lcd_write_cmd (0b00110000);     // Function set cmd(8-bit interface)

    // Set data interface width, # rows, font
    lcd_write_cmd(FUNCTION_SET);    // Function set cmd

    // Turn the display on then off
    lcd_write_cmd(DOFF&CURSOR_OFF&BLINK_OFF); // Display OFF/Blink OFF
    lcd_write_cmd(DON&CURSOR_OFF&BLINK_OFF);  // Display ON/Blink OFF

    // Clear display
    lcd_write_cmd(0x01);

    // Set entry mode inc, no shift
    lcd_write_cmd(SHIFT_CUR_LEFT);

    // Set DD Ram address to 0
    lcd_write_cmd(SET_DDRAM_ADDR|0);

    return;
}

/*
Writes a command to the LCD controller. Built-in delay to allow time in-between 
successive calls for the LCD controller to process commands.
*/
void lcd_write_cmd(unsigned char cmd)
{
    lcd_update(cmd, 0);
    long_delay();

    return;
}
