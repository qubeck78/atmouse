#ifndef _CONFIG_H
#define _CONFIG_H

#define uchar unsigned char
#define ushort unsigned short
#define ulong unsigned long

#define PS2_MOUSEDATA	(1<<3)
#define PS2_MOUSECLK	(1<<2)

//PD3 ->DATA
#define PS2_MOUSEDATA_OUT()	DDRD|=PS2_MOUSEDATA
#define PS2_MOUSEDATA_IN()	DDRD&=(PS2_MOUSEDATA^0xff);

//PD2 ->CLK
#define PS2_MOUSECLK_OUT()	DDRD|=PS2_MOUSECLK;
#define PS2_MOUSECLK_IN()	DDRD&=(PS2_MOUSECLK^0xff)

//PD6 -> RMB
#define AMI_RMB (1<<6)

//PB0 -> LMB
#define AMI_LMB (1<<0)
	
//PB1 -> HQP
#define AMI_HQP (1<<1)

//PB2 -> VQP
#define AMI_VQP (1<<2)

//PB3 -> HP
#define AMI_HP (1<<3)

//PB4 -> VP
#define AMI_VP (1<<4)


#include <stdint.h>
#include <stdlib.h>

#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>


#endif
