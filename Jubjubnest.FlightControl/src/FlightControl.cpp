
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

#ifdef DIAGNOSTICS
Diagnostics diag( DIAGNOSTIC_OUTPUT_RATE );
#endif

Engine engines[4] = {
	Engine(5),  //  1, 1
	Engine(6),  //  1,-1
	Engine(9),  // -1,-1
	Engine(10)  // -1, 1
};

#define STOP_ERROR( msg ) { ERROR_F( msg ); while( true ); }

void setup() {
	// Init serial and wait for it to be up
	// (Required for Leonardo)
	Serial.begin( SERIAL_RATE );
	while( !Serial );

	pinMode( LED_PIN, OUTPUT );
	pinMode( 2, INPUT );

	digitalWrite( LED_PIN, LOW );

	Radio.addChannel( 0, -0.5 );
	Radio.addChannel( 1, -0.5 );
	Radio.addChannel( 2, 0 );
	Radio.addChannel( 3, -0.5 );
	Radio.addChannel( 4, -0.5 );

	// Setup the engines first to reset the channels for ECM
	for( int i = 0; i < 4; i++ )
	{
		FlightModel.addEngine( &engines[i] );
		if( !engines[i].setup() )
			STOP_ERROR( "Failed to initialize engine");
	}

	if( !PinStatus::setup() )
		STOP_ERROR( "Failed to initialize pin change interrupts");

	if( !IMU.setup() || !IMU.setupInterrupt() )
		STOP_ERROR( "Failed to initialize IMU");

	// Wait for the TX to give valid values
	while( !Radio.update() ||
			Radio.channels[4]->raw < 750 ||
			Radio.channels[4]->raw > 2250 );

	// If elevator is pulled down when the thing is reset, enter calibration mode.
	bool calibrationSaved = false;
	if( Radio.channels[1]->raw < 1300 )
	{
		unsigned long canExitCalibration = millis() + 4000;
		
		while( millis() < canExitCalibration )
		{
			blink( 100 );
			Radio.update();
			Radio.calibrate();

			// When the elevator isn't centered, postpone the calibration ending.
			if( Radio.channels[1]->raw < 1400 || Radio.channels[1]->raw > 1600 )
				canExitCalibration = millis() + 4000;
		}

		// Now allow for 1 second for the user to make a choice whether to save values or not.
		delay( 1000 );

		Radio.update();
		if( Radio.channels[1]->raw > 1700 )
		{
			// Elevator was up
			//   -> this calibration was intended and should be saved.
			Radio.calibrationDone();
			INFO( "Calibration saved" );
			calibrationSaved = true;
		}
	}

	if( !calibrationSaved )
	{
		// Calibration didn't happen or it did but elevator wasn't up
		//   -> Ignore calibration and load old values.
		Radio.loadCalibration();
		INFO( "Calibration loaded" );
	}
}

void loop()
{
	while( Serial.available() > 0 )
	{
		char c = Serial.read();
		switch( c )
		{
			case '1':
				FlightModel.pitchOffset.Kp += 1;
				FlightModel.rollOffset.Kp += 1;
				break;
			case '2':
				FlightModel.pitchOffset.Ki += 0.001;
				FlightModel.rollOffset.Ki += 0.001;
				break;
			case '3':
				FlightModel.pitchOffset.Kd += 1;
				FlightModel.rollOffset.Kd += 1;
				break;
			case 'q':
				FlightModel.pitchOffset.Kp -= 1;
				FlightModel.rollOffset.Kp -= 1;
				break;
			case 'w':
				FlightModel.pitchOffset.Ki -= 0.001;
				FlightModel.rollOffset.Ki -= 0.001;
				break;
			case 'e':
				FlightModel.pitchOffset.Kd -= 1;
				FlightModel.rollOffset.Kd -= 1;
				break;
		}

		Serial.print( "PID:\t" );
		Serial.print( FlightModel.pitchOffset.Kp );
		Serial.print( "\t" );
		Serial.print( FlightModel.pitchOffset.Ki, 4 );
		Serial.print( "\t" );
		Serial.println( FlightModel.pitchOffset.Kd );
	}

	if( Radio.update() )
	{
		FlightModel.updateHeading(
				Radio.channels[3]->value,
				-Radio.channels[1]->value,
				-Radio.channels[0]->value,
				Radio.channels[2]->value,
				Radio.channels[4]->value < 0 );
	}

	if( IMU.readData() )
	{
		FlightModel.updateOrientation( &IMU.orientation );
	}

	FlightModel.update();

#ifdef DIAGNOSTICS
	diag.report( millis() );
#endif
}
