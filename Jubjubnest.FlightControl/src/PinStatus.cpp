
#include "Arduino.h"
#include "PinStatus.h"
#include "debug.h"

uint8_t previousPins0 = 0;
uint8_t previousPins1 = 0;
unsigned long highStart[16];
unsigned int pinValues[16];

bool PinStatus::setup()
{
	PCICR |= ( 1 << PCIE0 );
	PCICR |= ( 1 << PCIE1 );

	PCMSK0 |= ( 1 << PCINT0 );
	PCMSK0 |= ( 1 << PCINT1 );

	// Set MCUCR to 01 (Trigger on any change)
	MCUCR &= ~( 1 << ISC00 );
	MCUCR |= ( 1 << ISC01 );
	MCUCR = (1 << ISC01) | (1 << ISC00);

	return true;
}

unsigned int PinStatus::getValue( uint8_t pin )
{
	return pinValues[ pin ];
}


#define LED_PIN 13
inline void checkPin( uint8_t changedPins, uint8_t currentPins,
		uint8_t physicalPin, uint8_t arduinoPin,
		unsigned long us )
{
	if( (changedPins & (1 << physicalPin)) )
	{
		if( (currentPins & (1 << physicalPin)) )
		{
			// Current pin just went up
			highStart[ arduinoPin ] = us;
		}
		else
		{
			// Current pin just went down.
			pinValues[ arduinoPin ] = us - highStart[ arduinoPin ];
		}
	}
}

ISR( PCINT0_vect )
{
	uint8_t pins = PINB;
	unsigned long us = micros();

	// We got the important bits. Enable interrupt.
	sei();


	uint8_t changedPins = previousPins0 ^ pins;
	checkPin( changedPins, pins, 0, 8, us );
	checkPin( changedPins, pins, 1, 9, us );
	checkPin( changedPins, pins, 2, 10, us );
	checkPin( changedPins, pins, 3, 11, us );
	checkPin( changedPins, pins, 4, 12, us );
	checkPin( changedPins, pins, 5, 13, us );
	previousPins0 = pins;
}

ISR( PCINT1_vect )
{
	uint8_t pins = PIND;
	unsigned long us = micros();

	// We got the important bits. Enable interrupt.
	sei();


	uint8_t changedPins = previousPins1 ^ pins;
	checkPin( changedPins, pins, 0, 0, us );
	checkPin( changedPins, pins, 1, 1, us );
	checkPin( changedPins, pins, 2, 2, us );
	checkPin( changedPins, pins, 3, 3, us );
	checkPin( changedPins, pins, 4, 4, us );
	checkPin( changedPins, pins, 5, 5, us );
	checkPin( changedPins, pins, 6, 6, us );
	checkPin( changedPins, pins, 7, 7, us );
	previousPins1 = pins;
}

