
#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <Arduino.h>
#include <Servo.h>
#include "status.h"

class Engine
{
	public:
		/**
		 * Engine constructor
		 *
		 * @param id Engine ID
		 * @param pin The physical PWM pin the engine is attached to
		 */
		Engine( uint8_t id, uint8_t pin );

		/**
		 * Engine setup routine
		 */
		bool setup();

		/**
		 * Set the engine power
		 *
		 * @param power Engine power as 8-bit unsigned integer
		 */
		void setPower( uint16_t power );

		/**
		 * Get the engine power
		 *
		 * @return Engine power
		 */
		uint16_t getPower() { return Status.enginePower[ _id ]; }

		/**
		 * Get the engine pin
		 *
		 * @return Engine pin
		 */
		uint8_t getPin() { return _pin; }


	private:

		//! Engine ID
		uint8_t _id;

		//! Engine pin
		uint8_t _pin;

		//! The servo to which the engine is attached
		Servo _servo;
		
};

#endif

