/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#include "routing.h"
#define NODE_TABLE_SIZE 5

node nextNodes[NODE_TABLE_SIZE];
void mangageNextNodes(unsigned char id, unsigned char htm) {
  // id=0 -> place in table is empty!
  unsigned char swapped;
  node tmp;
  
  if(htm < nextNodes[NODE_TABLE_SIZE -1].htm) {
    nextNodes[NODE_TABLE_SIZE -1].id  = id;	// take the better htm value
    nextNodes[NODE_TABLE_SIZE -1].htm = htm;	
  }
  
  // bubble sort node table smallest hopsToMaster first
  do {
    swapped = false;
    for(unsigned char i = 0; i < NODE_TABLE_SIZE-1; i++) {
      for(unsigned char j = i+1; j < NODE_TABLE_SIZE-1; j++) {
	if(nextNodes[i].id == nextNodes[j].id) {	// search for doublettes
					nextNodes[j].id  = 0;	// set id to non valid
	}
      }
      if(nextNodes[i].id == 0) {	// no valid id
				nextNodes[i].htm = 255;	// set htm to max to have valid id's first in table
      }
      if(nextNodes[i].id == id) {	// if node is already known
				nextNodes[i].htm = htm;	// alway take actual htm value
    }
    if (nextNodes[i+1].htm < nextNodes[i].htm) {
      // tmp save actual value
      tmp.id  = nextNodes[i].id;
      tmp.htm = nextNodes[i].htm;
      // swap values
      nextNodes[i].id    = nextNodes[i+1].id;
      nextNodes[i].htm   = nextNodes[i+1].htm;
      nextNodes[i+1].id  = tmp.id;
      nextNodes[i+1].htm = tmp.htm;
      swapped = true;
    }
  }
}  while (swapped);

  if (NODETYPE == SLAVE) {
    if(nextNodes[0].id) {	// only for slave nodes
		  myself.htm = nextNodes[0].htm + 1; // updates htm of node each time called
  }
  else {
    myself.htm = 255; 		// FIXME has to be 255 when no nodes are in table
  }
  }
}


void printNodes() { 
  Serial.println();
  for(unsigned char i = 0; i < NODE_TABLE_SIZE; i++) {
    Serial.print(" id: ");
    Serial.print(nextNodes[i].id, DEC);		
    Serial.print(" htm: ");
    Serial.println(nextNodes[i].htm, DEC);
  }
}


unsigned char allocateID() {
  static unsigned char IDcnt = 1;	// how many ID's are out there
  return ++IDcnt;
}
