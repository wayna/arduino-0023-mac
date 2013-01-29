/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef phy.h
#define phy.h


#include "globals.h"

#include "config.h"


/************************************************************************
 * sends raw bytes of data,                                             *
 ************************************************************************/
void phySendPacket(unsigned char to, unsigned char from, unsigned char type, unsigned char *data, unsigned char length);

/************************************************************************
 * prints the actual content of RX_BUFFER                               *
 ************************************************************************/
void printPkt();


#endif
