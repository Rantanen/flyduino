#include <v8.h>
#include <node.h>
#include <node_buffer.h>
#include "common.h"

#include <string.h>

#define NODE_JS
#include "Status.h"

using namespace node;
using namespace v8;

int32_t _decode(
		const uint8_t* input, const int32_t inputSize,
		uint8_t* output, const int32_t outputSize )
{
	const uint8_t* outputStart = output;

	// We know how many bytes we want.
	const uint8_t* inputEnd = input + inputSize;
	const uint8_t* outputEnd = output + outputSize;

	// Read until we got the bytes we wanted
	while( ( !inputSize || input < inputEnd ) &&
			( !outputSize || output < outputEnd ) )
	{
		// Read the segment size
		uint8_t code = *input++;

		// Read enough values
		for( uint16_t i = 1;
				i < code && ( !outputSize || output < outputEnd ) &&
							( !inputSize || input < inputEnd );
				i++ )
		{
			*output++ = *input++;
		}

		// If the segment size was less than 255 we end in zero.
		// (But only if this wans't the last segment)
		if( code < 0xFF && ( !outputSize || output < outputEnd ) )
			*output++ = 0;
	}

	return (int32_t)( output - outputStart );
}

Handle<Value> decodeStatus( const Arguments& args ) {
	HandleScope scope;

	REQ_OBJ_ARG( 0, packet );

	uint8_t* bytes = reinterpret_cast< uint8_t* >( Buffer::Data( packet ) );

	// Decode the bytes into the status.
	Status_ status;
	_decode( bytes, 0, reinterpret_cast< uint8_t* >( &status ), sizeof( Status_ ) );

	return scope.Close( status.toObject() );
}


Handle<Value> decodeBuffer( const Arguments& args ) {
	HandleScope scope;

	REQ_OBJ_ARG( 0, packet );
	REQ_INT_ARG( 1, length );

	// Reserve buffer based in the length.
	// COBS should have overhead of at most 4% rounded up.
	// Add 1 to round up and 1 as safety to the 104% length.
	int32_t bufferLength = 2 + (int32_t)(length * 1.04);
	uint8_t* buffer = new uint8_t[ bufferLength ];
	uint8_t* bytes = reinterpret_cast< uint8_t* >( Buffer::Data( packet ) );

	// Decode the bytes into the status.
	int32_t decodedLength = _decode( bytes, length, buffer, bufferLength );

	CREATE_BUFFER( out, buffer, decodedLength );

	return scope.Close( out );
}



void NodeInit(Handle<Object> exports)
{
	exports->Set( String::NewSymbol( "decodeStatus" ),
			FunctionTemplate::New( decodeStatus )->GetFunction() );
	exports->Set( String::NewSymbol( "decodeBuffer" ),
			FunctionTemplate::New( decodeBuffer )->GetFunction() );
}

NODE_MODULE(node_flightcontrol, NodeInit)

