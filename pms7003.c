#include "types.h"
#include "pms7003.h"

void pms_read (struct envdata * env, u8 dat[])
{
	u8 chkh = 0, chkl = 0, i;
	if (dat[0] != PMS_DATA_START1 || dat[1] != PMS_DATA_START2)
		return;
	
	for (i = 0; i < PMS_DATA_CHECK_H; i += 2)
		chkh += dat[i];
	for (i = 1; i < PMS_DATA_CHECK_H; i += 2)
		chkl += dat[i];
	if (chkh != dat[PMS_DATA_CHECK_H] || chkl != dat[PMS_DATA_CHECK_L])
		return;
	
	env->pm10 = dat[PMS_DATA_PM10_H];
	env->pm10 <<= 8;
	env->pm10 |= dat[PMS_DATA_PM10_L];
	env->pm2_5 = dat[PMS_DATA_PM2_5_H];
	env->pm2_5 <<= 8;
	env->pm2_5 |= dat[PMS_DATA_PM2_5_L];
	env->pm1_0 = dat[PMS_DATA_PM1_0_H];
	env->pm1_0 <<= 8;
	env->pm1_0 |= dat[PMS_DATA_PM1_0_L];
}

