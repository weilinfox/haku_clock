#ifndef __MAIN_H
#define __MAIN_H

#include "lcd1602.h"
#include "pms7003.h"
#include "ds3231.h"
#include "dht11.h"
#include "types.h"

/* 4*4 switch panel */
#define PANEL	P1

/* ds3231 module interupt */
#define CLKINT	INT0

/* pms series sensor */
#define PMSTXD	TXD
#define PMSRXD	RXD

/* dht11 series */
#define DHTINT	INT1

/* env and ala show delay */
#define SHOWDELAY	10;

/* debug light */
sbit serial_debug =	P2^3;	/* serial interrupt */
sbit clock_debug =	P2^4;	/* clock interrupt */
sbit alarm1_debug =	P2^5;	/* alarm happen */
sbit alarm2_debug =	P2^6;	/* show alarm */
sbit env_show =	P2^7;	/* show env data */



#endif
