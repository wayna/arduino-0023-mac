/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef ir_ctrl.h
#define ir_ctrl.h

#include "globals.h"
#include "WProgram.h"
#include "packetDef.h"
#include "mac.h"

#define	IRRECVPORT  PIND
#define	IRRECVPIN   7			// IR input low active

// Configuration of RC5 decoder
#define RC5TIME 	1.778e-3		// 1.778msec
#define PULSE_MIN 6	//(uchar)(XTAL / 512 * RC5TIME * 0.4 + 0.5)	// 6
#define PULSE_1_2 12	//(uchar)(XTAL / 512 * RC5TIME * 0.8 + 0.5)	// 11.6
#define PULSE_MAX 17	// (uchar)(XTAL / 512 * RC5TIME * 1.2 + 0.5)	// 17
// im original wird isr alle 128µs aufgerufen

#define STARTSYMBOL  0xAA
#define RC5_BITTIME  889
#define SYSCLOCK 16000000  // main Arduino clock
#define TOPBIT 0x80000000
#define CSMA_TIME 5		// ms the channel needs to be free to set PHY_CHANNEL_FREE = true



extern unsigned int rc5_data;

void init_pt_rcv();
static int proto_rcvPkt(struct pt *pt);


/************************************************************************
 * enable and configure the isr and connect it                          *
 ************************************************************************/
void enable_IR_rx();

/************************************************************************
 * init of ir led pin and ir receiver pin                               *
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
/************************************************************************
* Sends an IR mark for the specified number of microseconds.            *
* The mark output is modulated at the PWM frequency.                    *
*************************************************************************/
void mark(int time);

/************************************************************************
* Sends an IR space for the specified number of microseconds.           *
* A space is no output, so the PWM output is disabled.                  *
*************************************************************************/
void space(int time);

#endif
