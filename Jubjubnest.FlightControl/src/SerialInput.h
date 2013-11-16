
#ifndef _SERIALINPUT_H_
#define _SERIALINPUT_H_

#include <Arduino.h>
#include <Servo.h>

class _SerialInput
{
	public:
		_SerialInput();
		void update();
};

extern _SerialInput SerialInput;

#endif

