

#include "GrayCode.h"

#define NUM_ENCODERS 2
#define SHIFT_IN 7
#define SHIFT_LOAD 3
#define SHIFT_CLK 2

GrayCode gray(NUM_ENCODERS, SHIFT_LOAD, SHIFT_CLK, SHIFT_IN);

void setup() {
  Serial.begin(115200);
  while(!Serial){}
  delay(500);
  Serial.println("Gray Code Encoder Example");
  if(!gray.begin()){
    Serial.println("Memory allocation failed");
  }
}

void loop() {

  if(!gray.checkPositions()){
    for(int i=0; i<NUM_ENCODERS; i++){
      Serial.print(gray.pos[i],HEX);
      Serial.print("\t");
    }
    Serial.println();
  }
  delay(10);

}

