// /* 
//  * Copyright (c) 2011, Jan Clement
//  * licenced under the GPL
//  *
//  * Author: Jan Clement <jan.clement@audiokits.de>
//  *
//  * This file is part of the arduiNode library
//  * http://code.google.com/p/arduinode/
//  */
// 
// #include "ir_ctrl.h"
// 
// static struct pt pt_rcvPkt;
// unsigned char rxbuff[RX_BUFFER_SIZE], rcvPktType;
// bool NEWPACKETRECEIVED, NODE_IR_ACTIVE, PHY_CHANNEL_FREE;
// 
// void init_pt_rcv() {
//   PT_INIT(&pt_rcvPkt); 		// init proto rcv var called in isr
//   
// }
// 
// void rxtx_ctrl_init() {
//   pinMode(7, INPUT);	// IR Receiver
// }
// 
// void enableIROut(int khz) {
//   #if RAWMODE
//   Serial.println(" ");
//   Serial.print(" <-SND:");
//   #endif
//   NODE_IR_ACTIVE = true;
//   // Enables IR output.  Takes ~47µs. The khz value controls the modulation frequency in kilohertz.
//   // The IR output will be on pin 3 (OC2B).
//   // This routine is designed for 36-40KHz; if you use it for other values, it's up to you
//   // to make sure it gives reasonable results.  (Watch out for overflow / underflow / rounding.)
//   // TIMER2 is used in phase-correct PWM mode, with OCR2A controlling the frequency and OCR2B
//   // controlling the duty cycle.
//   // There is no prescaling, so the output frequency is 16MHz / (2 * OCR2A)
//   // To turn the output on and off, we leave the PWM running, but connect and disconnect the output pin.
//   // A few hours staring at the ATmega documentation and this will all make sense.
//   // See my Secrets of Arduino PWM at http://arcfn.com/2009/07/secrets-of-arduino-pwm.html for details.
//   
//   
//   // Disable the Timer2 Interrupt (which is used for receiving IR)
//   TIMSK2 &= ~_BV(TOIE2); //Timer2 Overflow Interrupt
//   
//   DDRD  |=  (1<<3);	// == pinMode(3, OUTPUT);
//   PORTD &= ~(1<<3);	// == digitalWrite(3, LOW);
//   
//   // COM2A = 00: disconnect OC2A
//   // COM2B = 00: disconnect OC2B; to send signal set to 10: OC2B non-inverted
//   // WGM2  = 101: phase-correct PWM with OCRA as top
//   // CS2   = 000: no prescaling
//   TCCR2A = _BV(WGM20);
//   TCCR2B = _BV(WGM22) | _BV(CS20);
//   
//   // The top value for the timer.  The modulation frequency will be SYSCLOCK / 2 / OCR2A.
//   OCR2A = SYSCLOCK / 2 / khz / 1000;
//   OCR2B = OCR2A / 3; // 33% duty cycle
// }
// 
// /************************************************************************
//  * Send a single Byte with modified RC5 Protokoll						*
//  * first 2 start bits are send, then the payload byte                    *
//  ************************************************************************/
// // void sendByte(unsigned char data) {
// //   #if RAWMODE
// //   Serial.print(data, HEX);	
// //   Serial.print(" ");	
// //   #endif
// //   mark( RC5_BITTIME); // First start bit
// //   space(RC5_BITTIME); 
// //   mark( RC5_BITTIME); // Second start bit
// //   for (unsigned char i = 0; i < 8; i++) { // 8 was nbits
// // 		if (data & 0x80) {
// // 		  space(RC5_BITTIME); // 1 is space, then mark
// // 		  mark(RC5_BITTIME);
// // 		} 
// // 		else {
// // 		  mark(RC5_BITTIME);
// // 		  space(RC5_BITTIME);
// // 		}
// // 		data <<= 1;
// //   }
// //   space(2*RC5_BITTIME);
// //   //TCCR2A &= ~(_BV(COM2B1)); // Disable pin 3 PWM output  Turn off at end
// // }
// // 
// #include "rf12_driver/rf12_driver/rfm12_config.h"
// #include "rf12_driver/rf12_driver/rfm12.h"
// unsigned char datab[13], buffcnt;
// 
// void sendByte(unsigned char data) {
//   if (NODE_IR_ACTIVE){	// connection is open
// // Serial.println("#");
//     datab[buffcnt++] = data;
//   }
// }
// 
// void rf12_send_buffer() {
// //   Serial.println("!");
//   rfm12_tx (sizeof(datab), 0, datab);
//   buffcnt = 0;
// }
// 
// /***********************************************************************
//  * Sends an IR mark for the specified number of microseconds.           *
//  * The mark output is modulated at the PWM frequency.                   *
//  ************************************************************************/
// void mark(int time) {
//   TCCR2A |= _BV(COM2B1); // Enable pin 3 PWM output
//   delayMicroseconds(time);
// }
// 
// 
// /***********************************************************************
//  * Sends an IR space for the specified number of microseconds.          *
//  * A space is no output, so the PWM output is disabled.                 *
//  ************************************************************************/
// void space(int time) {
//   TCCR2A &= ~(_BV(COM2B1)); // Disable pin 3 PWM output
//   delayMicroseconds(time);
// }
// 
// 
// unsigned char tcnt2;  // Timer2 reload value
// // static struct pt pt_rcvPkt;		// FIXME declared in header
// void enable_IR_rx() {
//   // takes ~4µs
//   
//   
//   /* First disable the timer overflow interrupt while we're configuring */
//   TIMSK2 &= ~(1<<TOIE2);  
//   
//   /* Configure timer2 in normal mode (pure counting, no PWM etc.) */
//   TCCR2A &= ~((1<<WGM21) | (1<<WGM20));  
//   TCCR2B &= ~(1<<WGM22);  
//   
//   /* Select clock source: internal I/O clock */
//   ASSR &= ~(1<<AS2);  
//   
//   /* Disable Compare Match A interrupt enable (only want overflow) */
//   TIMSK2 &= ~(1<<OCIE2A);  
//   
//   /* Now configure the prescaler to CPU clock divided by 128 */
//   TCCR2B |= (1<<CS22)  | (1<<CS20); // Set bits  
//   TCCR2B &= ~(1<<CS21);             // Clear bit  
//   
//   /* We need to calculate a proper value to load the timer counter. 
//    * The following loads the value 131 into the Timer 2 counter register 
//    * The math behind this is: 
//    * (CPU frequency) / (prescaler value) = 125000 Hz = 8us. 
//    * (desired period) / 8us = 125. 
//    * MAX(unsigned int8) + 1 - 125 = 131; 
//    */
//   /* Save value globally for later reload in ISR */
//   tcnt2 = 240;   // FIXME write #macro for this value
//   
//   /* Finally load end enable the timer */
//   TCNT2 = tcnt2;  
//   TIMSK2 |= (1<<TOIE2);  
// }  
// 
// /************************************************************************
//  * RC5 receive routine. Originally coded by Peter Dannegger, see        *
//  * www.mikrocontroller.net/topic/12216 for details.                     *
//  ************************************************************************/
// unsigned char	rc5_bit;	// bit value
// unsigned char	rc5_time;	// count bit time
// unsigned int	rc5_tmp;	// shift bits in
// unsigned int	rc5_data;	// GLOBAL store result
// volatile unsigned char NEWBYTERECEIVED;
// ISR(TIMER2_OVF_vect) {  
// TCNT2 = tcnt2;                  // reload the timer
// unsigned int tmp = rc5_tmp;	    // for faster access
// 
// if( ++rc5_time > PULSE_MAX ){			// count pulse time
// 		// if( !(tmp & 0x4000) && tmp & 0x2000 )	// only if 14 bits received 
// 		if( !(tmp & 0x400) && tmp & 0x200 ) {       // only if 10 bits received 
// 			rc5_data = tmp;                           // 2 start bits + 1 byte
// 			NEWBYTERECEIVED = true;
// 		}
// 		if(tmp){
// 		  // #if BYTEDEBUGMODUS == 1
// 		  // Serial.println(tmp,HEX);
// 		  // #endif	
// 		}
// 		tmp = 0;
// }
// 
// if( (rc5_bit ^ IRRECVPORT ) & 1<<IRRECVPIN ){	// change detect
// 		rc5_bit = ~rc5_bit;				// 0x00 -> 0xFF -> 0x00
// 		
// 		if( rc5_time < PULSE_MIN )			// too short
// 		tmp = 0;
// 		
// 		if( !tmp || rc5_time > PULSE_1_2 ){		// start or long pulse time
// 			if( !(tmp & 0x400) )			// not too many bits
// 			tmp <<= 1;				// shift
// 			if( !(rc5_bit & 1<<IRRECVPIN) )		// inverted bit
// 			tmp |= 1;				// insert new bit
// 			rc5_time = 0;				// count next pulse time
// 		}
// }
// rc5_tmp = tmp;
// carrierSense();		// update the PHY_CHANNEL_FREE flag
// 
//   if(NEWBYTERECEIVED){
//     proto_rcvPkt(&pt_rcvPkt);
//   }
// }
// 
// bool rf12_getData(){
//     //  FIXME I AM A DIRTY HACK..
// }
// 
// 
// unsigned char RXbyte() {
//   cli();               // detach all interupts 
//   unsigned char c = rc5_data;  // discard first byte of rc5_data (int->char)
//   rc5_data = 0;
//   sei();               // start all interupts 
//   PORTB = (PORTB & 1) ^1;
//   return c;
// }
// 
// static int proto_rcvPkt(struct pt *pt) {
//   static unsigned char i; 
//   PT_BEGIN(pt);
//   while(1) {
//     PT_WAIT_UNTIL(pt, RXbyte() == STARTSYMBOL); // FIXME implement with PT_YIELD_UNTIL???
//     NEWBYTERECEIVED = false; // startsymbol
//     //Serial.println("new byte");	
//     PT_WAIT_UNTIL(pt, NEWBYTERECEIVED);	// FIXME ??? a blocking condition??
//     NEWBYTERECEIVED = false;
//     rxbuff[0] = RXbyte(); // packet length
//     
//     i = 1;
//     while (i < rxbuff[0] && i < RX_BUFFER_SIZE) { // FIXME implement timeout 
// 			PT_WAIT_UNTIL(pt, NEWBYTERECEIVED);
// 			rxbuff[i] = RXbyte();                  // get 1 byte from rx isr
// 			NEWBYTERECEIVED = false;
// 			i++;
//   }
//   PORTB &= 0;  // led off
//   
//   // if you are here a full pkt was received..
//   /*******begin pktInspection************/
//   
//   #if RAWMODE
//   Serial.print(" ->PKT:");	
//   printPkt();
//   #endif
//   if((myself.id == 0 && rxbuff[3] == ANID) || rxbuff[1] == myself.id || rxbuff[1] == BROADCASTADDR) {	// check if the received pkt is for me or broadcast or ANID for me
// 			NEWPACKETRECEIVED = true;
// 			//enableIROut(38);	// enable IR to conserve buffer, EXPERIMENTAL
// 			rcvPktType = rxbuff[3];
//   }
//   #if RAWMODE
//   else {
//     Serial.print(" ->PNFM");	// pkt not for me
//     printPkt();
//   }
//   #endif
//   
//   /******* end pktInspection************/
//   }
//   PT_END(pt);
// }
// 
// 
// void carrierSense() {	// FIXME HACK
// //   static unsigned long tstamp;
// //   if( !(IRRECVPORT & (1<<IRRECVPIN)) ) { // pin = low = rcv
// // 		tstamp = millis();
// // 		PHY_CHANNEL_FREE = false; 
// //   }
// //   else if (millis() - tstamp > CSMA_TIME) {
//     PHY_CHANNEL_FREE = true; 
// //   }
// }
