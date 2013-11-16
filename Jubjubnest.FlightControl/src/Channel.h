
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
		 * Constructor
		 *
		 * @param pin Pin number of the input pin for this channel
		 */
		Channel( uint8_t pin );

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
		 * Stores the center spot and calculates the calibration scale
		 */
		void storeCenter();

		/**
		 * Resets the current calibration data in preparation for new
		 * calibration
		 */
		void resetCalibration();

		//! Reads the input pin and updates the current channel value.
		bool update();

		/**
		 * Saves the calibration range to EEPROM
		 *
		 * @param slot Channel slot to save the range to
		 */
		void saveCalibration();

		/**
		 * Loads the calibration range from the EEPROM
		 *
		 * @param slot Channel slot to load the range from
		 */
		void loadCalibration();

		//! Input pin number
		uint8_t pin;

		//! Raw pulse duration in microseconds.
		unsigned int raw;

		//! Current channel value.
		float value;

		struct CalibrationData
		{
			uint16_t minValue;
			uint16_t maxValue;
			uint16_t center;
			float scale;
		} calibrationData;
};

#endif // _CHANNEL_H_

