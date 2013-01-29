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

#include "ir_ctrl.h"
#include "phy.h"
#include "routing.h"

extern unsigned int errorCnt, pktCnt;
extern uchar timestamp;
extern node myself;	// myID, myHTM
// extern struct pt pt_relPktSend;
void init_pt_snd();
void sendHeader(uchar length, uchar toAddr, uchar type);
void closeConnection();
//void sendINID();
//void sendANID();
//void sendBeacon();
//void sendRtsCtsAck(uchar type, uchar toAddr);
void sendData(uchar *data, uchar length);
static int macProtoSendReliablePacket(struct pt *pt, uchar *data, uchar length);


unsigned char CSMA_channel_free();



#endif
