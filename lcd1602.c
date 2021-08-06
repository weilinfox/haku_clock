#include <reg52.h>

#include "types.h"
#include "lcd1602.h"

/* edit frame address table */
u8 code LCD_CLK_EDIT_POS[] =	{0x83, 0x84, 0x85, 0x86,	/* yyyy */
			0x88, 0x89,		/* mm */
			0x8a, 0x8b,		/* dd */
			0xc4, 0xc5,		/* hh */
			0xc7, 0xc8,		/* mm */
			0xc9, 0xca 		/* ss */ };
u8 code LCD_CLK_EDIT_POS2[] =	{0x83, 0x84, 0x85, 0x86,	/* yyyy */
			0x88, 0x89,		/* mm */
			0x8a, 0x8b,		/* dd */
			0xc2, 0xc3,		/* hh */
			0xc5, 0xc6,		/* mm */
			0xc8, 0xc9 		/* ss */ };
u8 code LCD_ALAM_EDIT_POS[] =	{0x88, 0x89, 0x8b, 0x8c, 0x8e, 0x8f,
			0xc8, 0xc9, 0xcb, 0xcc, 0xce, 0xcf};
u8 code LCD_NUM_CHAR[] = "0123456789";

void lcd_delay200us (u8 t)
{
	u8 i;
	for (; t; t--)
		for (i = 100; i; i--) ;
}

void lcd_debug_delay(void)
{
	u8 i;
	DEBUG = 0;	/* light on */
	for (i=10; i; i--)
		lcd_delay200us(255);
	DEBUG = 1;	/* light off */
	for (i=10; i; i--)
		lcd_delay200us(255);
}

void lcd_busy_wait (void)
{
	LCDEN = 0;
	LCDDATA &= 0xc0;
	LCDDATA |= LCD_COM_Rbusy;
	LCDEN = 1;
	lcd_delay200us(10);
	while (LCDBUSY) {DEBUG=1; DEBUG=0;}
	LCDEN = 0;
}

void lcd_send_string (u8 *str, u8 len)
{
	u8 i;
	for (i = 0; i < len; i++) {
		LCD_SEND_DATA(str[i]);
	}
}

void lcd_send_int (u16 d, u16 flag)
{
	while (flag) {
		LCD_SEND_DATA(LCD_NUM_CHAR[d/flag]);
		d %= flag;
		flag /= 10;
	}
}

void lcd_init (void)
{
	/* the highest 2 bits are not used */
/*	LCDDATA &= 0xc0;
	lcd_delay200us(80);
	LCDDATA |= 0x03;
	LCDEN = 1;
	LCDEN = 0;
	lcd_delay200us(25);
	LCDDATA &= 0xc0;
	LCDDATA |= 0x03;
	LCDEN = 1;
	LCDEN = 0;
	lcd_delay200us(1);
	LCDDATA &= 0xc0;
	LCDDATA |= 0x03;
	LCDEN = 1;
	LCDEN = 0;
	lcd_delay200us(1);
	LCDDATA &= 0xc0;
	LCDDATA |= 0x02;
	LCDEN = 1;
	LCDEN = 0;
	lcd_delay200us(1);*/

	LCD_INIT_COM(LCD_COM_Setfunc, 1);

	LCD_INIT_COM(LCD_COM_Setfunc, 1);
	LCD_INIT_COM(LCD_COM_HIDE, 1);
	LCD_INIT_COM(LCD_COM_Clear, 10);
	LCD_INIT_COM(LCD_COM_Setmode, 1);
	LCD_SEND_COM(LCD_COM_Cursoroff);
}

/* ___yyyy/mm/dd___ */
/* ____xx:xx:xx____ */
/* __xx:xx:xx__AM__ */
void lcd_show_clock (struct time * tm)
{
	LCD_SEND_COM(LCD_COM_Line1);

	lcd_send_string("   ", 3);
	lcd_send_int(tm->year, 1000);
	lcd_send_string("/", 1);
	lcd_send_int(tm->month, 10);
	lcd_send_string("/", 1);
	lcd_send_int(tm->date, 10);
	lcd_send_string("   ", 3);

	LCD_SEND_COM(LCD_COM_Line2);

	if (tm->time.as24) lcd_send_string("    ", 4);
	else lcd_send_string("  ", 2);
	lcd_send_int(tm->time.hour, 10);
	lcd_send_string(":", 1);
	lcd_send_int(tm->time.minute, 10);
	lcd_send_string(":", 1);
	lcd_send_int(tm->time.second, 10);
	if (tm->time.as24) lcd_send_string("    ", 4);
	else {
		lcd_send_string("  ", 2);
		if (tm->time.am) lcd_send_string("AM", 2);
		else lcd_send_string("PM", 2);
		lcd_send_string("  ", 2);
	}

	if (tm->edit) {
		if (tm->time.as24) {
			LCD_SEND_COM(LCD_CLK_EDIT_POS[tm->editpos]);
		} else {
			LCD_SEND_COM(LCD_CLK_EDIT_POS2[tm->editpos]);
		}
		LCD_SEND_COM(LCD_COM_Cursorflash);
	} else {
		LCD_SEND_COM(LCD_COM_Cursoroff);
	}
}

/* Alarm1: xx:xx:xx */
/* Alarm2: xx:xx:xx */
void lcd_show_alarm (struct alarms * ala)
{
	LCD_SEND_COM(LCD_COM_Line1);

	lcd_send_string("Alarm1: ", 8);
	lcd_send_int(ala->alarm1.hour, 10);
	lcd_send_string(":", 1);
	lcd_send_int(ala->alarm1.minute, 10);
	lcd_send_string(":", 1);
	lcd_send_int(ala->alarm1.second, 10);
	
	LCD_SEND_COM(LCD_COM_Line2);

	lcd_send_string("Alarm2: ", 8);
	lcd_send_int(ala->alarm2.hour, 10);
	lcd_send_string(":", 1);
	lcd_send_int(ala->alarm2.minute, 10);
	lcd_send_string(":", 1);
	lcd_send_int(ala->alarm2.second, 10);
	
	if (ala->edit) {
		LCD_SEND_COM(LCD_ALAM_EDIT_POS[ala->editpos]);
		LCD_SEND_COM(LCD_COM_Cursorflash);
	} else {
		LCD_SEND_COM(LCD_COM_Cursoroff);
	}
}

/* T:-xx.x_C RH:xx% */
/* PM2.5: xxx ug/m3 */
void lcd_show_env(struct envdata * env)
{
	LCD_SEND_COM(LCD_COM_Line1);

	lcd_send_string("T:", 2);
	if (env->temp < -99) {
		lcd_send_string("-", 1);
		lcd_send_int((-env->temp)/10, 10);
		lcd_send_string(".", 1);
		lcd_send_int((-env->temp)%10, 1);
	} else if (env->temp < 0) {
		lcd_send_string(" -", 2);
		lcd_send_int((-env->temp)/10, 1);
		lcd_send_string(".", 1);
		lcd_send_int((-env->temp)%10, 1);
	} else if (env->temp < 100) {
		lcd_send_string("  ", 2);
		lcd_send_int(env->temp/10, 1);
		lcd_send_string(".", 1);
		lcd_send_int(env->temp%10, 1);
	} else {
		lcd_send_string(" ", 1);
		lcd_send_int(env->temp/10, 10);
		lcd_send_string(".", 1);
		lcd_send_int(env->temp%10, 1);
	}
	lcd_send_string("\337C RH:", 6);
	if (env->humidity > 9) {
		lcd_send_string(" ", 1);
		lcd_send_int(env->humidity, 1);
	} else {
		lcd_send_int(env->humidity, 10);
	}
	lcd_send_string("%", 1);
	
	LCD_SEND_COM(LCD_COM_Line2);

	lcd_send_string("PM2.5: ", 7);
	if (env->pm2_5 > 99) {
		lcd_send_int(env->pm2_5, 100);
	} else if (env->pm2_5 > 9) {
		lcd_send_string(" ", 1);
		lcd_send_int(env->pm2_5, 10);
	} else {
		lcd_send_string("  ", 2);
		lcd_send_int(env->pm2_5, 1);
	}
	lcd_send_string(" \344g/m3", 6);
}
