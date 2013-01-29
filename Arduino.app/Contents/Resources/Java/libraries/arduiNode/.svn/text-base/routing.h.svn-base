/* 
 * Copyright (c) 2011, Jan Clement
 * licenced under the GPL
 *
 * Author: Jan Clement <jan.clement@audiokits.de>
 *
 * This file is part of the arduiNode library
 * http://code.google.com/p/arduinode/
 */

#ifndef routing.h
#define routing.h

#include "mac.h"
#include "packetDef.h"

//	the node table used to manage the neighbour nodes
extern node nextNodes[];

//	update the node table and sort it smallest htm first.
void mangageNextNodes(uchar id, uchar htm) ;

// 	print the node table on console
void printNodes();

//	only used by the master node, gives back an incrementing number 
uchar allocateID();


#endif
