/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef main.h
#define main.h



#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <stdlib.h>
//#include <../pt/pt.h>
#include "../pt/pt.h"	// FIXME
#include "config.h"

#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))


extern unsigned char NODETYPE;
extern unsigned char rxbuff[], txbuff[], rcvPktType;
extern bool NEWPACKETRECEIVED;
extern bool PHY_CHANNEL_FREE;
extern bool NODE_IR_ACTIVE;



#endif
