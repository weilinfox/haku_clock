#ifndef __TYPES_H
#define __TYPES_H

typedef unsigned char	u8;
typedef signed char		s8;
typedef unsigned int	u16;
typedef signed int 		s16;

struct clock {
	u8 hour, minute, second;
	u8 as24;		/* as24 ? show as 24hour : 12 hour with AM/PM */
	u8 am;		/* if !as24 then am ? show AM : show PM */
};

struct time {
	u16 year;
	u8 month, date;
	u8 day;		/* day in a week */
	struct clock time;
	u8 edit;		/* edit ? show cursor : hide cousor */
	u8 editpos;
};

struct alarms {
	struct clock alarm1, alarm2;
	u8 on1, on2;	/* alarm1/2 is on/off */
	u8 edit;		/* edit ? show cursor : hide cousor */
	u8 editpos;
};

struct envdata {
	s16 temp;
	u16 humidity;
	u16 pm1_0, pm2_5, pm10;
	u16 voc;
};

#endif
