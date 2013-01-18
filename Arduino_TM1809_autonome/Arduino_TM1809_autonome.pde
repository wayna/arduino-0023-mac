#include <SPI.h>         // needed for Arduino versions later than 0018
#include <FastSPI_LED.h>
#include <SerialCommand.h>

#define NUM_LEDS 76

// Sometimes chipsets wire in a backwards sort of way
struct CRGB { unsigned char b; unsigned char r; unsigned char g; };
// struct CRGB { unsigned char r; unsigned char g; unsigned char b; };
struct CRGB *leds;

#define PIN 11 //pin output for SPI

#define BUTTON1 2
#define BUTTON2 3
#define DELAY(X) if(mustReset){return ;}delay(X);if(mustReset){return ;}

int state = 0;
volatile int program = 12;
volatile int mustReset = false;

void setup ()
{
  Serial.begin(9600);
  pinMode (BUTTON1, INPUT);
  pinMode (BUTTON2, INPUT);
  FastSPI_LED.setLeds(NUM_LEDS);
  FastSPI_LED.setChipset(CFastSPI_LED::SPI_TM1809);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_TM1809);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_LPD6803);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_HL1606);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_595);
  //FastSPI_LED.setChipset(CFastSPI_LED::SPI_WS2801);

  FastSPI_LED.setPin(PIN);
  
  FastSPI_LED.init();
  FastSPI_LED.start();

  leds = (struct CRGB*)FastSPI_LED.getRGBData(); 

  attachInterrupt(0, changeProgram, RISING );
  
}


void changeProgram(){
    if (--program == 8){
		program = 12; 
	}
	mustReset=true;
}

void oneAtATime(){
  for(int j = 0; j < 3; j++) { 
    for(int i = 0 ; i < NUM_LEDS; i++ ) {
      memset(leds, 0, NUM_LEDS * 3);
      switch(j) { 
        case 0: leds[i].r = 255; break;
        case 1: leds[i].g = 255; break;
        case 2: leds[i].b = 255; break;
      }
      FastSPI_LED.show();
      DELAY(0);
    }
  }
}


void growingBar(){
  for(int j = 0; j < 3; j++) { 
    memset(leds, 0, NUM_LEDS * 3);
    for(int i = 0 ; i < NUM_LEDS; i++ ) {
      switch(j) { 
        case 0: leds[i].r = 255; break;
        case 1: leds[i].g = 255; break;
        case 2: leds[i].b = 255; break;
      }
      FastSPI_LED.show();
      DELAY(0);
    }
    for(int i = NUM_LEDS-1 ; i >= 0; i-- ) {
      switch(j) { 
        case 0: leds[i].r = 0; break;
        case 1: leds[i].g = 0; break;
        case 2: leds[i].b = 0; break;
      }
      FastSPI_LED.show();
      DELAY(0);
    }
  }
}

void FadeInFadeOut(){
  for(int j = 0; j < 3; j++ ) { 
    memset(leds, 0, NUM_LEDS * 3);
    for(int k = 0; k < 256; k++) { 
      for(int i = 0; i < NUM_LEDS; i++ ) {
        switch(j) { 
          case 0: leds[i].r = k; break;
          case 1: leds[i].g = k; break;
          case 2: leds[i].b = k; break;
        }
      }
      FastSPI_LED.show();
      DELAY(0);
    }
    for(int k = 255; k >= 0; k--) { 
      for(int i = 0; i < NUM_LEDS; i++ ) {
        switch(j) { 
          case 0: leds[i].r = k; break;
          case 1: leds[i].g = k; break;
          case 2: leds[i].b = k; break;
        }
      }
      FastSPI_LED.show();
      DELAY(0);
    }
  }  

}


void noProgram(){
	delay(100);
}

void loop ()
{ 
  mustReset=false;
  Serial.print ("int program= ");
  Serial.println(program);
  switch(program){
	  case 12 : oneAtATime(); break;
	  case 11 : growingBar(); break;
	  case 10 : FadeInFadeOut(); break;
	  case 9 : noProgram();break;
	  default : noProgram(); break;
  }
  
  Serial.print ("PROGRAMME: ");
  if (program == 12) { Serial.println ("PROGRAMME 1"); }
  if (program == 11) { Serial.println ("PROGRAMME 2"); }
  if (program == 10) { Serial.println ("PROGRAMME 3"); }
  if (program == 9) { Serial.println ("OFF"); }
}



