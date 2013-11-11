
#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <Servo.h>

class Engine
{
	public:
		/**
		 * Engine constructor
		 *
		 * @param pin The physical PWM pin the engine is attached to
		 * @param x Engine X coordinate
		 * @param y Engine Y coordinate
		 * @param z Engine Z coordinate
		 */
		Engine( uint8_t pin );

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
		uint8_t getPower() { return _power; }

		/**
		 * Get the engine pin
		 *
		 * @return Engine pin
		 */
		uint8_t getPin() { return _pin; }


	private:

		//! Engine pin
		uint8_t _pin;

		//! Current engine power
		uint8_t _power;


		//! The servo to which the engine is attached
		Servo _servo;
		
};

#endif

