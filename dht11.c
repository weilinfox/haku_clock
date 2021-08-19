
#include <reg52.h>
#include "dht11.h"
#include "types.h"

void dht_init ()
{
	DHT_BUS = 1;
	DHT_DEBUG = 0;
}

void dht_delay10us (void)
{
	u8 i;
	i--; i--; i--; i--; i--; i--;
}

/*
void dht_delay5us (void)
{
	u8 i;
	i--; i--; i--;
}
*/

u8 dht_read_byte (void)
{
	u8 dat = 0, i;
	for (i = 8; i; i--) {
		dat <<= 1;
		while (~DHT_BUS);
		dht_delay10us();
		dht_delay10us();
		dht_delay10us();
		dht_delay10us();
		if (DHT_BUS) {
			dat |= 0x01;
			while (DHT_BUS);
		}
	}
	return dat;
}

void dht_read_data (struct envdata * env)
{
	u8 i, dat, sum = 0;
	s16 temp = 0;
	u16 humidity = 0;
	EA = 0;
	DHT_BUS = 0;
	for (i = 18; i; i--)
	for (dat = 100; dat; dat--)
		dht_delay10us();
	DHT_BUS = 1;
	dht_delay10us();
	dht_delay10us();
	dht_delay10us();
	dht_delay10us();
	if (~DHT_BUS) {
		/* dht reply */
		while (~DHT_BUS) ;
		while (DHT_BUS) ;
		/* start data */
		dht_delay10us();
		dat = dht_read_byte();
		sum += dat;
		humidity = dat;

		dat = dht_read_byte();
		sum += dat;

		dat = dht_read_byte();
		sum += dat;
		temp = dat;
		
		dat = dht_read_byte();
		sum += dat;
		
		dat = dht_read_byte();
		if (dat == sum) {
			env->humidity = humidity;
			env->temp = (s16)temp * 10;
			DHT_DEBUG = 1;
		} else {
			DHT_DEBUG = ~DHT_DEBUG;
		}
	} else {
		DHT_DEBUG = 0;
	}
	EA = 1;
}
