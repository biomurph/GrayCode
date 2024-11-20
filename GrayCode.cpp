


#include "GrayCode.h"

/*
    @brief  Constructor
	@note	
*/
GrayCode::GrayCode(int pinLatch, int pinClock, int pinSerial)
{
	_pinLatch = pinLatch;
	_pinClock = pinClock;
	_pinSerial = pinSerial;
	_pinInhibit = -1;

}

/*
    @brief  Constructor Option
	@note	If you have a project that requires use of the inhibit pin
			on the SIPO, you can add it with this constructor option.
			By default, the inhibit pin is pulled low on the PCB
			so you don't need to think about it unless you have to.
*/
GrayCode::GrayCode(int pinLatch, int pinClock, int pinSerial, int pinInhibit)
{
	
	_pinLatch = pinLatch;
	_pinClock = pinClock;
	_pinSerial = pinSerial;
	_pinInhibit = pinInhibit;

	
}

/*
    @brief  Allocate memory for the switch position values.
    		Allocate memory for the switch types and save them.
    		Set pin direction and control for the PISO. 
	@return True if memory allocation is successful.
			False if memory allocation fails.
	@note	There is an internal copy of the position array
			used to determine if any encoders have changed.
			The comparison is done in getEncoderPositions().
			The switch type is used to determine the conversion method.
			If the constructor inclues an inhibit pin, it will 
			be set up. If not, it won't.
			This will cap the number of encoders at 100
			in order to keep you from running away with the memory.
			Change that ceiling as needed.
			If you use an odd number of encoders
			this will add 1 to make the rest of the library make sense
*/

bool GrayCode::begin(int* switchType, int numEncoders)
{
	_numEncoders = numEncoders;
	if(numEncoders > 100){ numEncoders = 100; }
	if(_numEncoders %2 != 0){ // avoid odd numbers at all cost
		_numEncoders++;
	}
	_numBytes = _numEncoders/2;	// used to retrieve the shifted bytes

	bool result = false; // expect the worst
	free(pos);
	free(_pos);
	if( (pos = (uint8_t *)malloc(_numEncoders)) &&      // allocate external position array
	  (_pos = (uint8_t *)malloc(_numEncoders)) &&		// allocate internal position array
	  (_switchType = (uint8_t *)malloc(_numEncoders))){ // allocate switch type array
		memset(pos, 0, _numEncoders); // initialize incoming switch positions to 0x00
		memset(_pos, 0xFF, _numEncoders); // initialize internal switch positions to 0xFF
		for(int i=0; i<_numEncoders; i++){
			_switchType[i] = byte(switchType[i]); // keep track of what type each switch is
		}
		result = true;
	}
	pinMode(_pinLatch,OUTPUT); digitalWrite(_pinLatch,HIGH);
	pinMode(_pinClock,OUTPUT); digitalWrite(_pinClock,LOW);
	pinMode(_pinSerial,INPUT);
	if(_pinInhibit >= 0){
		pinMode(_pinInhibit,OUTPUT); digitalWrite(_pinInhibit, HIGH); // HIGH inhibits, LOW hibits
	}
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
	bool change = false;
	uint8_t b = 0x00;
	int posCounter = 0;
	digitalWrite(_pinLatch,LOW);
	delayMicroseconds(50);
	digitalWrite(_pinLatch,HIGH);
	if(_pinInhibit >= 0){ digitalWrite(_pinInhibit,LOW); }
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
			pos[posCounter] = decodeGray(pos[posCounter],_switchType[posCounter]);
			if(pos[posCounter] != _pos[posCounter]){ change = true; }
			_pos[posCounter] = pos[posCounter];
			posCounter++;
		}
	}
	if(_pinInhibit >= 0){ digitalWrite(_pinInhibit,HIGH); }
	return change;
}

/*
	@brief	Transform Gray code into binary code.
	@param	1 byte with Gray encoded number in the low nibble.
	@return	Binary translation of the Gray code
			0 to 7 for 8 position switches
			0 to F for 16 position switches
*/
uint8_t GrayCode::decodeGray(uint8_t gray, uint8_t type)
{
	uint8_t decimal, mask;

	switch(type){
	case 16:
		mask = decimal = gray;
		while(mask){
			mask >>= 1;
			decimal ^= mask;
		}
		break;
	case 12:
		decimal = lookupTwelve(gray);
		break;
	case 8:
		decimal = lookupEight(gray);
		break;
	default:
		break;
	}	

	return decimal;

}

uint8_t GrayCode::lookupTwelve(uint8_t gray){
	uint8_t decimal = 0x00;
	switch(gray){
		case 0x00:
			decimal = 0x00; break;
		case 0x08:
			decimal = 0x01; break;
		case 0x0C:
			decimal = 0x02; break;
		case 0x04:
			decimal = 0x03; break;
		case 0x06:
			decimal = 0x04; break;
		case 0x0E:
			decimal = 0x05; break;
		case 0x0A:
			decimal = 0x06; break;
		case 0x02:
			decimal = 0x07; break;
		case 0x03:
			decimal = 0x08; break;
		case 0x0B:
			decimal = 0x09; break;
		case 0x09:
			decimal = 0x0A; break;
		case 0x01:
			decimal = 0x0B; break;
		default:
			break;
	}
	return decimal;
}

uint8_t GrayCode::lookupEight(uint8_t gray){
	uint8_t decimal = 0x00;
	switch(gray){
		case 0x00:
			decimal = 0x00; break;
		case 0x04:
			decimal = 0x01; break;
		case 0x0C:
			decimal = 0x02; break;
		case 0x0E:
			decimal = 0x03; break;
		case 0x06:
			decimal = 0x04; break;
		case 0x07:
			decimal = 0x05; break;
		case 0x03:
			decimal = 0x06; break;
		case 0x02:
			decimal = 0x07; break;
		default:
			break;
	}
	return decimal;
}
	
