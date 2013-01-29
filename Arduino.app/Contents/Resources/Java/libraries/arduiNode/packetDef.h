/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef packetDef.h
#define packetDef.h

#include "config.h"

typedef struct {
	unsigned char id;
	unsigned char htm;
}
node;

extern unsigned char MASTERNODE;

#define PKTTIMEOUT 1000		// how many ms to wait for an ACK befor to resend the pkt
#define PKTRETRY 4		// how many time to retry until to delete this node from the node table
#define BROADCASTADDR 255	// BEACONS use this, this goes out to every node

// Packet organisation
// RtsAckCts:	| length | toAddr | fromAddr | pkt type |
// BEACON:	| length | toAddr | fromAddr | BEACON   | HTM     |
// DATA:	| length | toAddr | fromAddr | pkt type | data    |
// INID: 	| length | toAddr | fromAddr | INID     | nodeKey | ID(a)   | ID(b) | ID(n) |
// ANID:	| length | toAddr | fromAddr | ANID     | newID   | nodeKey | ID(a) | ID(b) | ID(n) |
// rxbuff[]	    [0]	     [1]       [2]       [3]        [4]	      [5]      [6]     [7]

// packet type number

#define RTS 	0
#define CTS  	1
#define ACK  	2
#define PING 	3
#define ECHO 	4
#define DATA 	5
#define BEACON  7	// contains HTM as 1 data byte
#define INID	8	// I NeedID, node needs an ID
#define ANID	9	// allocated new ID, response to INID from the master node
#define PKTACKED 42	// used to determine if macProtoSendReliablePacket() succeded
#endif

