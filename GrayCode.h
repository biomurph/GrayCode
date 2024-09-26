/*
	GrayCode.h - Library for reading absolute rotary encoder switch positions
	This library is meant to work with the Absolute Encoder Board from Flywheel Lab
	https://github.com/biomurph/Absolute_Encoder

	The Absolute Encoder Board uses 2 PAC18R 16 position rotary swich encoders,
	each with a 4 pin output interfaced through a 74HC165 PISO shift register.
	These encoders use a 4 bit scheme called Gray Code for each one of the 16 positions. 

	The Absolute Encoder Boards can be daisy-chained and the library will shift out
	all of the encoder positions. 

	Created by Joel Murphy, Fall 2024
	
*/

#ifndef GRAY_CODE_H
#define GRAY_CODE_H

#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>


class GrayCode
{

public:
	GrayCode(int numEncoders, int pinLatch, int pinClock, int pinSerial); 
	GrayCode(int numEncoders, int pinLatch, int pinClock, int pinSerial, int pinInhibit); 
	bool begin();
	bool checkPositions();
	uint8_t *pos;

private:
	int _numEncoders;
	int _pinLatch;
	int _pinClock;
	int _pinSerial;
	int _pinInhibit;
	int _numBytes;
	uint8_t *_pos;
	uint8_t decodeGray(uint8_t g);
};




#endif