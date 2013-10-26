
#include "Radio.h"
#include "IMU.h"
#include "Channel.h"
#include "FlightModel.h"
#include "PinStatus.h"
#include "SerialInput.h"
#include "Engine.h"
#include "Diagnostics.h"

#include "debug.h"

#define ULONG_MAX 4294967295
#define HAS_IMU

#define LED_PIN 13

#define CHN1 8
#define CHN2 9

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

Diagnostics diag( 16 );
Radio radio( 30 );
FlightModel flightModel;
SerialInput serialInput;
Engine engines[4] = {
	Engine(5, 10, 10, 0),
	Engine(6, 10, -10, 0),
	Engine(9, -10, -10, 0),
	Engine(10, -10, 10, 0)
};

#ifdef HAS_IMU
IMU imu;
#endif

#define STOP_ERROR( msg ) { ERROR( msg ); while( true ); }

void setup() {

	// Init serial and wait for it to be up
	// (Required for Leonardo)
	Serial.begin( 115200 );
	while( !Serial );

	pinMode( LED_PIN, OUTPUT );
	pinMode( CHN1, INPUT );
	pinMode( CHN2, INPUT );
	pinMode( 2, INPUT );

	digitalWrite( LED_PIN, LOW );

	radio.addChannel( CHN1, -0.5 );
	radio.addChannel( CHN2, -0.5 );
	diag.setFlightModel( &flightModel );

	for( int i = 0; i < 4; i++ )
	{
		flightModel.addEngine( &engines[i] );
		if( !engines[i].setup() )
			STOP_ERROR( "Failed to initialize engine" );
	}

	if( !PinStatus::setup() )
		STOP_ERROR( "Failed to initialize pin change interrupts" );

	if( !radio.setup() )
		STOP_ERROR( "Failed to initialize the radio" );

	if( !serialInput.setup() )
		STOP_ERROR( "Failed to set up serial input" );

#ifdef HAS_IMU
	if( !imu.setup() || !imu.setupInterrupt() )
		STOP_ERROR( "Failed to initialize IMU" );
#endif

	while( Serial.available() && Serial.read() );
	INFO( "Setup done. Press any key to continue." );
	while( !Serial.available() ) ;
	while( Serial.available() && Serial.read() );

}

bool blinkState = false;
void loop()
{

	unsigned long startms = millis();
	unsigned long ms = startms;

	/*
	if( radio.sample( ms ) )
	{
		flightModel.updateHeading(
				radio.channels[0]->value,
				radio.channels[1]->value,
				0.0f, 0 );
	}
	*/

#ifdef HAS_IMU
	if( imu.readData() )
	{
		flightModel.updateOrientation( &imu.orientation );
	}
#endif

	unsigned long start = micros();
	flightModel.update();
	//INFO( "Update duration:\t%lu", micros() - start );
	serialInput.update();

	diag.report( ms );

	blinkState = !blinkState;
	//digitalWrite( LED_PIN, blinkState );
}
