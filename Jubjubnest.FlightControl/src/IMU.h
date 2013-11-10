
#ifndef _IMU_H_
#define _IMU_H_

#include "arduino.h"
#include <helper_3dmath.h>
#include <I2Cdev.h>
#include <Wire.h>

class MPU6050;

/**
 * Wrapper for the MPU6050 sampling
 */
class IMU
{
	public:
		/**
		 * Constructor
		 */
		IMU();

		/**
		 * Setup routine for the IMU module
		 *
		 * @returns True if the setup was successful
		 */
		bool setup();

		/**
		 * Sets up the interrupt routine
		 */
		bool setupInterrupt();

		/**
		 * Reads the IMU data and updates the orientation
		 *
		 * @returns True if there was new data
		 */
		bool readData();

		//! Current orientation based on the IMU
		Quaternion orientation;

		//! Interrupt flag. Set to true on interrupt.
		bool interruptFlag;

	private:

		//! The wrapped MPU
		MPU6050* mpu;

		//! Bytes in the FIFO buffer
		uint16_t fifoCount;

		//! Size of a single packet from the FIFO
		uint16_t fifoPacketSize;

		//! True if the interrupt handler has been set up for this IMU
		bool hasInterruptHandler;

		//! Stores the time in milliseconds when the FIFO was read last.
		unsigned long lastRead;
};

#endif // _IMU_H_

