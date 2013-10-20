
#ifndef _RADIO_H_
#define _RADIO_H_

#include "arduino.h"

#define MAX_CHANNELS 7

#include "Channel.h"

/**
 * Radio control for sampling the radio input
 */
class Radio
{
	public:

		/**
		 * Basic constructor
		 *
		 * @param sampleRate Intended sample rate for the radio control
		 */
		Radio( uint8_t sampleRate );

		/**
		 * Destructor
		 */
		~Radio();

		/**
		 * Adds channel specification to the radio control
		 *
		 * @param pin Channel input pin
		 * @param offset Possible offset for the channel
		 */
		void addChannel( int pin, float offset );

		/**
		 * Setup routine for the radio control.
		 *
		 * Performs radio calibration for example.
		 *
		 * @returns True if setup was successsful
		 */
		bool setup();

		/**
		 * Checks whether the radio input should be sampled
		 *
		 * @param currentMillis Current time in milliseconds
		 * @returns True if the radio should be sampled
		 */
		bool shouldSample( unsigned long currentMillis );

		/**
		 * Samples the radio inputs and updates the channel values
		 * 
		 * @param currentMillis Current time in milliseconds
		 */
		void sample( unsigned long currentMillis );

		/**
		 * Channels controlled by this radio
		 */
		Channel* channels[ MAX_CHANNELS ];
	private:

		/**
		 * Private non-default constructor to prevent default destructor
		 */
		Radio() {}

		//! Intended sample rate
		uint8_t sampleRate;

		//! Interval between samples based on the sample rate
		uint8_t interval;

		//! Current channel count
		uint8_t channelCount;

		//! Time treshold for the next sample
		unsigned long nextSample;

};

#endif // _RADIO_H_
