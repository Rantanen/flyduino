
#include "Radio.h"
#include "IMU.h"
#include "Channel.h"
#include "FlightModel.h"
#include "PinStatus.h"

#include "debug.h"

#define ULONG_MAX 4294967295

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

Radio radio( 30 );
IMU imu;
FlightModel flightModel;

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

	if( !PinStatus::setup() )
		STOP_ERROR( "Failed to initialize pin change interrupts" );

	if( !imu.setup() || !imu.setupInterrupt() )
		STOP_ERROR( "Failed to initialize IMU" );

	if( !radio.setup() )
		STOP_ERROR( "Failed to initialize the radio" );

	while( Serial.available() && Serial.read() );
	INFO( "Setup done. Press any key to continue." );
	while( !Serial.available() ) ;
	while( Serial.available() && Serial.read() );

}

bool blinkState = false;
void loop() {
	TRACE( "Loop" );

	unsigned long startms = millis();
	unsigned long ms = startms;

	while( !imu.hasData() && !radio.shouldSample( ms ) )
	{
		ms = millis();
	}

	if( radio.shouldSample( ms ) )
	{
		radio.sample( ms );
		flightModel.updateHeading(
				radio.channels[0]->value,
				radio.channels[1]->value,
				0.0f );
	}

	if( imu.hasData() )
	{
		imu.readData();
		flightModel.updateOrientation( &imu.orientation );
	}

	blinkState = !blinkState;
	digitalWrite( LED_PIN, blinkState );
}
