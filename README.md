# GrayCode Arduino Library
This library is used to read encoder positions on 1 or more Absolute Encoder Boards. Hardware files are located [HERE](https://github.com/biomurph/Absolute_Encoder_Board).

## Overview
The Absolute Encoder Board has 2 Gray code encoders on board. The library expects to interface with a 74HC165 PISO shift register. The two encoder positions are read into a signle byte, S1 in the low nibble and S2 is in the high nibble. You can daisy-chain as many encoder boards as you want to, but this release of the library will cap the number at 100 in order to not have the memory allocation chase itself round and round.

# Usage

## Constructor
Basic:

	GrayCode gray(pinLatch, pinClock, pinSerial);
	
Option:

	GrayCode gray(pinLatch, pinClock, pinSerial, pinInhibit);

Make a GrayCode object by defining the latch pin, clock pin, and serial pin you want to use. Optionally, you can use a pin to inhibit the clock signal if that is something you need. The inhibit pin is pulled low on the PCB, so if you don't need the inhibit feature, you can ignore the pin. 

**NOTE: Low is inhibit, High is hibit**

## Initialize

	bool begin(int* switchType, int numEncoders);
	
Begin takes two parameters. First is a pointer to an array that contains the type of switches you are using, and also the number of encoders that you are using. The initializer will return true if memory allocation for the number of encoders you want is available and false if there is a memory allocation error. 

For example, if you have two Absolute Encoder Boards with two 8 position and 2 16 position encoders, you need to provide an array with the switch types in order, followed by the number of encoders. The encoders should be listed in order from the first board in the daisy-chain: S1, S2, S1, S2, etc.

	#define NUM_ENCODERS 4
	int switchType[NUM_ENCODERS] = {8,8,16,16};
	GrayCode gray(SHIFT_LATCH, SHIFT_CLK, SHIFT_SERIAL);
	
	void setup(){
		if(!gray.begin(switchType, NUM_ENCODERS)){
			Serial.println("Memory allocation failed");
		}
	}

## Read Encoder Positions

	bool checkPositions()
	
Update the `pos` array with a new reading from the shift register(s). 
This function checks the newly read encoder positions agains saved positions from the last reading.
It will return true when the positions have changed, and false if they are the same.
After calling checkPositions(), the `pos` array will contain the newly read positions regardles of change in value.

## Encoder Data

The encoder data is stored in a user accessible array called `pos`. 
Depending on how many encoders you are using, you will be able to access their position values via this array. In the physical world, the encoder closest to the PISO bus that is connected to your Arduino board is S1, and it will be the first value `pos[0]` in the array.

