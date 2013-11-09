
#include "Radio.h"
#include "IMU.h"
#include "Channel.h"
#include "FlightModel.h"
#include "PinStatus.h"
#include "SerialInput.h"
#include "Engine.h"
#include "Diagnostics.h"
#include "Quaternion.h"

#include "debug.h"

#define ULONG_MAX 4294967295

#define LED_PIN 13

#define CHN1 4
#define CHN2 11
#define CHN3 7
#define CHN4 8
#define CHN5 3

int freeRam () {
	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

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
	Engine(5, 30, 30, 0),
	Engine(6, 30, -30, 0),
	Engine(9, -30, -30, 0),
	Engine(10, -30, 30, 0)
};

IMU imu;

#define STOP_ERROR( msg ) { ERROR( msg ); while( true ); }

void setup() {

	// Init serial and wait for it to be up
	// (Required for Leonardo)
	Serial.begin( 115200 );
	while( !Serial );

	pinMode( LED_PIN, OUTPUT );
	pinMode( 2, INPUT );

	digitalWrite( LED_PIN, LOW );

	radio.addChannel( CHN1, -0.5 );
	radio.addChannel( CHN2, -0.5 );
	radio.addChannel( CHN3, 0 );
	radio.addChannel( CHN4, -0.5 );
	radio.addChannel( CHN5, -0.5 );
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

	if( !imu.setup() || !imu.setupInterrupt() )
		STOP_ERROR( "Failed to initialize IMU" );

	digitalWrite( LED_PIN, HIGH );
	unsigned long endTime = millis() + 5000;
	while( millis() < endTime )
	{
		radio.calibrate();
		imu.calibrate();
	}
	digitalWrite( LED_PIN, LOW );

	radio.calibrationDone();
	imu.calibrationDone();

	delay( 2000 );
	digitalWrite( LED_PIN, HIGH );
	delay( 500 );
	digitalWrite( LED_PIN, LOW );

}

bool blinkState = false;
void loop()
{
	unsigned long startms = millis();
	unsigned long ms = startms;

	if( radio.sample( ms ) )
	{
		flightModel.updateHeading(
				radio.channels[0]->value,
				radio.channels[1]->value,
				radio.channels[2]->value,
				radio.channels[3]->value );
	}

	if( imu.readData() )
	{
		flightModel.updateOrientation( reinterpret_cast< Quat* >( &imu.orientation ) );
	}

	flightModel.update();

	//serialInput.update();

	diag.report( ms );

	//while( Serial.available() && Serial.read() );

	blinkState = !blinkState;
	//digitalWrite( LED_PIN, blinkState );
}
