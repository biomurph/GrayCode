


#include "GrayCode.h"


GrayCode::GrayCode(int numEncoders, int pinLatch, int pinClock, int pinSerial)
{
	_numEncoders = numEncoders;
	_pinLatch = pinLatch;
	_pinClock = pinClock;
	_pinSerial = pinSerial;

	if(numEncoders > 100){ numEncoders = 100; }
	if(_numEncoders %2 != 0){ // avoid odd numbers at all cost
		_numEncoders++;
	}
}

/*
    @brief  Set pin direction and control for the PISO. 
    		Allocate memory for the switch position values.
	@return True if memory allocation is successful.
			False if memory allocation fails.
	@note	There is an internal copy of the position array
			used to determine if any encoders have changed.
			The comparison is done in getEncoderPositions().
*/

bool GrayCode::begin()
{
	bool result = false; // expect the worst
	pinMode(_pinLatch,OUTPUT);
	digitalWrite(_pinLatch,HIGH);
	pinMode(_pinClock,OUTPUT);
	digitalWrite(_pinClock,LOW);
	pinMode(_pinSerial,INPUT);
	_numBytes = _numEncoders/2;
	free(pos);
	if( (pos = (uint8_t *)malloc(_numEncoders)) && 
	  (_pos = (uint8_t *)malloc(_numEncoders)) ){
		memset(pos, 0, _numEncoders);
		memset(_pos, 0xFF, _numEncoders);
		result = true;
	}
	return result;
}


// bool Gray_Encoder::getEncoderPositions()
// {
// 	bool status = false; // expectations low
// 	newCodes = shiftInCodes();
// 	if(newCodes != oldCodes){
// 		status = true;
// 	}
// 	oldCodes = newCodes;
// 	enc1code = newCodes & 0x0F;
// 	enc2code = (newCodes >> 4) & 0x0F;
// 	enc1pos = decodeGray(enc1code);
// 	enc2pos = decodeGray(enc2code);
// 	return status;
// }

/*
	@brief	Latch the PISO data and then shift in Gray codes.
			Each incomming byte contains data for 2 encoders.
			Break the data apart and assign values to pos array.
			Then, compare the pos and _pos arrays
	@return	True if any of the encoders has changed.
			False if no encoders have changed.

*/
bool GrayCode::checkEncoderPositions()
{
	uint8_t b = 0x00;
	// int numBytes = _numEncoders/2;
	int posCounter = 0;
	digitalWrite(_pinLatch,LOW);
	delayMicroseconds(50);
	digitalWrite(_pinLatch,HIGH);
	for(int i=0; i<_numBytes; i++){
		for(int j=7; j>=0; j--){
			bitWrite(b,j,digitalRead(_pinSerial));
			digitalWrite(_pinClock,HIGH);
			delayMicroseconds(50);
			digitalWrite(_pinClock,LOW);
			delayMicroseconds(50);
		}
		for(int k; k=0; k<2){
			pos[posCounter] = (b >>(k*4)) & 0x0F;
			pos[posCounter] = decodeGray(pos[posCounter]);
			posCounter++;
		}
	}
	bool same = true;
	int i=0;
	while(i < _numEncoders && same){
		same = pos[i] == _pos[i];
		i++;
	}
	if(!same){
		memcpy(&_pos, &pos, sizeof(pos));
	}
	return same;
}


uint8_t GrayCode::decodeGray(uint8_t g)
{
	uint8_t mask = g;
	while(mask){
		mask >>= 1;
		g ^= mask;
	}
	return g;
}




