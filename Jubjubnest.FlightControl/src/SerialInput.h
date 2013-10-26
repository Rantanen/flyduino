
#ifndef _SERIALINPUT_H_
#define _SERIALINPUT_H_

#include <Arduino.h>
#include <Servo.h>

class SerialInput
{
	public:
		SerialInput();

		bool setup();
		void update();

	private:
		void processMsg();

		uint8_t buffer[16];
		uint8_t read;

		Servo servo;
};

#endif

