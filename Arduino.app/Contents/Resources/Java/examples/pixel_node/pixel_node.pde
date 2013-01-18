/***************************************************************************
*
* Title          : Arduino based Art-Net -> LED Pixel, Digital LED Strip gateway
* Version        : v 0.1 beta
* Last updated   : 02.07.2012
* Target         : Arduino mega 2560, Arduino mega 1280, *** Arduino IDE v0023 ***
* Author         : Toni Merino - merino.toni at gmail.com
* Web            : www.deskontrol.net/blog
* 
* Based on code from Christoph Guillermet, http://www.le-chat-noir-numerique.fr  -  karistouf@yahoo.fr
* 
* Structures and definitions (common.h and packet.h) from libartnet (c)Simon Newton and Lutz Hillebrand (ilLUTZminator), www.opendmx.net
*
* Art-Net™ Designed by and Copyright Artistic Licence Holdings Ltd.
*
***************************************************************************
 This program is free software; you can redistribute it and/or 
 modify it under the terms of the GNU General Public License 
 as published by the Free Software Foundation; either version 2 of 
 the License, or (at your option) any later version. 

 This program is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of 
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 General Public License for more details. 

 If you have no copy of the GNU General Public License, write to the 
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 

 For other license models, please contact the author.

;***************************************************************************/
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <Udp.h>         // UDP library from: bjoern@cs.stanford.edu 12/30/2008
#include <FastSPI_LED.h>
#include "artnet_node.h"
#include "common.h"      // headers from libartnet
#include "packets.h"     // headers from libartnet, striped version

uint8_t factory_mac          [6] = {   1,   2,   3,   0,   0,  10}; // the mac address of node
uint8_t factory_localIp      [4] = {   2,   0,   0,  20};           // the IP address of node
uint8_t factory_broadcastIp  [4] = {   2, 255, 255, 255};           // broadcast IP address
uint8_t factory_gateway      [4] = {   2,   0,   0,   0};           // gateway IP address (use ip address of controller)
uint8_t factory_subnetMask   [4] = { 255,   0,   0,   0};           // network mask (art-net use 'A' network type)

uint8_t factory_swin         [4] = {   0,   1,   2,   3};
uint8_t factory_swout        [4] = {   0,   1,   2,   3};

uint8_t  factory_shortname    [ARTNET_SHORT_NAME_LENGTH];
uint8_t  factory_longname     [ARTNET_LONG_NAME_LENGTH];

// if you have more than 1 node and need to change addresses, change here node->sub = x
// default value 0 you have output universes addressed as 0x00, 0x01, 0x02, 0x03
// change value to 1 and you have output universes addressed as 0x10, 0x11, 0x12, 0x13
// is a good idea implement a switch for change subnet address.
uint8_t factory_subnet = 0;

artnet_node_t             ArtNode;
artnet_reply_t            ArtPollReply;
//artnet_ipprog_reply_t     ArtIpprogReply; // not implemented yet
artnet_packet_type_t      packet_type;

const int MAX_BUFFER_UDP = 1650;
volatile uint8_t packetBuffer [MAX_BUFFER_UDP]; //buffer to store incoming data

#define NUM_LEDS 170  // 170 pixels * 3 colors = 510 channels
//#define NUM_LEDS 341  // uncomment for 1024 channels (slow frame rate)
//#define NUM_LEDS 512  // uncomment for 1536 channels (very slow frame rate)

// Sometimes chipsets wire in a backwards sort of way
// struct CRGB { unsigned char b; unsigned char r; unsigned char g; };
struct CRGB { unsigned char r; unsigned char g; unsigned char b; };
struct CRGB *leds;

#define PIN 4 // Arduino output pin to data input on pixels or digital led strips

void setup() 
{
  sprintf((char *)factory_shortname, "Pixel Node\0");
  sprintf((char *)factory_longname, "LED Pixel Node v1.0 (c)2012 Toni Merino www.deskontrol.net\0");
  
  fill_art_node          (&ArtNode);
    
  ArtNode.numbports  = 1; // number of universes (1 to 4)
    
  fill_art_poll_reply    (&ArtPollReply, &ArtNode);
  //fill_art_ipprog_reply  (&ArtIpprogReply, &ArtNode);
    
  // subnet mask needed because art-net uses 'A' type network (255.0.0.0), ethernet library defaults to 255.255.255.0
  Ethernet.begin(ArtNode.mac, ArtNode.localIp, ArtNode.gateway, ArtNode.subnetMask); 
  Udp.begin(ArtNode.localPort);
  
  FastSPI_LED.setLeds(NUM_LEDS);
  FastSPI_LED.setChipset(CFastSPI_LED::SPI_TM1809);
  
  FastSPI_LED.setPin(PIN);
  
  FastSPI_LED.init();
  FastSPI_LED.start();

  leds = (struct CRGB*)FastSPI_LED.getRGBData();
}

void loop() 
{
  if( Udp.available() > ARNET_HEADER_SIZE ) 
    handle_packet();
}

void handle_packet()
{
  Udp.readPacket((uint8_t *)&packetBuffer, MAX_BUFFER_UDP, (uint8_t *)&ArtNode.remoteIp, (uint16_t *)&ArtNode.remotePort);  
  
  packet_type = (artnet_packet_type_t) get_packet_type((uint8_t *)&packetBuffer);
    
  if(packet_type == 0)  // bad packet
  {
    return;
  }  
  if(packet_type == ARTNET_DMX)
  {
    if(sizeof(packetBuffer) < sizeof(artnet_dmx_t)) // take care with programs sending less than 512 channels (comment this 3 lines 
      return;                                       // if controller is sending less channels, or configure controller to send full frames
    else                                            // with 512 channels each)
      handle_dmx((artnet_dmx_t *)&packetBuffer);
  }   
  else if(packet_type == ARTNET_POLL)
  {
    if(sizeof(packetBuffer) < sizeof(artnet_poll_t)) 
      return;
    else
      handle_poll((artnet_poll_t *)&packetBuffer);
  } /*
  else if(packet_type == ARTNET_IPPROG)
  {
    if(sizeof(packetBuffer) < sizeof(artnet_ipprog_t))
      return;
    else
      handle_ipprog((artnet_ipprog_t *)&packetBuffer);
  } */
  else if(packet_type == ARTNET_ADDRESS)
  {
    if(sizeof(packetBuffer) < sizeof(artnet_address_t))
      return;
    else
      handle_address((artnet_address_t *)&packetBuffer);
  } 
}

uint16_t get_packet_type(uint8_t *packet) //this get artnet packet type
{
  if (! memcmp( packet, ArtNode.id, 8)) 
  {
    return bytes_to_short(packet[9], packet[8]); 
  } 
  return 0;
}

int handle_dmx(artnet_dmx_t *packet)
{
  uint16_t packet_length = bytes_to_short(packet->lengthHi, packet->length); 
  
  if(packet->universe == 0)
  { 
    memcpy((uint8_t *)leds, packet->data, NUM_LEDS * 3);
  }
  else if(packet->universe == 1)
  {   
    // memcpy((uint8_t *)&leds[512], packet->data, NUM_LEDS * 3); // uncomment for 1024 channels
  }
  else if(packet->universe == 2)
  {   
    // memcpy((uint8_t *)&leds[1024], packet->data, NUM_LEDS * 3); // uncomment for 1536 channels (slow frame rate)
  }
  else if(packet->universe == 3)
  {   
    // memcpy((uint8_t *)&leds[1536], packet->data, NUM_LEDS * 3); // uncomment for 2048 channels (very slow frame rate)
  }
  
  FastSPI_LED.show();
}

int handle_poll(artnet_poll_t *packet) 
{
  if((packet->ttm & 1) == 1) // controller say: send unicast reply
  {
    send_reply(UNICAST, (uint8_t *)&ArtPollReply, sizeof(ArtPollReply));
  }
  else // controller say: send broadcast reply
  {
    send_reply(BROADCAST, (uint8_t *)&ArtPollReply, sizeof(ArtPollReply));
  }
}
/*
int handle_ipprog(artnet_ipprog_t *packet) 
{
  send_reply(UNICAST, (uint8_t *)&ArtIpprogReply, sizeof(ArtIpprogReply));//ojo
}
*/
int handle_address(artnet_address_t *packet) //not implemented yet
{
  if(packet->subnet == 0)
  {
    memcpy (ArtNode.shortname, factory_shortname, sizeof(factory_shortname));
    memcpy (ArtNode.longname, factory_longname, sizeof(factory_longname));
  }
  else
  {
    memcpy (ArtNode.shortname, packet->shortname, ARTNET_SHORT_NAME_LENGTH);
    memcpy (ArtNode.longname, packet->longname, ARTNET_LONG_NAME_LENGTH);
  }
  
  for(uint8_t i = 0; i <= 4; i++)
  {
    if(packet->swout[i] & 0x80)
      ArtNode.swout[i] = packet->swout[i] - 0x80;
    else if(packet->swout[0] == 0)
      ArtNode.swout[i] = factory_swout[i];
      
    if(packet->swin[i] & 0x80)
      ArtNode.swin[i] = packet->swin[i] - 0x80;
    else if(packet->swin[i] == 0)
      ArtNode.swin[i] = factory_swin[i];
  }
  
  if(packet->subnet & 0x80)
    ArtNode.sub = packet->subnet - 0x80;
  else if(packet->subnet == 0)
    ArtNode.sub = factory_subnet;
    
  // change reply values
  fill_art_poll_reply    (&ArtPollReply, &ArtNode);
  // send ArtPollReply
  send_reply(UNICAST, (uint8_t *)&ArtPollReply, sizeof(ArtPollReply));
}

void send_reply(uint8_t mode_broadcast, uint8_t *packet, uint16_t size)
{
  if(mode_broadcast == 1) // send broadcast packet
  {
    Udp.sendPacket(packet, size, ArtNode.broadcastIp, ArtNode.remotePort);
  }
  else // send unicast packet to controller
  {
    Udp.sendPacket(packet, size, ArtNode.remoteIp, ArtNode.remotePort);
  }
}

void fill_art_node(artnet_node_t *node)
{
  //fill to 0's
  memset (node, 0, sizeof(node));
  
  //fill data
  memcpy (node->mac, factory_mac, 6);                   // the mac address of node
  memcpy (node->localIp, factory_localIp, 4);           // the IP address of node
  memcpy (node->broadcastIp, factory_broadcastIp, 4);   // broadcast IP address
  memcpy (node->gateway, factory_gateway, 4);           // gateway IP address
  memcpy (node->subnetMask, factory_subnetMask, 4);     // network mask 
  memcpy (node->swout, factory_swout, 4);
  memcpy (node->swin, factory_swin, 4);
  
  memcpy (node->shortname, factory_shortname, sizeof(factory_shortname));
  memcpy (node->longname, factory_longname, sizeof(factory_longname));
  
  sprintf((char *)node->id, "Art-Net\0");
      
  memset (node->porttypes,  0x80, 4);
  memset (node->goodinput,  0x08, 4);

#if defined(USE_UNIVERSE_0)
  node->goodoutput [0] = 0x80;
#endif

#if defined(USE_UNIVERSE_1)
  node->goodoutput [1] = 0x80;
#endif

#if defined(USE_UNIVERSE_2)
  node->goodoutput [2] = 0x80;
#endif

#if defined(USE_UNIVERSE_3)
  node->goodoutput [3] = 0x80;
#endif

  node->etsaman[0] = 0;        // The ESTA manufacturer code.
  node->etsaman[1] = 0;        // The ESTA manufacturer code.
  node->localPort  = 0x1936;   // artnet UDP port is by default 6454 (0x1936)
  node->verH       = 1;        // high byte of Node firmware revision number.
  node->ver        = 0;        // low byte of Node firmware revision number.
  node->ProVerH    = 0;        // high byte of the Art-Net protocol revision number.
  node->ProVer     = 14;       // low byte of the Art-Net protocol revision number.
  node->subH       = 0;        // high byte of the Node Subnet Address
  node->sub        = factory_subnet;   // low byte of the Node Subnet Address
  node->oemH       = 0;        // high byte of the oem value.
  node->oem        = 0xFF;     // low byte of the oem value. (0x00FF = developer code)
  node->ubea       = 0;        // This field contains the firmware version of the User Bios Extension Area (UBEA). 0 if not used
  node->status     = 0x08;
  node->swvideo    = 0;
  node->swmacro    = 0;
  node->swremote   = 0;
  node->style      = 0;        // StNode style - A DMX to/from Art-Net device
}

void fill_art_poll_reply(artnet_reply_t *poll_reply, artnet_node_t *node)
{
  //fill to 0's
  memset (poll_reply, 0, sizeof(poll_reply));
  
  //copy data from node
  memcpy (poll_reply->id, node->id, sizeof(poll_reply->id));
  memcpy (poll_reply->ip, node->localIp, sizeof(poll_reply->ip));
  memcpy (poll_reply->mac, node->mac, sizeof(poll_reply->mac)); 
  memcpy (poll_reply->shortname, node->shortname, sizeof(poll_reply->shortname)); 
  memcpy (poll_reply->longname, node->longname, sizeof(poll_reply->longname));
  memcpy (poll_reply->nodereport, node->nodereport, sizeof(poll_reply->mac));
  memcpy (poll_reply->porttypes, node->porttypes, sizeof(poll_reply->porttypes));
  memcpy (poll_reply->goodinput, node->goodinput, sizeof(poll_reply->goodinput));
  memcpy (poll_reply->goodoutput, node->goodoutput, sizeof(poll_reply->goodoutput));
  memcpy (poll_reply->swin, node->swin, sizeof(poll_reply->swin));
  memcpy (poll_reply->swout, node->swout, sizeof(poll_reply->swout));
  memcpy (poll_reply->etsaman, node->etsaman, sizeof(poll_reply->etsaman));
  
  sprintf((char *)poll_reply->nodereport, "%i DMX output universes active.\0", node->numbports);
  
  poll_reply->opCode          = 0x2100;  // ARTNET_REPLY
  poll_reply->port            = node->localPort;
  poll_reply->verH            = node->verH;
  poll_reply->ver             = node->ver;
  poll_reply->subH            = node->subH;
  poll_reply->sub             = node->sub;
  poll_reply->oemH            = node->oemH;
  poll_reply->oem             = node->oem;
  poll_reply->status          = node->status;
  poll_reply->numbportsH      = node->numbportsH;
  poll_reply->numbports       = node->numbports;
  poll_reply->swvideo         = node->swvideo;
  poll_reply->swmacro         = node->swmacro;
  poll_reply->swremote        = node->swremote;
  poll_reply->style           = node->style; 
}
/*
void fill_art_ipprog_reply(artnet_ipprog_reply_t *ipprog_reply, artnet_node_t *node)
{
  //fill to 0's
  memset (ipprog_reply, 0, sizeof(ipprog_reply));
  
  //copy data from node
  memcpy (ipprog_reply->id, node->id, sizeof(ipprog_reply->id));
  
  ipprog_reply->ProgIpHi  = node->localIp[0];
  ipprog_reply->ProgIp2   = node->localIp[1];
  ipprog_reply->ProgIp1   = node->localIp[2];
  ipprog_reply->ProgIpLo  = node->localIp[3];
  
  ipprog_reply->ProgSmHi  = node->subnetMask[0];
  ipprog_reply->ProgSm2   = node->subnetMask[1];
  ipprog_reply->ProgSm1   = node->subnetMask[2];
  ipprog_reply->ProgSmLo  = node->subnetMask[3];
    
  ipprog_reply->OpCode        = 0xF900; //ARTNET_IPREPLY
  ipprog_reply->ProVerH       = node->ProVerH;
  ipprog_reply->ProVer        = node->ProVer;
  ipprog_reply->ProgPortHi    = node->localPort >> 8;
  ipprog_reply->ProgPortLo    =(node->localPort & 0xFF);
} 
*/

