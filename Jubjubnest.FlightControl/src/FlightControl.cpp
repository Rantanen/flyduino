
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

Radio radio;
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

	radio.addChannel( 0, -0.5 );
	radio.addChannel( 1, -0.5 );
	radio.addChannel( 2, 0 );
	radio.addChannel( 3, -0.5 );
	radio.addChannel( 4, -0.5 );

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
	unsigned long enginesZeroed = millis() + ENGINE_ZERO_DELAY;

	if( !PinStatus::setup() )
		STOP_ERROR( "Failed to initialize pin change interrupts");

	if( !imu.setup() || !imu.setupInterrupt() )
		STOP_ERROR( "Failed to initialize IMU");

	// Wait for the TX to give valid values
	while( !radio.update() ||
			radio.channels[4]->raw < 750 ||
			radio.channels[4]->raw > 2250 );

	// If engines are turned on during startup,
	// wait until engines are turned off and use
	// this time to calibrate radio.
	bool calibrated = false;

	// Start with fast blinking
	unsigned long calibrationStart = millis();
	unsigned int blinkDelay = 100;

	digitalWrite( LED_PIN, HIGH );
	while( millis() < enginesZeroed || radio.channels[4]->raw < 1500 )
	{
		if( radio.update() && radio.channels[4]->raw < 1500 )
		{
			if( radio.channels[4]->raw > 0 )
			{
				calibrated = true;
				radio.calibrate();
			}
			radio.update();

		}

		// Once the engines have been zeroed,
		// continue with slow blinking.
		if( enginesZeroed < millis() )
			blinkDelay = 250;

		// Mark activity
		digitalWrite( LED_PIN, ( (millis() - calibrationStart) / blinkDelay ) % 2 == 0 );
		radio.update();
	}

	// If we calibrated, check whether elevator is turned up.
	if( calibrated && radio.channels[1]->raw > 1750 )
	{
		// Elevator was up
		//   -> this calibration was intended and should be saved.
		//radio.calibrationDone();
		INFO( "Calibration saved" );
	}
	else
	{
		// Calibration didn't happen or it did but elevator wasn't up
		//   -> Ignore calibration and load old values.
		radio.loadCalibration();
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
				flightModel.pitchOffset.Kp += 1;
				flightModel.rollOffset.Kp += 1;
				break;
			case '2':
				flightModel.pitchOffset.Ki += 0.001;
				flightModel.rollOffset.Ki += 0.001;
				break;
			case '3':
				flightModel.pitchOffset.Kd += 1;
				flightModel.rollOffset.Kd += 1;
				break;
			case 'q':
				flightModel.pitchOffset.Kp -= 1;
				flightModel.rollOffset.Kp -= 1;
				break;
			case 'w':
				flightModel.pitchOffset.Ki -= 0.001;
				flightModel.rollOffset.Ki -= 0.001;
				break;
			case 'e':
				flightModel.pitchOffset.Kd -= 1;
				flightModel.rollOffset.Kd -= 1;
				break;
		}

		Serial.print( "PID:\t" );
		Serial.print( flightModel.pitchOffset.Kp );
		Serial.print( "\t" );
		Serial.print( flightModel.pitchOffset.Ki, 4 );
		Serial.print( "\t" );
		Serial.println( flightModel.pitchOffset.Kd );
	}

	if( radio.update() )
	{
		flightModel.updateHeading(
				radio.channels[3]->value,
				-radio.channels[1]->value,
				-radio.channels[0]->value,
				radio.channels[2]->value,
				radio.channels[4]->value < 0 );
	}

	if( imu.readData() )
	{
		flightModel.updateOrientation( &imu.orientation );
	}

	flightModel.update();

#ifdef DIAGNOSTICS
	diag.report( millis() );
#endif
}
