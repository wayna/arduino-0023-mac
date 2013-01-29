/* Copyright (c) 2011, Jan Clement
* licenced under the GPL
*
* Author: Jan Clement <jan.clement@audiokits.de>
* http://code.google.com/p/arduinode/
* 
* Example code to demonstrate the use of arduinode 
* in an arduino sketch
* 
* THIS IS THE CODE FOR THE MASTER NODE
*/

#include <arduiNode.h>

/** open the config file in an editor it is in the libraries/arduinode folder **/
#include "config.h" 


void setup() {
    Serial.begin(57600);
    arduiNodeSetup(MASTER);		// initialize the arduiNode library
}

// process new DATA
void dataProcessing() {
    if(NEWDATA) {		// GLOBAL variable, set to true if a new data packet was captured
        NEWDATA = false;	// reset the variable
        Serial.println("Got Data:");
        printPkt();		// this function will print the received packet including the header
        
        /* Example code, HOWTO process received data
        * the RX_BUFFER_SIZE check makes sure the array boundaries 
        * are respected.
        * data starts at rxbuff[4] and goes to (rxbuff[0]-1)
        * look at packetDef.h for a reference of packet structure 
        */
        unsigned char i = 4; // first data byte in the rx buffer
        unsigned int result=0;
        while(i < rxbuff[0] && i < RX_BUFFER_SIZE) {
            result += rxbuff[i];	// all the data bytes are added
            i++;
        }
        Serial.println(" ");
        Serial.print(" DATA:");
        Serial.println(result, DEC);
    }
}

void loop() {
    pktDaemon();        // call this function frequently, it manages all protocol logic
    rf12_getData();     // call this often if you use the rfm12 driver
    dataProcessing();   // do smth with the received data
}