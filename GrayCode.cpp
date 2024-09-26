


#include "GrayCode.h"

/*
    @brief  Constructor
	@note	This will cap the number of encoders at 100
			in order to keep you from running away with the memory.
			Change that ceiling as needed.
			If you use an odd number of encoders
			this will add 1 to make the rest of the library make sense
*/
GrayCode::GrayCode(int numEncoders, int pinLatch, int pinClock, int pinSerial)
{
	_numEncoders = numEncoders;
	_pinLatch = pinLatch;
	_pinClock = pinClock;
	_pinSerial = pinSerial;
	_pinInhibit = -1;

	if(numEncoders > 100){ numEncoders = 100; }
	if(_numEncoders %2 != 0){ // avoid odd numbers at all cost
		_numEncoders++;
	}
}

/*
    @brief  Constructor Option
	@note	If you have a project that requires use of the inhibit pin
			on the SIPO, you can add it with this constructor option.
*/
GrayCode::GrayCode(int numEncoders, int pinLatch, int pinClock, int pinSerial, int pinInhibit)
{
	_numEncoders = numEncoders;
	_pinLatch = pinLatch;
	_pinClock = pinClock;
	_pinSerial = pinSerial;
	_pinInhibit = pinInhibit;

	if(numEncoders > 100){ numEncoders = 100; }
	if(_numEncoders %2 != 0){ // avoid odd numbers at all cost
		_numEncoders++;
	}
}

/*
    @brief  Allocate memory for the switch position values.
    		Set pin direction and control for the PISO. 
	@return True if memory allocation is successful.
			False if memory allocation fails.
	@note	There is an internal copy of the position array
			used to determine if any encoders have changed.
			The comparison is done in getEncoderPositions().
			If the constructor inclues an inhibit pin, it will 
			be set up. If not, it won't.
*/

bool GrayCode::begin()
{
	bool result = false; // expect the worst
	free(pos);
	free(_pos);
	if( (pos = (uint8_t *)malloc(_numEncoders)) && 
	  (_pos = (uint8_t *)malloc(_numEncoders)) ){
		memset(pos, 0, _numEncoders);
		memset(_pos, 0xFF, _numEncoders);
		result = true;
	}
	pinMode(_pinLatch,OUTPUT);
	digitalWrite(_pinLatch,HIGH);
	pinMode(_pinClock,OUTPUT);
	digitalWrite(_pinClock,LOW);
	pinMode(_pinSerial,INPUT);
	if(_pinInhibit > 0){
		pinMode(_pinInhibit,OUTPUT);
		digitalWrite(_pinInhibit, HIGH); // HIGH inhibits, LOW hibits
	}
	_numBytes = _numEncoders/2;
	return result;
}

/*
	@brief	Latch the PISO data and then shift in Gray codes.
			Each incomming byte contains data for 2 encoders.
			Break the data apart and assign values to pos array.
			Then, compare the pos and _pos arrays
	@return	True if any of the encoders has changed.
			False if no encoders have changed.

*/
bool GrayCode::checkPositions()
{
	bool same = false;
	uint8_t b = 0x00;
	int posCounter = 0;
	digitalWrite(_pinLatch,LOW);
	delayMicroseconds(50);
	digitalWrite(_pinLatch,HIGH);
	if(_pinInhibit > 0){ digitalWrite(_pinInhibit,LOW); }
	for(int i=0; i<_numBytes; i++){
		for(int j=7; j>=0; j--){
			bitWrite(b,j,digitalRead(_pinSerial));
			digitalWrite(_pinClock,HIGH);
			delayMicroseconds(50);
			digitalWrite(_pinClock,LOW);
			delayMicroseconds(50);
		}
		for(int k=1; k>=0; k--){
			pos[posCounter] = (b >>(k*4)) & 0x0F;
			pos[posCounter] = decodeGray(pos[posCounter]);
			if(pos[posCounter] != _pos[posCounter]){ same = true; }
			_pos[posCounter] = pos[posCounter];
			posCounter++;
		}
	}
	if(_pinInhibit > 0){ digitalWrite(_pinInhibit,HIGH); }
	return same;
}

/*
	@brief	Transform Gray code into binary code.
	@param	1 byte with Gray encoded number.
	@return	True if any of the encoders has changed.
			False if no encoders have changed.

*/
uint8_t GrayCode::decodeGray(uint8_t g)
{
	uint8_t mask = g;
	while(mask){
		mask >>= 1;
		g ^= mask;
	}
	return g;
}




