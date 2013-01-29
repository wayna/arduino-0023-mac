/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#include "transport.h"

unsigned char NEWDATA, PINGED;


/*****************************************************************************
 * One of the key functions. Incoming packets are ordered by the specific    *
 * packet type and the corresponding functions to process the given event    *
 * are called. Handles all the protocol logic                                *
 *****************************************************************************/
void pktDaemon() {
  static unsigned long pdt, rsttime;	// packet deamon time
  static unsigned int sched;
  if(millis() - pdt >= 1) { 	// at least every 1 ms
    pdt = millis();
    sched++;
    switch(rcvPktType) { 
      case PING:
	#if CONNECTDEBUGMODUS
	Serial.print(" ->PING");
	#endif
	sendHeader(4, rxbuff[2], CTS);	// answer with a CTS pkt
	break;
      case RTS:
	#if CONNECTDEBUGMODUS
	Serial.print(" ->RTS");
	#endif
	sendHeader(4, rxbuff[2], CTS);	// answer with a CTS pkt
	break;
      case DATA:
	#if CONNECTDEBUGMODUS
	Serial.println(" ->DATA");
	#endif
	//delay(2);	// FIXME
	sendHeader(4, rxbuff[2], ACK);	// answer with a CTS pkt
	// if the node is not a master relay the data pkt
	// FIXME check not to send a pkt back to the node it came from to prevent loops
	if (NODETYPE == SLAVE) {
	  sendHeader(rxbuff[0], nextNodes[0].id, DATA);	
	  for(unsigned char i=4; i<rxbuff[0]; i++) {
	    sendByte(rxbuff[i]);	// relay content of the pkt
	  }
	  #if CONNECTDEBUGMODUS
	  Serial.print(" RLY:");
	  printPkt();
	  #endif
	}
	else {	// NODETYPE == MASTER
	  NEWDATA = true;
	  #if DEBUGMODUS
	  //	Serial.print("Got Data");
	  //	printPkt();
	  #endif
	}
	break;
	//	BEACON = packet containing the node ID and his "hops to master" as 1 byte of data
	//	nodes analyze this pkt to build the mesh network. 
      case BEACON: 
	if(!timestamp) {
	  timestamp = millis();
	}
	mangageNextNodes(rxbuff[2], rxbuff[4]);
	#if CONNECTDEBUGMODUS
	Serial.print(" ->BC[");
	Serial.print(rxbuff[2], DEC);		
	Serial.print("|");
	Serial.print(rxbuff[4], DEC);
	Serial.print("]");
	printNodes();
	#endif
	break;
	//	INID = I need ID, the nodes request for a UID to identify itself in the network.
	// 	a new key in form of a timestamp is generated every time
	// 	the node receives a beacon until . It is the 8 bit value of millis()
	//	at this time. This key is used to identify the nodes befor they 
	//	have a UID allocated from the master node himself. It is send as 
	//	1 byte of data. The master node sends the key back when responding
	//	with an ANID packet.
	#if !USESTATICID	
      case INID:
	#if CONNECTDEBUGMODUS
	Serial.print(" ->INID");
	#endif
	#if BYTEDEBUGMODUS
	Serial.print(" TSrqst:");
	Serial.print(rxbuff[4],HEX);	// print out the timestamp (=key) associated with INID request
	#endif
	// rxbuff[rxbuff[0] -1] is the ID of the next node in the chain back to the node which requested the new ID
	// if the master node receives a INID pkt, it responds with an ANID
	if (NODETYPE == MASTER) {
	  sendHeader(rxbuff[0] +1, rxbuff[rxbuff[0] -1], ANID);	// process request for a new ID
	  sendByte(allocateID());	// a freshly generated ID is also sent back
	  for(unsigned char i=4; i<rxbuff[0]; i++) {
	    sendByte(rxbuff[i]);	// relay content of the pkt
	  }
	  #if CONNECTDEBUGMODUS
	  Serial.print(" <-ANID");
	  #endif
	}
	else {	// NODETYPE == SLAVE
	  // the receiving node is not the master node, so the INID pkt has to be relayed
	  sendHeader((rxbuff[0] +1), nextNodes[0].id, INID);	
	  for(unsigned char i=4; i<rxbuff[0]; i++) {
	    sendByte(rxbuff[i]);	// relay content of the pkt
	  }
	  sendByte(myself.id);	// attach nodes own id to the pkt
	  #if CONNECTDEBUGMODUS
	  Serial.print(" RLY:");
	  printPkt();
	  #endif
	}
	break;
	// 	ANID = Allocated New ID
	//	This packet is transported back to the requesting node
      case ANID:	//FIXME should be only relevant if Node is MASTER
	#if CONNECTDEBUGMODUS
	Serial.print(" ->ANID");
	#endif
	if(myself.id) {	// id != 0 means the ANID can't be for me, so forward the pkt
	  sendHeader(rxbuff[0] -1, rxbuff[0] -1, ANID);	// process request for a new ID
	  for(unsigned char i=4; i<rxbuff[0]-1; i++) {
	    sendByte(rxbuff[i]);	// relay content of the pkt
	  }
	  #if DEBUGMODUS
	  Serial.print(" RLY:");
	  printPkt();
	  #endif
	}
	else if(rxbuff[5] == timestamp){	// it's the right key, the ANID is for me!
	  myself.id = rxbuff[4];		// ID allocated from master node
	  #if CONNECTDEBUGMODUS
	  Serial.print(" gotID:");
	  Serial.print(myself.id,DEC);
	  #endif
	}
	#if DEBUGMODUS
	Serial.print(" ->ANID key:");
	Serial.print(rxbuff[4],DEC);
	Serial.print("TS:");
	Serial.print(timestamp,DEC);
	#endif
	break;
#endif 	// USESTATICID
    }
    rcvPktType = 255;	// reset the received packet type
    
    if(sched > 5000) {	// send beacon or INID 
      sched = 0;
    if (NODETYPE == SLAVE) {
      if(timestamp) { // become active if other nodes are out there
	#if DEBUGMODUS
	Serial.print(" pktCnt:");
	Serial.print(pktCnt);	
	Serial.print(" errCnt:");
	Serial.print(errorCnt);
	#endif
	#if !USESTATICID
	static unsigned char WFR = false;
	if(myself.id == 0 && myself.htm != 255) { 	//node needs an ID, contact to other nodes exist
	  #if CONNECTDEBUGMODUS
	  Serial.print(" rqstID");
	  #endif
	  if(!WFR) {	// WFR = waiting for response
	    rsttime = millis();
	    //timestamp taken when receiving first BC
	    WFR = true;
	    sendHeader(5, nextNodes[0].id, INID);	// send INID Header
	    sendByte(timestamp);	// send UID key
	    #if DEBUGMODUS
	    Serial.print(" <-INIDkey:");
	    Serial.print(timestamp,HEX);
	    #endif
	  }
	  if(millis() - rsttime > PKTTIMEOUT * myself.htm) {
	    WFR = false;
	    timestamp = 0;
	    #if CONNECTDEBUGMODUS
	    Serial.print(" INID RST");
	    #endif
	  }
	}
	#endif
      }
    }
      #if DEBUGMODUS
      if(myself.htm == 255) {
	Serial.println(" noNodes");
      }
      if(myself.id == 0) {
	Serial.println(" noID");
      }
      #endif
      if(myself.id) {	// node has a valid ID, send a beacon
	sendHeader(5, BROADCASTADDR, BEACON);
	sendByte(myself.htm);
	#if CONNECTDEBUGMODUS
	Serial.print(" <-BC");
	Serial.print("<");
	Serial.print(myself.id,DEC);
	Serial.print("|");
	Serial.print(myself.htm,DEC);
	Serial.println("> ");
	#endif
      }
    }
  }
  if(NODE_IR_ACTIVE) {
    closeConnection();	// enable receiving pkt again	
    NODE_IR_ACTIVE = false;
  }
}
