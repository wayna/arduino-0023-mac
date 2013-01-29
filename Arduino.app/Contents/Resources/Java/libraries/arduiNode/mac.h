/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef mac.h
#define mac.h

#include "phy.h"
#include "routing.h"

extern unsigned int errorCnt, pktCnt;
extern unsigned char timestamp;
extern node myself;

/************************************************************************
 *  
 ************************************************************************/
void init_pt_snd();

/************************************************************************
 *  
 ************************************************************************/
void sendHeader(unsigned char length, unsigned char toAddr, unsigned char type);

/************************************************************************
 *  
 ************************************************************************/
void closeConnection();

/************************************************************************
 *  
 ************************************************************************/
bool sendData(unsigned char *data, unsigned char length);

/************************************************************************
 *  
 ************************************************************************/
static int macProtoSendReliablePacket(struct pt *pt, unsigned char *data, unsigned char length);

/************************************************************************
 *  
 ************************************************************************/
unsigned char CSMA_channel_free();



#endif
