/*
    Absolute Encoder example sketch. 
    Works with the Absolute Encoder Board https://github.com/biomurph/Absolute_Encoder_Board
    Each board has 2 encoders, and works with 8 or 16 position switches.
    Interface is via 74HC165 PISO shift register
    Boards are daisy-chainable

    NUM_ENCODERS should be even. If it's not, the library will add 1

    The constructor has an option to add a SHIFT_INHIBIT pin
    if you want to use that feature of the PISO. 
    The inhibit pin is pulled up on the PCB, so you're not obligated to use it.

    Created by Joel Murphy, Flywheel Lab, Fall 2024
*/

#include "GrayCode.h"

#define NUM_ENCODERS 4  // how many encoders do you want to use?
#define SHIFT_LATCH 3   // pin that loads data into the register(s)
#define SHIFT_CLK 2     // shift register clock pin
#define SHIFT_SERIAL 7  // shift register serial out pin

GrayCode gray(NUM_ENCODERS, SHIFT_LATCH, SHIFT_CLK, SHIFT_SERIAL);

void setup() {
  Serial.begin(115200);
  while(!Serial){}
  delay(500);
  Serial.println("Gray Code Encoder Example");
  // The begin() function will return true if memory is successfully allocated 
  // for the number of encoders that you want to use, and false if not enough memory
  if(!gray.begin()){
    Serial.println("Memory allocation failed");
  }
}

void loop() {
  // The checkPositions() function returns true if any of the encoder positions has changed
  // and false if the encoders are in the same position as last time checked.
  // You can use this feature to only do things when positions change, or
  // you can access the pos array to get encoder positions even if they haven't changed.
  if(gray.checkPositions()){
    for(int i=0; i<NUM_ENCODERS; i++){
      Serial.print(gray.pos[i],HEX);
      Serial.print("\t");
    }
    Serial.println();
  }
  delay(10);
}

