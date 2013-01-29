#include "rf12_wrap.h"

// void led(byte val){
//   digitalWrite(7, val);
// }

// void setup() {
//   pinMode(7, OUTPUT);
//   rfm12_init();
//   sei();
//   Serial.begin(9600);
//   Serial.print("RFM12 Das Labor");
//   rfm12_poll();
//   rfm12_tick();
// }


void testFun(){
  
  rfm12_poll();

}









// unsigned long rf12_timer = millis();

// uint8_t *bufcontents;
// uint8_t i;
// uint8_t tv[] = "BÄMBÄM ";
// uint8_t pktcnt[1];if (millis() - timer >= 3000)

/**
void loop__() {
  uint8_t *bufcontents;
  uint8_t i;
  uint8_t tv[] = "foobar";
  uint8_t cnt[1], cnt_char;


  _delay_ms(250);
  _delay_ms(250);
  _delay_ms(250);
  
  rfm12_init();
  
  sei();
  
  
  while (42) {
    rfm12_poll();
    rfm12_tick();

    if (rfm12_rx_status() == STATUS_COMPLETE){
      led(1);
      Serial.print("new packet:\r\n");
      bufcontents = rfm12_rx_buffer();
      // dump buffer contents to uart			
      for (i=0;i<rfm12_rx_len();i++){
	Serial.print(bufcontents[i] );
      }
      rfm12_rx_clear();
      led(0);
    }
    if(millis() - timer >= 1500){
      led(1);
      timer = millis();
      Serial.print("send pkt..");
//       Serial.println(cnt_char++, DEC);
      rfm12_tx (sizeof(tv), 0, tv);
//       _delay_ms(250);
//       cnt[0] = cnt_char;
//       rfm12_tx (sizeof(cnt), 0, cnt);
      led(0);
    }
  }
}
***/