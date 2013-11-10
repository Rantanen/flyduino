
#include "debug.h"

#include "common.h"

#include "Radio.h"
#include "IMU.h"
#include "Channel.h"
#include "FlightModel.h"
#include "PinStatus.h"
#include "SerialInput.h"
#include "Engine.h"

#ifdef DIAGNOSTICS
#include "Diagnostics.h"
#endif

//#define CALIBRATION

int main(void)
{
	init();

	setup();
    
	for (;;) {
		loop();
		if (serialEventRun) serialEventRun();
	}
        
	return 0;
}

unsigned long nextUpdate = 0;

#ifdef DIAGNOSTICS
Diagnostics diag( DIAGNOSTIC_OUTPUT_RATE );
#endif

Radio radio( RADIO_SAMPLE_RATE );
FlightModel flightModel;
Engine engines[4] = {
	Engine(5),  //  1, 1
	Engine(6),  //  1,-1
	Engine(9),  // -1,-1
	Engine(10)  // -1, 1
};

IMU imu;

#define STOP_ERROR( msg ) { ERROR_F( msg ); while( true ); }

void setup() {
	// Init serial and wait for it to be up
	// (Required for Leonardo)
	Serial.begin( SERIAL_RATE );
	while( !Serial );

	pinMode( LED_PIN, OUTPUT );
	pinMode( 2, INPUT );

	digitalWrite( LED_PIN, LOW );

	radio.addChannel( CHN1, -0.5 );
	radio.addChannel( CHN2, -0.5 );
	radio.addChannel( CHN3, 0 );
	radio.addChannel( CHN4, -0.5 );
	radio.addChannel( CHN5, -0.5 );

#ifdef DIAGNOSTICS
	diag.setFlightModel( &flightModel );
#endif

	// Setup the engines first to reset the channels for ECM
	for( int i = 0; i < 4; i++ )
	{
		flightModel.addEngine( &engines[i] );
		if( !engines[i].setup() )
			STOP_ERROR( "Failed to initialize engine");
	}
	unsigned long enginesZeroed = millis();

	if( !PinStatus::setup() )
		STOP_ERROR( "Failed to initialize pin change interrupts");

	if( !imu.setup() || !imu.setupInterrupt() )
		STOP_ERROR( "Failed to initialize IMU");

	while( radio.channels[4]->raw < 750 || radio.channels[4]->raw > 2250 )
		radio.sample( 0 );

	bool saved = false;

#ifdef CALIBRATION
	if( radio.channels[4]->raw < 1500  )
	{
		INFO_F( "Calibrating radio" );
		digitalWrite( LED_PIN, HIGH );

		int calibrationDone = 0;
		while( calibrationDone < 100 )
		{
			radio.sample( 0 );

			radio.calibrate();
			if( radio.channels[4]->raw > 1500 )
				calibrationDone++;
			else
				calibrationDone = 0;
		}
		digitalWrite( LED_PIN, LOW );

		if( radio.channels[1]->raw > 1750 )
		{
			INFO_F( "Calibration done" );
			radio.calibrationDone();
			saved = true;
		}
		else
		{
			INFO_F( "Calibration not saved. Elevator was not up" );
		}
	}

#endif
	if( !saved )
	{
		radio.loadCalibration();
	}

	digitalWrite( LED_PIN, HIGH );

	// Delay at least 250 millis to mark the led.
	delay( 250 );

	// Make sure we've delayed enough for the ESC to calibrate.
	long engineDelay = (enginesZeroed + ENGINE_ZERO_DELAY) - millis();
	if( engineDelay > 0 )
		delay( engineDelay );

	digitalWrite( LED_PIN, LOW );
}

void loop()
{
	unsigned long startms = millis();
	unsigned long ms = startms;

	if( radio.sample( ms ) )
	{
		flightModel.updateHeading(
				radio.channels[3]->value,
				-radio.channels[1]->value,
				-radio.channels[0]->value,
				radio.channels[2]->value );
	}

	if( imu.readData() )
	{
		flightModel.updateOrientation( &imu.orientation );
	}

	flightModel.update();

#ifdef DIAGNOSTICS
	diag.report( ms );
#endif
}
