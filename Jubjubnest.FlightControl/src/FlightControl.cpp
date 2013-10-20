
#include "Radio.h"
#include "IMU.h"
#include "Channel.h"
#include "FlightModel.h"

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

void setup() {

	radio.addChannel( CHN1, -0.5 );
	radio.addChannel( CHN2, -0.5 );

	if( !imu.setup() ||
			!imu.setupInterrupt() ||
			!radio.setup() )
	{
		PRINT( "Initialization failed." );
		while( true ) ;
	}
	
	pinMode( LED_PIN, OUTPUT );
	pinMode( CHN1, INPUT );
	pinMode( CHN2, INPUT );

	while( Serial.available() && Serial.read() );
	TRACE( "Testing escape sequences" );
	PRINT( "Setup done. Press any key to continue." );
	while( !Serial.available() ) ;
	while( Serial.available() && Serial.read() );

}

bool blinkState = false;
void loop() {

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
