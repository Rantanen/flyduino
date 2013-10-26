
#ifndef _CHANNEL_H_
#define _CHANNEL_H_

/**
 * A single control channel abstraction.
 *
 * Handles value reading and scaling off a single control channel
 * attached to a digital input pin on the board.
 */
class Channel {

	public:

		/**
		 * Basic constructor.
		 *
		 * @param pin Pin number of the input pin for this channel
		 */
		Channel( uint8_t pin );

		/**
		 * Constructor with an offset.
		 *
		 * Use the offset to define value ranges other than 0..1
		 *
		 * @param pin Pin number of the input pin for this channel
		 * @param offset Offset added to the value when updating it
		 */
		Channel( uint8_t pin, float offset );

		/**
		 * Perform one sample worth of calibration.
		 *
		 * Calibration is used to find the min and max values for the
		 * channel. It should be called several times while the user
		 * is instructed to exercise the channel using the control
		 * mechanism.
		 */
		void calibrate();

		/**
		 * Resets the old calibration data.
		 *
		 * Resets the max/min value information. This can be used
		 * before re-calibrating the channel
		 */
		void resetCalibration();

		//! Reads the input pin and updates the current channel value.
		void update();

		//! Input pin number
		uint8_t pin;

		//! Offset for the value.
		float offset;

		//! Current channel value
		float value;

	  
		//! Calibrated minimum value for the channel input
		unsigned long minValue;

		//! Calibrated maximum value for the channel input
		unsigned long maxValue;

		//! Amount of samples received for calibration.
		unsigned int samples;

		//! Amount of timed out samples during the calibration.
		unsigned int timeouts;
};

#endif // _CHANNEL_H_

