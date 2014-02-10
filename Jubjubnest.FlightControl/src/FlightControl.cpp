
#include "debug.h"

#include "common.h"

#include "Radio.h"
#include "IMU.h"
#include "Channel.h"
#include "FlightModel.h"
#include "PinStatus.h"
#include "SerialInput.h"
#include "Engine.h"
#include "FlightState.h"
#include "status.h"

#ifdef DIAGNOSTICS
#include "Diagnostics.h"
#endif

Status_ Status;

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

#ifdef DIAGNOSTICS
Diagnostics diag( DIAGNOSTIC_OUTPUT_RATE );
#endif

Engine engines[4] = {
	Engine(0, 11),  //  1, 1
	Engine(1, 10),  //  1,-1
	Engine(2, 9),  // -1,-1
	Engine(3, 6)  // -1, 1
	/* Old
	Engine(5),  //  1, 1
	Engine(6),  //  1,-1
	Engine(9),  // -1,-1
	Engine(10)  // -1, 1
	*/
};

#define STOP_ERROR( msg ) { ERROR_F( msg ); while( true ); }

void debug()
{
}

void setup() {
	// Init serial and wait for it to be up
	// (Required for Leonardo)
	Serial.begin( SERIAL_RATE );
	while( !Serial );

	pinMode( LED_PIN, OUTPUT );
	pinMode( 2, INPUT );

	digitalWrite( LED_PIN, LOW );

	Radio.addChannel( 0 );
	Radio.addChannel( 1 );
	Radio.addChannel( 2 );
	Radio.addChannel( 3 );
	Radio.addChannel( 4 );
	Radio.addChannel( 5 );

	Serial.println( "Init" );

	// Setup the engines first to reset the channels for ECM
	for( int i = 0; i < 4; i++ )
	{
		FlightModel.addEngine( &engines[i] );
		if( !engines[i].setup() )
			STOP_ERROR( "Failed to initialize engine");
	}

	if( !PinStatus::setup() )
		STOP_ERROR( "Failed to initialize pin change interrupts");

	if( !IMU.setup() || !IMU.setupInterrupt() ) {
		STOP_ERROR( "Failed to initialize IMU" );
	}

	// Wait for the TX to give valid values
	Serial.println( "Waiting radio" );
	while( true )
	{
		if( !Radio.update() ) { continue; }

		for( uint8_t i = 0; i < Radio.channelCount; i++ )
		{
			if( Status.channelData[ i ].raw < 750 || Status.channelData[ i ].raw > 2250 )
			{
				continue;
			}
		}

		break;
	}

	// If elevator is pulled down when the thing is reset, enter calibration mode.
	bool calibrationSaved = false;

#ifdef ENABLE_CALIBRATION

	if( Status.channelData[ 1 ].raw < 1400 )
	{
		INFO( "Calibration started" );

		unsigned long canExitCalibration = millis() + 4000;
		while( millis() < canExitCalibration )
		{
			blink( 100 );
			Radio.update();
			Radio.calibrate();

			// When the elevator isn't centered, postpone the calibration ending.
			if( Status.channelData[ 1 ].raw < 1400 || Status.channelData[ 1 ].raw > 1600 )
				canExitCalibration = millis() + 4000;
		}

		// Sticks are centered. Record the zero values.
		digitalWrite( LED_PIN, HIGH );
		delay( 500 );
		Radio.update();
		Radio.recordCenterPositions();
		digitalWrite( LED_PIN, LOW );

		// Now allow for 1 second for the user to make a choice whether to save values or not.
		delay( 1000 );

		Radio.update();
		if( Status.channelData[ 1 ].raw > 1700 )
		{
			// Elevator was up
			//   -> this calibration was intended and should be saved.
			Radio.saveCalibration();
			INFO( "Calibration saved" );
			calibrationSaved = true;
		}
	}
#endif

	if( !calibrationSaved )
	{
		// Calibration didn't happen or it did but elevator wasn't up
		//   -> Ignore calibration and load old values.
		Radio.loadCalibration();
		INFO( "Calibration loaded" );
	}

	Serial.println( "OK" );
}

void loop()
{
	Radio.update();
	IMU.readData();
	// SerialInput.update();

	FlightState.update();

#ifdef DIAGNOSTICS
	diag.report( millis() );
#endif
}
