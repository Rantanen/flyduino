
#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "arduino.h"

class _Encoder
{
	public:
		void WriteSerial( const uint8_t *ptr, uint16_t length );

#ifdef DECODE
		uint8_t Decode( const uint8_t *ptr, uint8_t* buffer );
#endif
};

extern _Encoder Encoder;

#endif
