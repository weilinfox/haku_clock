#include <reg52.h>
#include <intrins.h>

#include "types.h"
#include "ds3231.h"

void ds_delay (void)
{
	_nop_(); _nop_(); _nop_(); _nop_();
}

void ds_iic_start (void)
{
	CLKSDA = 1;
	CLKSCL = 1;
	ds_delay();
	CLKSDA = 0;
	ds_delay();
	CLKSCL = 0;
	ds_delay();
}

void ds_iic_stop (void)
{
	CLKSDA = 0;
	CLKSCL = 1;
	ds_delay();
	CLKSDA = 1;
	ds_delay();
}

void ds_iic_write (u8 dat)
{
	u8 i;
	for (i = 8; i; i--) {
		CLKSDA = dat & 0x80;
		ds_delay();
		CLKSCL = 1;
		ds_delay();
		CLKSCL = 0;
		ds_delay();
		dat <<= 1;
	}
}

u8 ds_iic_read (void)
{
	u8 i, dat = 0;
	for (i = 8; i; i--) {
		CLKSCL = 1;
		ds_delay();
		dat <<= 1;
		if (CLKSDA)
			dat |= 0x01;
		CLKSCL = 0;
		ds_delay();
		
	}
	return dat;
}

void ds_iic_write_ack (bit ack)
{
	CLKSDA = ~ack;
	ds_delay();
	CLKSCL = 1;
	ds_delay();
	CLKSCL = 0;
	ds_delay();
}

bit ds_iic_read_ack (void)
{
	bit ack;
	//CLKSDA = 1;
	CLKSCL = 1;
	ds_delay();
	ack = CLKSDA;
	ds_delay();
	CLKSCL = 0;
	ds_delay();
	return ack;
}

u8 ds_read_byte (u8 dev, u8 addr)
{
	u8 dat;
	ds_iic_start();
	ds_iic_write(dev);
	ds_iic_read_ack();
	ds_iic_write(addr);
	ds_iic_read_ack();
	ds_iic_start();
	ds_iic_write(dev | 0x01);
	ds_iic_read_ack();
	dat = ds_iic_read();
	ds_iic_write_ack(0);
	ds_iic_stop();
	return dat;
}

void ds_write_byte (u8 dat, u8 dev, u8 addr)
{
	ds_iic_start();
	ds_iic_write(dev);
	ds_iic_read_ack();
	ds_iic_write(addr);
	ds_iic_read_ack();
	ds_iic_write(dat);
	ds_iic_read_ack();
	ds_iic_stop();
}

void ds_init (void)
{
	CLKSDA = 1;
	CLKSCL = 1;
	
	ds_write_byte(0x00, CLK_DEV_ADDR, CLK_ADDR_CONTROL);
}

void ds_write_time (struct time * tm)
{
	u8 dat;
	dat = ((tm->time.second/10)<<4) | ((tm->time.second%10)&0x0f);
	ds_write_byte(dat, CLK_DEV_ADDR, CLK_ADDR_SEC);
	dat = ((tm->time.minute/10)<<4) | ((tm->time.minute%10)&0x0f);
	ds_write_byte(dat, CLK_DEV_ADDR, CLK_ADDR_MIN);
	if (tm->time.as24) {
		if (tm->time.hour > 19) dat = 0x20;
		else if (tm->time.hour > 9) dat = 0x10;
		else dat = 0x00;
		dat |= tm->time.hour % 10;
	} else {
		dat = 0x40;
		if (~tm->time.am) dat |= 0x20;
		if (tm->time.hour > 9) dat |= 0x10;
		dat |= tm->time.hour % 10;
	}
	ds_write_byte(dat, CLK_DEV_ADDR, CLK_ADDR_HOUR);
	ds_write_byte(tm->day, CLK_DEV_ADDR, CLK_ADDR_DAY);
	if (tm->year == 2100) dat = 0xf0;
	else dat = 0x00;
	dat |= (tm->month/10)<<4;
	dat |= tm->month % 10;
	ds_write_byte(dat, CLK_DEV_ADDR, CLK_ADDR_MON);
	dat = ((tm->year%100)/10) << 4;
	dat |= tm->year % 10;
	ds_write_byte(dat, CLK_DEV_ADDR, CLK_ADDR_YEAR);
	dat = ((tm->date/10)<<4) | ((tm->date%10)&0x0f);
	ds_write_byte(dat, CLK_DEV_ADDR, CLK_ADDR_DATE);
}

void ds_write_alarm (struct alarms * ala)
{
	u8 dat;
	dat = ((ala->alarm1.second/10)<<4) | ((ala->alarm1.second%10)&0x0f);
	ds_write_byte(dat, CLK_DEV_ADDR, CLK_ADDR_ALARM1_SEC);
	dat = ((ala->alarm1.minute/10)<<4) | ((ala->alarm1.minute%10)&0x0f);
	ds_write_byte(dat, CLK_DEV_ADDR, CLK_ADDR_ALARM1_MIN);
	if (ala->alarm1.hour > 19) dat = 0x20;
	else if (ala->alarm1.hour > 9) dat = 0x10;
	else dat = 0x00;
	dat |= ala->alarm1.hour % 10;
	ds_write_byte(dat, CLK_DEV_ADDR, CLK_ADDR_ALARM1_HOUR);
	
	dat = ((ala->alarm2.minute/10)<<4) | ((ala->alarm2.minute%10)&0x0f);
	ds_write_byte(dat, CLK_DEV_ADDR, CLK_ADDR_ALARM2_MIN);
	if (ala->alarm2.hour > 19) dat = 0x20;
	else if (ala->alarm2.hour > 9) dat = 0x10;
	else dat = 0x00;
	dat |= ala->alarm2.hour % 10;
	ds_write_byte(dat, CLK_DEV_ADDR, CLK_ADDR_ALARM2_HOUR);
}

void ds_read_data (struct time * tm, struct alarms * ala)
{
	u8 dat;
	
	if (tm->edit || ala->edit)
		return;

	/* read time */
	dat = ds_read_byte(CLK_DEV_ADDR, CLK_ADDR_SEC);
	tm->time.second = (dat>>4)*10 + (dat&0x0f);
	dat = ds_read_byte(CLK_DEV_ADDR, CLK_ADDR_MIN);
	tm->time.minute = (dat>>4)*10 + (dat&0x0f);
	dat = ds_read_byte(CLK_DEV_ADDR, CLK_ADDR_HOUR);
	if (dat & 0x40) {
		tm->time.as24 = 0;
		if (dat & 0x20) tm->time.am = 0;
		else tm->time.am = 1;
		if (dat & 0x10) tm->time.hour = 10;
		else tm->time.hour = 0;
		tm->time.hour += dat & 0x0f;
	} else {
		tm->time.as24 = 1;
		switch (dat & 0x30) {
			case 0x10:
				tm->time.hour = 10;
				break;
			case 0x20:
				tm->time.hour = 20;
				break;
			default:
				tm->time.hour = 0;
		}
		tm->time.hour += dat & 0x0f;
	}
	dat = ds_read_byte(CLK_DEV_ADDR, CLK_ADDR_DAY);
	tm->day = dat;
	dat = ds_read_byte(CLK_DEV_ADDR, CLK_ADDR_DATE);
	tm->date = (dat>>4)*10 + (dat&0x0f);
	dat = ds_read_byte(CLK_DEV_ADDR, CLK_ADDR_MON);
	if (dat & 0x10) tm->month = 10;
	else tm->month = 0;
	tm->month += dat & 0x0f;
	if (dat & 0x80) tm->year = 100;
	else tm->year = 0;
	dat = ds_read_byte(CLK_DEV_ADDR, CLK_ADDR_YEAR);
	tm->year += 2000 + (dat>>4)*10 + (dat&0x0f);
	
	/* read alarms */
	dat = ds_read_byte(CLK_DEV_ADDR, CLK_ADDR_ALARM1_SEC);
	ala->alarm1.second = (dat>>4)*10 + (dat&0x0f);
	dat = ds_read_byte(CLK_DEV_ADDR, CLK_ADDR_ALARM1_MIN);
	ala->alarm1.minute = (dat>>4)*10 + (dat&0x0f);
	dat = ds_read_byte(CLK_DEV_ADDR, CLK_ADDR_ALARM1_HOUR);
	ala->alarm1.as24 = 1;
	switch (dat & 0x30) {
		case 0x10:
			ala->alarm1.hour = 10;
			break;
		case 0x20:
			ala->alarm1.hour = 20;
			break;
		default:
			ala->alarm1.hour = 0;
	}
	ala->alarm1.hour += dat & 0x0f;

	//dat = ds_read_byte(CLK_DEV_ADDR, CLK_ADDR_ALARM2_SEC);
	ala->alarm2.second = 0;
	dat = ds_read_byte(CLK_DEV_ADDR, CLK_ADDR_ALARM2_MIN);
	ala->alarm2.minute = (dat>>4)*10 + (dat&0x0f);
	dat = ds_read_byte(CLK_DEV_ADDR, CLK_ADDR_ALARM2_HOUR);
	ala->alarm2.as24 = 1;
	switch (dat & 0x30) {
		case 0x10:
			ala->alarm2.hour = 10;
			break;
		case 0x20:
			ala->alarm2.hour = 20;
			break;
		default:
			ala->alarm2.hour = 0;
	}
	ala->alarm2.hour += dat & 0x0f;
}
