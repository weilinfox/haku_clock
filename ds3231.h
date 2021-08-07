#ifndef __DS3231_H
#define __DS3231_H

/* ds3231 I2C interface */
sbit CLKSCL =	P0^6;
sbit CLKSDA =	P0^7;

#define CLK_DEV_ADDR	0xd0
#define CLK_DEV_READ	0xd1
#define CLK_ADDR_TIME	0x00
#define CLK_ADDR_ALARM1	0x07
#define CLK_ADDR_ALARM2	0x0b
#define CLK_ADDR_SEC	0x00
#define CLK_ADDR_MIN	0x01
#define CLK_ADDR_HOUR	0x02
#define CLK_ADDR_DAY	0x03
#define CLK_ADDR_DATE	0x04
#define CLK_ADDR_MON	0x05
#define CLK_ADDR_YEAR	0x06
#define CLK_ADDR_ALARM1_SEC	0x07
#define CLK_ADDR_ALARM1_MIN	0x08
#define CLK_ADDR_ALARM1_HOUR	0x09
#define CLK_ADDR_ALARM1_DATE	0x0a
#define CLK_ADDR_ALARM2_MIN	0x0b
#define CLK_ADDR_ALARM2_HOUR	0x0c
#define CLK_ADDR_ALARM2_DATE	0x0d
#define CLK_ADDR_CONTROL	0x0e
#define CLK_ADDR_STATUS	0x0f
#define CLK_ADDR_AGING_OFFSET	0x10
#define CLK_ADDR_TEMP_MSB	0x11
#define CLK_ADDR_TEMP_LSB	0x12

extern void ds_init (void);
extern void ds_read_data(struct time *, struct alarms *);
extern void ds_write_time (struct time *);
extern void ds_write_alarm (struct alarms *);

#endif
