
#include "Arduino.h"
#include "PinStatus.h"
#include "debug.h"
#include "common.h"

uint8_t previousPins0 = 0;
uint8_t previousPins1 = 0;
unsigned long highStart[ MAX_CHANNELS ];
unsigned int volatile pinValues[ MAX_CHANNELS ][ CHANNEL_AVERAGE_SAMPLES ];
uint8_t pinBufferIndex[ MAX_CHANNELS ];


#define CHECK_CHANNEL( port, chn_pin, chn_num ) \
	if( chn_pin == 0 && port == 2 ) checkPin( changedPins, pins, 0, chn_num - 1, us ); \
	if( chn_pin == 1 && port == 2 ) checkPin( changedPins, pins, 1, chn_num - 1, us ); \
	if( chn_pin == 2 && port == 2 ) checkPin( changedPins, pins, 2, chn_num - 1, us ); \
	if( chn_pin == 3 && port == 2 ) checkPin( changedPins, pins, 3, chn_num - 1, us ); \
	if( chn_pin == 4 && port == 2 ) checkPin( changedPins, pins, 4, chn_num - 1, us ); \
	if( chn_pin == 5 && port == 2 ) checkPin( changedPins, pins, 5, chn_num - 1, us ); \
	if( chn_pin == 6 && port == 2 ) checkPin( changedPins, pins, 6, chn_num - 1, us ); \
	if( chn_pin == 7 && port == 2 ) checkPin( changedPins, pins, 7, chn_num - 1, us ); \
	if( chn_pin == 8 && port == 0 ) checkPin( changedPins, pins, 0, chn_num - 1, us ); \
	if( chn_pin == 9 && port == 0 ) checkPin( changedPins, pins, 1, chn_num - 1, us ); \
	if( chn_pin == 10 && port == 0 ) checkPin( changedPins, pins, 2, chn_num - 1, us ); \
	if( chn_pin == 11 && port == 0 ) checkPin( changedPins, pins, 3, chn_num - 1, us ); \
	if( chn_pin == 12 && port == 0 ) checkPin( changedPins, pins, 4, chn_num - 1, us ); \
	if( chn_pin == 13 && port == 0 ) checkPin( changedPins, pins, 5, chn_num - 1, us );

#ifndef CHN1
#define CHN1 -1
#endif

#ifndef CHN2
#define CHN2 -1
#endif

#ifndef CHN3
#define CHN3 -1
#endif

#ifndef CHN4
#define CHN4 -1
#endif

#ifndef CHN5
#define CHN5 -1
#endif

#ifndef CHN6
#define CHN6 -1
#endif

#ifndef CHN7
#define CHN7 -1
#endif

#ifndef CHN8
#define CHN8 -1
#endif

// 3, 4, 7, 8, 11

bool PinStatus::setup()
{
	// Enable the interrupts
	PCICR |= ( 1 << PCIE0 );
	PCICR |= ( 1 << PCIE2 );

	PCMSK0 |= ( 1 << PCINT0 ); // 8
	PCMSK0 |= ( 1 << PCINT3 ); // 11

	PCMSK2 |= ( 1 << PCINT19 ); // 3
	PCMSK2 |= ( 1 << PCINT20 ); // 4
	PCMSK2 |= ( 1 << PCINT23 ); // 7

	// Set MCUCR to 01 (Trigger on any change)
	MCUCR &= ~( 1 << ISC00 );
	MCUCR |= ( 1 << ISC01 );
	MCUCR = (1 << ISC01) | (1 << ISC00);

	return true;
}

unsigned int PinStatus::getValue( uint8_t channel )
{
	unsigned long duration = 0;
	uint8_t goodValues = 0;

	for( uint8_t i = 0; i < CHANNEL_AVERAGE_SAMPLES; i++ )
	{
		unsigned long value = pinValues[ channel ][ i ];

		// If this is a bad value, ignore it.
		if( value < 750 || value > 2250 )
			continue;

		// Otherwise add it to the sum.
		goodValues++;
		duration += value;
	}

	// If there weren't enough good values, return 0.
	if( goodValues < (CHANNEL_AVERAGE_SAMPLES / 2) )
		return 0;

	return duration / goodValues;
}


#define LED_PIN 13
inline void checkPin( uint8_t changedPins, uint8_t currentPins,
		uint8_t physicalPin, uint8_t channel,
		unsigned long us )
{
	if( (changedPins & (1 << physicalPin)) )
	{
		if( (currentPins & (1 << physicalPin)) )
		{
			// Current pin just went up
			highStart[ channel ] = us;
		}
		else
		{
			// Current pin just went down.
			pinValues[ channel ][ pinBufferIndex[ channel ]++ ] = us - highStart[ channel ];
			pinBufferIndex[ channel ] %= CHANNEL_AVERAGE_SAMPLES;
		}
	}
}

ISR( PCINT0_vect )
{
	uint8_t pins = PINB;
	unsigned long us = micros();

	// We got the important bits. Enable interrupt.
	sei();

#define changedPins (previousPins0 ^ pins)

	CHECK_CHANNEL( 0, CHN1, 1 );
	CHECK_CHANNEL( 0, CHN2, 2 );
	CHECK_CHANNEL( 0, CHN3, 3 );
	CHECK_CHANNEL( 0, CHN4, 4 );
	CHECK_CHANNEL( 0, CHN5, 5 );
	CHECK_CHANNEL( 0, CHN6, 6 );
	CHECK_CHANNEL( 0, CHN7, 7 );
	CHECK_CHANNEL( 0, CHN8, 8 );
#undef changedPins

	previousPins0 = pins;

}

ISR( PCINT2_vect )
{
	uint8_t pins = PIND;
	unsigned long us = micros();

	// We got the important bits. Enable interrupt.
	sei();

#define changedPins (previousPins1 ^ pins)

	CHECK_CHANNEL( 2, CHN1, 1 );
	CHECK_CHANNEL( 2, CHN2, 2 );
	CHECK_CHANNEL( 2, CHN3, 3 );
	CHECK_CHANNEL( 2, CHN4, 4 );
	CHECK_CHANNEL( 2, CHN5, 5 );
	CHECK_CHANNEL( 2, CHN6, 6 );
	CHECK_CHANNEL( 2, CHN7, 7 );
	CHECK_CHANNEL( 2, CHN8, 8 );
#undef changedPins

	previousPins1 = pins;

}

