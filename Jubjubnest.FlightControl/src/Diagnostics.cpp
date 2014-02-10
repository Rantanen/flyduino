
#include "Diagnostics.h"
#include "FlightModel.h"
#include "Radio.h"
#include "Status.h"
#include "Encoder.h"
#include "debug.h"


Diagnostics::Diagnostics( uint16_t updateInterval )
	: interval( updateInterval ), nextUpdate( 0 ), engineStep( 0 ), step( 0 )
{
}

void Diagnostics::report( unsigned long ms )
{
	Encoder.WriteSerial( (uint8_t*)&Status, sizeof( Status ) );
}

