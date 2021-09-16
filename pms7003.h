#ifndef __PMS7003_H
#define __PMS7003_H

#define PMS_DATA_LEN	32
#define PMS_DATA_START1	0x42
#define PMS_DATA_START2	0x4d
#define PMS_DATA_PM10_H	10
#define PMS_DATA_PM10_L	11
#define PMS_DATA_PM1_0_H	12
#define PMS_DATA_PM1_0_L	13
#define PMS_DATA_PM2_5_H	14
#define PMS_DATA_PM2_5_L	15
#define PMS_DATA_CHECK_H	30
#define PMS_DATA_CHECK_L	31

/* serial interrupt debug */
sbit serial_debug =	P2^5;

extern void pms_serial_init (struct envdata *);

#endif

