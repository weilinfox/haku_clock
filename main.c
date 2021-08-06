#include <reg52.h>

#include "main.h"

void main()
{
	struct time hakuTime = 	{2021, 8, 3, 0,
				{23, 59, 59, 1, 0},
				0, 0};
	struct alarms hakuAlarm =	{{6, 0, 0}, {6, 0, 0},
				0, 0, 0, 0};
	struct envdata hakuEnv =	{0, 0, 0, 0, 0, 0};
	
	//_nop_();
	
	
	
	
	lcd_init();
	ds_init();
	ds_read_data(&hakuTime);

	lcd_show_clock(&hakuTime);
	//lcd_show_alarm(&hakuAlarm);
	//lcd_show_env(&hakuEnv);
	//while (1) lcd_show_env(&hakuEnv);
	while(1) {
		ds_read_data(&hakuTime);
		lcd_show_clock(&hakuTime);
	}
	
	//return 0;
}


