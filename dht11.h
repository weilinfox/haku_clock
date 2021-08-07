#ifndef __DHT11_H
#define __DHT11_H

sbit DHT_BUS =	P2^2;

extern void dht_read_data (struct envdata *);
extern void dht_init (void);

#endif
