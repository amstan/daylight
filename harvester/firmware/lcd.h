#ifndef LCD_H
#define LCD_H

#include "pic/pic16f88.h"

// Output pin defines
// Note: ensure the appropriate TRIS bits are cleared
#define LCD_DataRS_PIN RB7
#define LCD_Clock_PIN  RB6
#define LCD_Enable_PIN RB3

// Function set command
#define FUNCTION_SET 0b00111011 //8-bit interface, 5x8 characters, multiple lines

// Display ON/OFF Control defines
#define DON        0b00001111 // Display on
#define DOFF       0b00001011 // Display off
#define CURSOR_ON  0b00001111 // Cursor on
#define CURSOR_OFF 0b00001101 // Cursor off
#define BLINK_ON   0b00001111 // Cursor Blink
#define BLINK_OFF  0b00001110 // Cursor No Blink

// Cursor or Display Shift defines
#define SHIFT_CUR_LEFT   0b00010011 // Cursor shifts to the left
#define SHIFT_CUR_RIGHT  0b00010111 // Cursor shifts to the right
#define SHIFT_DISP_LEFT  0b00011011 // Display shifts to the left
#define SHIFT_DISP_RIGHT 0b00011111 // Display shifts to the right

// Misc Commands
#define SET_CGRAM_ADDR 0b01000000 //Set the Character Generator Address
#define SET_DDRAM_ADDR 0b10000000 //Set the Display Data Address

// Character Buffer: Lcd_Buf[row][column]
extern unsigned char Lcd_Buf[1][16];

// LCD functions
void lcd_update(unsigned char, unsigned char);
void short_delay(void);
void long_delay(void);
void lcd_init(void);
void lcd_write_cmd(unsigned char);

#endif