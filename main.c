#include <STC89C5xRC.H>

#include "main.h"

u8 showalarm = 0;
u8 showenv = 0;
u8 code day_of_month[] =	{0, 31, 28, 31, 30, 31, 30, 31, 31,
			30, 31, 30, 31};

/*
1	2	3	Env
4	5	6	Alarm
7	8	9	Up
Back	0	Enter	Down
*/
u8 scan_keyboard (void)
{
	u8 i, j, k, l;
	for (i = 0; i < 4; i++) {
		PANEL = ~(0x01 << i);
		for(j=0;j<4;j++) if(~PANEL&(0x10<<j)) {
			for (l = 20; l; l--)
				for (k = 0xff; k; k--) ;
			if (~PANEL&(0x10<<j)) {
				while (~PANEL&(0x10<<j)) ;
				for (l = 20; l; l--)
					for (k = 0xff; k; k--) ;
				if (PANEL&(0x10<<j))
					return i*4+j;
			}
		}
	}
	return 0xff;
}

void update_time_pos (struct time * tm, bit right)
{
	if (right) {
		tm->editpos ++;
		if (tm->editpos >= EDIT_TIME_MAX)
			tm->editpos = 0;
	} else {
		if (tm->editpos == 0)
			tm->editpos = EDIT_TIME_MAX;
		tm->editpos --;
	}
	lcd_show_clock(tm);
}

void update_time_data (struct time * tm, u8 dat)
{
	if (tm->edit == 0) return;
	if (dat < 0) return;
	switch (tm->editpos) {
		case 0:	/* yy */
			tm->year = 2000 + tm->year%10 + dat*10;
			break;
		case 1:
			tm->year = tm->year/10*10 + dat;
			break;
		case 2:	/* mm */
			tm->month = tm->month%10 + dat*10;
			break;
		case 3:
			tm->month = tm->month/10*10 + dat;
			break;
		case 4:	/* dd */
			tm->date = tm->date%10 + dat*10;
			break;
		case 5:
			tm->date = tm->date/10*10 + dat;
			break;
		case 6:	/* week */
			if (dat > 7) return;
			tm->day = dat;
			break;
		case 7:	/* hh */
			tm->time.hour = tm->time.hour%10 + dat*10;
			break;
		case 8:
			tm->time.hour = tm->time.hour/10*10 + dat;
			break;
		case 9:	/* mm */
			tm->time.minute = tm->time.minute%10 + dat*10;
			break;
		case 10:
			tm->time.minute = tm->time.minute/10*10 + dat;
			break;
		case 11:	/* ss */
			tm->time.second = tm->time.second%10 + dat*10;
			break;
		case 12:
			tm->time.second = tm->time.second/10*10 + dat;
			break;
	}
	update_time_pos(tm, 1);
}

bit update_time_check (struct time * tm)
{
	u8 leap;
	/* time */
	if (tm->time.hour>24||tm->time.minute>59||tm->time.second>59)
		return 0;
	/* year and month */
	if (tm->year < 2000 || tm->year > 2100) return 0;
	if (tm->month < 1 || tm->month > 12) return 0;
	/* leap year and day */
	if (tm->month != 2) leap = 0;
	else if (tm->year % 400 == 0) leap = 1;
	else if (tm->year % 100 == 0) leap = 0;
	else if (tm->year % 4 == 0) leap = 1;
	else leap = 0;
	if (tm->date < 1||tm->date > day_of_month[tm->month]+leap)
		return 0;
	/* day of week */
	if (tm->day < 1 || tm->day > 7) return 0;

	return 1;
}

void update_alarm_pos (struct alarms * ala, bit right)
{
	if (right) {
		ala->editpos ++;
		if (ala->editpos >= EDIT_ALARM_MAX)
			ala->editpos = 0;
	} else {
		if (ala->editpos == 0)
			ala->editpos = EDIT_ALARM_MAX;
		ala->editpos --;
	}
	lcd_show_alarm(ala);
}

void update_alarm_data (struct alarms * ala, u8 dat)
{
	if (ala->edit == 0) return;
	if (dat < 0) return;
	switch (ala->editpos) {
		case 0:	/* hh */
			ala->alarm1.hour = ala->alarm1.hour%10 + dat*10;
			break;
		case 1:
			ala->alarm1.hour = ala->alarm1.hour/10*10 + dat;
			break;
		case 2:	/* mm */
			ala->alarm1.minute = ala->alarm1.minute%10 + dat*10;
			break;
		case 3:
			ala->alarm1.minute = ala->alarm1.minute/10*10 + dat;
			break;
		case 4:	/* ss */
			ala->alarm1.second = ala->alarm1.second%10 + dat*10;
			break;
		case 5:
			ala->alarm1.second = ala->alarm1.second/10*10 + dat;
			break;
		case 6:	/* hh */
			ala->alarm2.hour = ala->alarm2.hour%10 + dat*10;
			break;
		case 7:
			ala->alarm2.hour = ala->alarm2.hour/10*10 + dat;
			break;
		case 8:	/* mm */
			ala->alarm2.minute = ala->alarm2.minute%10 + dat*10;
			break;
		case 9:
			ala->alarm2.minute = ala->alarm2.minute/10*10 + dat;
			break;
		case 10:	/* ss */
			break;
		case 11:
			break;
	}
	update_alarm_pos(ala, 1);
}

bit update_alarm_check (struct alarms * ala)
{
	if (ala->alarm1.hour > 24 || ala->alarm2.hour > 24)
		return 0;
	if (ala->alarm1.minute > 59 || ala->alarm2.minute > 59)
		return 0;
	if (ala->alarm1.second > 59 || ala->alarm2.second > 59)
		return 0;
	return 1;
}

bit check_at_night (const struct time* tm)
{
	if (tm->time.hour >= 22 || tm->time.hour < 6)
		return 1;
	return 0;
}

void main()
{
	u8 code keyboard_num[16] =	{1, 2, 3, -1,
				4, 5, 6, -1,
				7, 8, 9, -1,
				-1, 0, -1, -1};
	struct time hakuTime = 	{2021, 8, 3, 0,
				{23, 59, 59, 1, 0},
				0, 0};
	struct alarms hakuAlarm =	{{0, 0, 0}, {0, 0, 0},
				0, 0, 0, 0};
	struct envdata hakuEnv =	{0, 0, 0, 0, 0, 0};
	bit edit;			/* in edit mode */
	u8 editpos;		/* editpos ? alarm : time */
	u8 keypress;		/* pressed key No. */
	u8 psec;			/* detect another second */
	u8 light;			/* lcd light */


	PCON = 0x00;
	WDT_CONTR = 0x33;

	lcd_init();
	ds_init();
	dht_init();
	pms_serial_init(&hakuEnv);
	clock_debug = 0;
	light = 1;

	lcd_show_start();
	/* delay around 1.5s */
	for (editpos = 0xff; editpos; editpos--)
		for (keypress = 0xff; keypress; keypress--)
			for (psec = 10; psec; psec--);

	editpos = edit = 0;

	ds_read_data(&hakuTime, &hakuAlarm);
	dht_read_data(&hakuEnv);
	psec = hakuTime.time.second;

	lcd_show_clock(&hakuTime);
	clock_debug = 1;
	//lcd_show_alarm(&hakuAlarm);
	//lcd_show_env(&hakuEnv);
	//while (1) lcd_show_env(&hakuEnv);
	while(1) {
		if (~edit) {
			ds_read_data(&hakuTime, &hakuAlarm);
			if (showalarm) {
				light = 1;
				lcd_show_alarm(&hakuAlarm);
			} else if (showenv) {
				light = 1;
				lcd_show_env(&hakuEnv);
			} else {
				if (check_at_night(&hakuTime))
					light = 0;
				else
					light = 1;
				editpos = 0;
				lcd_show_clock(&hakuTime);
			}
			/* alarm */
			if (hakuAlarm.alarm1.second == hakuTime.time.second &&
				hakuAlarm.alarm1.minute == hakuTime.time.minute &&
				hakuAlarm.alarm1.hour == hakuTime.time.hour) {
				alarm_out = 0;
			} else if (hakuAlarm.alarm2.second == hakuTime.time.second &&
				hakuAlarm.alarm2.minute == hakuTime.time.minute &&
				hakuAlarm.alarm2.hour == hakuTime.time.hour) {
				alarm_out = 0;
			} else {
				alarm_out = 1;
			}
			/* show delay */
			if (psec != hakuTime.time.second) {
				if (showalarm) showalarm --;
				if (showenv) showenv --;
				psec = hakuTime.time.second;
				dht_read_data(&hakuEnv);
			}
		} else {
			light = 1;
		}
		/* light control */
		if (light) lcd_light_on();
		else lcd_light_off();

		keypress = scan_keyboard();
		switch (keypress) {
			case 0: case 1: case 2:
			case 4: case 5: case 6:
			case 8: case 9: case 10:
			case 13:
				/* number key */
				if (edit) {
					if (editpos) {
						showalarm = SHOWDELAY;
						update_alarm_data(&hakuAlarm, keyboard_num[keypress]);
					} else
						update_time_data(&hakuTime, keyboard_num[keypress]);
				}
				break;
			case 11:
				/* editpos left */
				if (edit) {
					if (editpos) {
						showalarm = SHOWDELAY;
						update_alarm_pos(&hakuAlarm, 0);
					} else
						update_time_pos(&hakuTime, 0);

				}
				break;
			case 15:
				/* editpos right */
				if (edit) {
					if (editpos) {
						showalarm = SHOWDELAY;
						update_alarm_pos(&hakuAlarm, 1);
					} else
						update_time_pos(&hakuTime, 1);

				}
				break;
			case 3:
				/* show alarm */
				if (~edit) {
					showenv = 0;
					showalarm = SHOWDELAY;
					editpos = 1;
				}
				break;
			case 7:
				/* show env */
				if (~edit) {
					showalarm = 0;
					showenv = SHOWDELAY;
					editpos = -1;
				}
				break;
			case 12:
				/* back */
				edit = 0;
				editpos = 0;
				showalarm = 0;
				showenv = 0;
				hakuTime.edit = 0;
				hakuAlarm.edit = 0;
				break;
			case 14:
				/* enter */
				if (edit) {
					if (editpos) {
						if (update_alarm_check(&hakuAlarm))
							ds_write_alarm (&hakuAlarm);
						else {
							showalarm = SHOWDELAY;
							break;
						}
					} else {
						if (update_time_check(&hakuTime))
							ds_write_time (&hakuTime);
						else
							break;
					}
					editpos = 0;
					edit = 0;
					showalarm = 0;
					showenv = 0;
					hakuTime.edit = 0;
					hakuAlarm.edit = 0;
				} else {
					if (editpos == 0) {
						hakuTime.edit = 1;
						hakuTime.editpos = 0;
						lcd_show_clock(&hakuTime);
						edit = 1;
					} else if (editpos == 1) {
						showalarm = SHOWDELAY;
						hakuAlarm.edit = 1;
						hakuAlarm.editpos = 0;
						lcd_show_alarm(&hakuAlarm);
						edit = 1;
					}
				}
				break;
		}
		WDT_CONTR = 0x33;
	}
}


