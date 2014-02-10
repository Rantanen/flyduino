
#ifndef _STATUS_H_
#define _STATUS_H_

#undef UINT_MAX
#include "common.h"

#ifdef NODE_JS

#include <v8.h>
#include <node.h>

using namespace node;
using namespace v8;

#ifdef _MSC_VER
#pragma pack(1)
#endif

#define SET_NUMBER( name, value ) \
	Set( String::NewSymbol( name ), Number::New( value ) )
#define SET_FIELD( field ) \
	Set( String::NewSymbol( #field ), Number::New( field ) )

class Quaternion {
	public:
		float w;
		float x;
		float y;
		float z;
};

#else

#include <helper_3dmath.h>

#endif

struct ChannelData
{
	uint16_t raw;
	float value;
};

struct Status_
{
	uint8_t engines;
	uint16_t enginePower[ MAX_ENGINES ];

	uint8_t channels;
	ChannelData channelData[ MAX_CHANNELS ];

	Quaternion orientation;

	float controlYaw;
	float controlPitch;
	float controlRoll;

	float rateYaw;
	float ratePitch;
	float rateRoll;

#ifdef NODE_JS

	Handle<Object> toObject()
	{
		HandleScope scope;

		Local<Object> status = Object::New();

		// Simple values
		status->SET_FIELD( engines );
		status->SET_FIELD( channels );

		status->SET_FIELD( controlYaw );
		status->SET_FIELD( controlPitch );
		status->SET_FIELD( controlRoll );

		status->SET_FIELD( rateYaw );
		status->SET_FIELD( ratePitch );
		status->SET_FIELD( rateRoll );

		// Create the engine power array
		Local<Array> arrEnginePower = Array::New();
		for( int i = 0; i < engines; i++ )
			arrEnginePower->Set( i, Number::New( enginePower[ i ] ) );

		// Crete the channel data array
		Local<Array> arrChannelData = Array::New();
		for( int i = 0; i < channels; i++ )
		{
			// Create the channel data JS object
			Local<Object> channel = Object::New();
			channel->SET_NUMBER( "raw", channelData[ i ].raw );
			channel->SET_NUMBER( "value", channelData[ i ].value );

			arrChannelData->Set( i, channel );
		}

		// Save the arrays
		status->Set( String::NewSymbol( "enginePower" ), arrEnginePower );
		status->Set( String::NewSymbol( "channelData" ), arrChannelData );

		// Save the orientation quaternion
		Local<Object> quat = Object::New();
		quat->SET_NUMBER( "w", orientation.w );
		quat->SET_NUMBER( "x", orientation.x );
		quat->SET_NUMBER( "y", orientation.y );
		quat->SET_NUMBER( "z", orientation.z );
		status->Set( String::NewSymbol( "orientation"), quat );

		return scope.Close( status );
	}

#endif

};

extern Status_ Status;

#endif
