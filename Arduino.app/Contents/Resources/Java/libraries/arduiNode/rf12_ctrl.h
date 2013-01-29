/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef rf12_ctrl.h
#define rf12_ctrl.h

#include "globals.h"
#include "WProgram.h"
#include "packetDef.h"
#include "mac.h"


#include "rf12_driver/rf12_driver/rfm12_config.h"
#include "rf12_driver/rf12_driver/rfm12.h"
// #include "rf12_driver/rf12_driver/rfm12.c"

#define STARTSYMBOL  0xAA




void init_pt_rcv();


static int proto_rcvPkt(struct pt *pt);


/************************************************************************
 * DUMMY FUNCTION                       *
 ************************************************************************/
void enable_IR_rx();	// FIXME I AM A DIRTY HACK..	

/************************************************************************
 * Init of the rfm12 <driver                                            *
 ************************************************************************/
void rxtx_ctrl_init();

/************************************************************************
 * checks if the medium is free,updates the boolean PHY_CHANNEL_FREE    *
 ************************************************************************/
void carrierSense();

/************************************************************************
 * RC5 receive routine. Originally coded by Peter Dannegger, see        *
 * www.mikrocontroller.net/topic/12216 for details.                     *
 ************************************************************************/
ISR(TIMER2_OVF_vect);

/************************************************************************
 * Enables IR output. The khz value controls the modulation frequency   *
 * in kilohertz. The IR output will be on pin 3 (OC2B)                  *
 ************************************************************************/
void enableIROut(int khz);

/************************************************************************
* Send a single Byte with modified RC5 Protokoll						*
* first 2 start bits are send, then the payload byte                    *
************************************************************************/
void sendByte(unsigned char data);
void rf12_send_buffer();	// FIXME I AM A DIRTY HACK..	
bool rf12_getData();		// FIXME I AM A DIRTY HACK..	

#endif
