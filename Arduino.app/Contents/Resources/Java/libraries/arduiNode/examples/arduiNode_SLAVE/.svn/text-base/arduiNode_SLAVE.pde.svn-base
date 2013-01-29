/* Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 * http://code.google.com/p/arduinode/
 * 
 * Example code to demonstrate the use of arduinode 
 * in an arduino sketch
 * 
 * THIS IS THE CODE FOR THE SLAVE NODE
 */

#include "arduinode.h"

// open the config file in an editor
// it is in the libraries/arduinode folder
#include "config.h"	


void setup() {
  Serial.begin(57600);
  arduiNode(SLAVE);		// initialize the arduiNode library
}

uchar tx[5] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE}; // some data to send
unsigned long t;

void loop() {
  pktDaemon();  
  if(millis() - t > 3000) {	// send data every 3000ms
    t= millis();
    if(myself.id) {	//check if the node already has an ID
      Serial.println("send data");
      sendData(tx, 5);
    }
  }
}
