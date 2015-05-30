/*******************************************************************
  Adafruit Animal - Sound testing module
 
  Works on Arduino Uno, Leonardo, Mega, Menta, etc.
  can work on Trinket with right pin mapping and no Serial port
 *******************************************************************/

#include <Adafruit_NeoPixel.h>

// pins
#define SPEAKER   3   // Piezo Speaker pin (positive, other pin to ground)
#define POT       A2   // for Trinket, use 1 for #2, 3 for #3, 2 for #4
                       //   for Uno/Leo/Mega A0 to A5
#define PIXPIN  4
#define RINGSIZE 12
 
#define BORINGTIME 10000
 
 Adafruit_NeoPixel pixels = Adafruit_NeoPixel(32, PIXPIN);
 
 
 
 uint8_t  anim_mode   = 0, // Current animation effect
         anim_pos = 0,
         leftOff = 7, // Position of spinny eyes
         rightOff = 2,
         ring_pos = 8;

uint32_t color  = 0xFF0000; // Start red
uint32_t prevTime;

uint32_t anim_time;

uint32_t last_event;

   int reading;
 
 int last_reading = 0;
unsigned long tone_start_time; // time of last read that was closed
 
void setup() {
 randomSeed(analogRead(1));

  pinMode(SPEAKER,OUTPUT);  // important to set pin as output
  
   pixels.begin();
  pixels.setBrightness(85); // 1/3 brightness
  prevTime = millis();
  reset_idle();
  
}
 
void loop()  {

 reading =analogRead(POT);
 if (reading >800){
     SolidRing(pixels.Color(255,255,255)); // black to start
     pixels.setBrightness(255); // full brightness
 pewpew2(10);
 delay(80);
  pewpew2(70);

  reset_idle();
   pixels.setBrightness(25); // 1/3 brightness
    delay(200);
    reset_idle();
   
   
 } else {
   
    if ( reading >200){
    SolidRing(0); // black to start
     pixels.setBrightness(255); // full brightness
 pewpew2(10);
 delay(80);
  pewpew2(70);

  reset_idle();
   pixels.setBrightness(25); // 1/3 brightness
    delay(400);
    reset_idle();
    
  } else {
  idle_anim();
  } 
   
 }

  
  

 // updatesound();
// scale();  // if you would like to hear the whole frequency
             //   range, you can use this function
}
 


void pewpew2(int durfac){
  int i;
  int j;
  int pick_a_pixel = 0;
  int step_accel =0;
  byte button_state = 1;
  uint32_t clr;
  uint32_t sparkle_time = millis();
  int duration = 300;
    for( i=1000; i<1400; ( i = (i+1+(step_accel/durfac))  )  ){
     tone_start_time = micros();
    
  
      while(micros() < tone_start_time + duration){ 
        digitalWrite(SPEAKER,button_state);
       delayMicroseconds(i);
       button_state = !button_state;
     
      }

          step_accel++;
          if (sparkle_time + 20 < millis()){          
            pixels.setPixelColor(pick_a_pixel, 0);
             pixels.setPixelColor(pick_a_pixel + RINGSIZE,   0 );
            pick_a_pixel = random(RINGSIZE);
            clr = random_color();
            pixels.setPixelColor(pick_a_pixel,   clr );
            pixels.setPixelColor(pick_a_pixel + RINGSIZE,   clr );
            pixels.show();
            sparkle_time = millis();
    
        }
          
    }
    pixels.setPixelColor(pick_a_pixel, 0);
    pixels.setPixelColor(pick_a_pixel + RINGSIZE,   0 );
    pixels.show();
}


void idle_anim(){
  // uses some globals:
  //   color
  //   anim_mode
  //   anim_pos
  
  byte i;
  
  
  if ((millis() - anim_time)> BORINGTIME) {
    reset_idle();
  }
  switch(anim_mode) {
    case 0: //pulse
    
    SolidRing(color);
    if(anim_pos > 64){
      ring_pos = 128-anim_pos;
    } else {
      ring_pos = anim_pos;
    }
     pixels.setBrightness(255*ring_pos/1024); // 1/2 brightness
     
    anim_pos++;
     anim_pos = anim_pos++ % 128;
    delay(50);
    break;
    
       case 1: // Spinny wheels (6 LEDs on at a time)
  // ======================================================
    
    ring_pos = anim_pos;
    for(i=0; i<RINGSIZE; i++) {
      uint32_t c = 0; // turn off non-selected pixels
     //if(((ring_pos + i) & 7) < 2) c = color; // 4 pixels on...
     if (RingDelta(ring_pos,i)<2)  c = color; 
      pixels.setPixelColor(  NormalizeRingPos(i+leftOff), c); // First eye
      pixels.setPixelColor(RINGSIZE + NormalizeRingPos(RINGSIZE-i+rightOff)  , c); // Second eye (flipped)
    }
    pixels.show();
     anim_pos++;
     anim_pos = anim_pos++ % RINGSIZE;
    delay(300);
   
    break;
    
  
  }

}


void reset_idle(){

  // uses some globals:
  //   color
  //   anim_mode
  //   ring_pos
  
  
  anim_pos = 0;
  

  anim_time = millis();
  ring_pos = 0;
  anim_mode = random(2); // 0 or 1  
 
  ClearRings();
  /*
  if (anim_mode == 0){
    color =random_saturated_color();
  }*/
 switch(anim_mode) {
    case 0: //pulse
      color  = 0xFF0000;
    break;
    
    default:
     color = random_color();
   }

}



uint32_t random_color(){
  return  pixels.Color(     SteppedColor(), SteppedColor(), SteppedColor() ); 

}

uint32_t random_saturated_color(){
  return  pixels.Color(   random(2)*256, random(2)*256, random(2)*256 ); 

}

 uint8_t SteppedColor(){
   // return a non-continuous value for a color axis
   return random(4)*64;
 }
 
 
void NextColor(){
        color >>= 8;                 // Next color R->G->B
      if(!color) color = 0xFF0000; // Reset to red
}

uint8_t NormalizeRingPos(uint8_t realPos){
  
  while (realPos < 0) { realPos += RINGSIZE;}
  while (realPos > RINGSIZE-1) { realPos -= RINGSIZE; }  
  return realPos;
}

void SetMirroredPixels(uint32_t clr, byte pos){
 pixels.setPixelColor(    NormalizeRingPos(pos+leftOff )  , clr); // First eye
      pixels.setPixelColor( RINGSIZE +NormalizeRingPos(pos+rightOff) , clr); // Second eye (not flipped)
      }
      
void ClearRings(){
    SolidRing(0);
}

void SolidRing(uint32_t c){
  byte i;
    for(i=0; i<32; i++) pixels.setPixelColor(i, c);
    pixels.show();
}


int8_t RingDelta(uint8_t pointA, uint8_t point_B){
  int8_t delta = abs(pointA - point_B);
  if (delta > RINGSIZE/2){
      delta = RINGSIZE -  delta;
  }
  return delta;
  
}
