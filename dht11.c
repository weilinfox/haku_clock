
#include <reg52.h>
#include "dht11.h"
#include "types.h"

void dht_init ()
{
	DHT_BUS = 1;
}

void dht_delay2us (u8 i)
{
	for ( ; i; i--) ;
}

u8 dht_read_byte (void)
{
	u8 dat = 0, i;
	for (i = 8; i; i--) {
		dat <<= 1;
		dht_delay2us(16);
		if (DHT_BUS) {
			dat |= 0x01;
			dht_delay2us(30);
		}
	}
	return dat;
}

void dht_read_data (struct envdata * env)
{
	u8 i, dat, sum = 0;
	s16 temp = 0;
	u16 humidity = 0;
	DHT_BUS = 0;
	for (i = 40; i; i--)
		dht_delay2us(255);
	DHT_BUS = 1;
	dht_delay2us(21);
	if (~DHT_BUS) {
		/* dht reply */
		while (~DHT_BUS) ;
		while (DHT_BUS) ;
		/* start data */
		dht_delay2us(12);
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
			env->temp = temp;
		}
	}
}