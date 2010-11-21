
#include "lcd.h"

//LCD variables
// Buffer of characters for the LCD: Lcd_Buf{row}[{column}]
unsigned char Lcd_Buf0[16];
unsigned char Lcd_Buf1[16];

unsigned char Lcd_Rs_Buf;
unsigned char Lcd_Cmd_Buf;

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
        _asm
        nop
        _endasm;
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
