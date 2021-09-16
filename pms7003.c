#include <reg52.h>
#include "types.h"
#include "pms7003.h"

u8 pmsdata[PMS_DATA_LEN];
u8 pmsdatacnt;
struct envdata * envdata;

void pms_serial_init (struct envdata * env)
{
	TMOD = 0x20;
	TH1 = 0xfd;
	TL1 = 0xfd;
	TR1 = 1;
	PCON &= 0x7f;
	SCON = 0x50;
	IE = 0x90;
	pmsdatacnt = 0;
	envdata = env;
	serial_debug = 0;
}

void pms_serial_interrupt (void) interrupt 4
{
	u8 i;
	u16 chk;

	if (pmsdatacnt >= PMS_DATA_LEN)
		pmsdatacnt = 0;
	if (SBUF == PMS_DATA_START1) {
		pmsdata[pmsdatacnt=0] = SBUF;
	} else if (pmsdatacnt) {
		pmsdata[pmsdatacnt] = SBUF;
	}
	pmsdatacnt++;
	if (pmsdatacnt == PMS_DATA_LEN) {
		/*env_get = ~env_get;*/
		chk = 0;
		for (i = 0; i < PMS_DATA_CHECK_H; i++)
			chk += pmsdata[i];
		if ((pmsdata[0] == PMS_DATA_START1 && pmsdata[1] == PMS_DATA_START2)
			&& (chk>>8 == pmsdata[PMS_DATA_CHECK_H] && chk%0x0100 == pmsdata[PMS_DATA_CHECK_L])) {

			envdata->pm10 = pmsdata[PMS_DATA_PM10_H];
			envdata->pm10 <<= 8;
			envdata->pm10 |= pmsdata[PMS_DATA_PM10_L];
			envdata->pm2_5 = pmsdata[PMS_DATA_PM2_5_H];
			envdata->pm2_5 <<= 8;
			envdata->pm2_5 |= pmsdata[PMS_DATA_PM2_5_L];
			envdata->pm1_0 = pmsdata[PMS_DATA_PM1_0_H];
			envdata->pm1_0 <<= 8;
			envdata->pm1_0 |= pmsdata[PMS_DATA_PM1_0_L];
			serial_debug = 1;
		}

		pmsdatacnt = 0;
	}
	TI = RI = 0;
}
