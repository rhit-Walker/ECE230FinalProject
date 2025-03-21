//File Name: lcd4bits_ece230w25template.h
//Author: Jianjian Song
//Date: January 29, 2025
//ECE230 Winter 2024-2025

#define LCDCMD_ClearDisplay     0x01    //clear display: clear, move cursor home
#define LCDCMD_EMS              0x06    //entry mode set: auto increment cursor after each char sent
#define LCDCMD_DisplaySettings  0x0C    //display ON/OFF control: display on, cursor off, blink off
#define LCDCMD_FunctionSet      0x38    //function set: 8-bit mode, 2 lines, 5x7 dots
#define LCDCMD_FunctionSet_4bits 0x28
#define FirstLine 0x00
#define SecondLine 0x40
#define CMD_MODE        0           //0 for command mode
#define DATA_MODE       1           //1 for data mode

extern void Display(unsigned char , unsigned int );

/* delay for indicated nr of ms */
extern void DelayMs(unsigned int);

/* write a byte to the LCD in 8 bit mode */
extern void lcd8bits_write(unsigned char mode, unsigned char CmdChar);
extern void lcd4bits_write(unsigned char mode, unsigned char CmdChar);

/* Clear and home the LCD */
extern void lcd_clear(void);

/* write a string of characters to the LCD */
extern void lcd_puts(char * s);

/* Go to the specified position */
extern void lcd_SetLineNumber(unsigned char pos);

/* intialize the LCD - call before anything else */
extern void lcd4bits_init(void);

extern void lcd_putch(char);

/*  Set the cursor position */
#define lcd_cursor(x)   lcd_write(((x)&0x7F)|0x80)
