#ifndef __LCD1602_H
#define __LCD1602_H

#include "types.h"

/* use low 4 bits of LCDD */
#define LCDDATA	P0
/* rs and w/r */
sbit LCDRW =	P0^4;
sbit LCDRS =	P0^5;
sbit LCDBUSY =	P0^3;
/* #define LCDLIGHT	P2^1 */
/* lcd cs */
sbit LCDEN =	P2^0;
/* debug light */
sbit DEBUG =	P2^4;
/* lcd light */
sbit LIGHT =	P2^3;

/* commands */
/* #define LCD_COM_flag	0x00	command rs rw flag */
#define LCD_COM_Rbusy	0x10	/* read busy */
#define LCD_COM_Clear	0x01	/* clear screen */
#define LCD_COM_Rhome	0x02	/* set cursor return home */
#define LCD_COM_Setfunc	0x28	/* set 4 bits data and dual line */
				/* function must be set before display */
#define LCD_COM_Setmode	0x06	/* AC ++ */
#define LCD_COM_HIDE	0x08	/* turn off screen */
#define LCD_COM_Cursoron	0x0e	/* show cursor */
#define LCD_COM_Cursoroff	0x0c	/* hide cursor */
#define LCD_COM_Cursorflash	0x0f	/* show flashing cursor */
#define LCD_COM_Cursorleft	0x10	/* cursor move next and AC++ */
#define LCD_COM_Cursorright	0x14	/* cursor move forward and AC-- */
#define LCD_COM_Line1	0x80	/* set cursor to line1 */
#define LCD_COM_Line2	0xc0	/* set cursor to line2 */

/* data flag */
#define LCD_DATA_Flag	0x20	/* & flag with your data */

/* address */
#define LCD_ADDR_Line1(n)	(0x01+(n))
#define LCD_ADDR_Line2(n)	(0x41+(n))

/* editpos define */
#define EDIT_ALARM_MAX	12
#define EDIT_TIME_MAX	13

extern void lcd_init (void);
extern void lcd_show_clock (struct time *);
extern void lcd_show_alarm (struct alarms *);
extern void lcd_show_env(struct envdata *);
extern void lcd_show_start (void);
extern void lcd_light_on (void);
extern void lcd_light_off (void);

#endif
