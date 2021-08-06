#include <reg52.h>
#include <intrins.h>

#include "types.h"
#include "ds3231.h"

void ds_delay (void)
{
	_nop_(); _nop_(); _nop_(); _nop_();
}

void ds_init (void)
{
	CLKSDA = 1;
	CLKSCL = 1;
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

void ds_iic_write_ack (void)
{
	CLKSDA = 0;
	ds_delay();
	CLKSCL = 1;
	ds_delay();
	CLKSCL = 0;
	ds_delay();
}

void ds_iic_write_nack (void)
{
	CLKSDA = 1;
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
	
}

void ds_read_data(struct time * tm)
{
	u8 dat;

	ds_iic_start();
	ds_iic_write(CLK_DEV_ADDR);
	ds_iic_read_ack();		ds_delay();ds_delay();
	ds_iic_write(CLK_ADDR_TIME);
	ds_iic_read_ack();		ds_delay();ds_delay();
	ds_iic_start();
	ds_iic_write(CLK_DEV_READ);
	ds_iic_read_ack();		ds_delay();ds_delay();
	dat = ds_iic_read();
	ds_iic_write_ack();		ds_delay();ds_delay();
	tm->time.second = (dat>>4)*10 + (dat&0x0f);
	dat = ds_iic_read();
	ds_iic_write_ack();		ds_delay();ds_delay();
	tm->time.minute = (dat>>4)*10 + (dat&0x0f);
	dat = ds_iic_read();
	ds_iic_write_ack();		ds_delay();ds_delay();
	tm->time.hour = dat;
	/*if (dat & 0x40) {
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
	}*/
	dat = ds_iic_read();
	ds_iic_write_ack();		ds_delay();ds_delay();
	tm->day = dat;
	dat = ds_iic_read();
	ds_iic_write_ack();		ds_delay();ds_delay();
	tm->date = dat;
	dat = ds_iic_read();
	ds_iic_write_ack();		ds_delay();ds_delay();
	tm->month = dat;
	dat = ds_iic_read();
	ds_iic_write_nack();
	tm->year = dat;
	ds_iic_stop();
}
