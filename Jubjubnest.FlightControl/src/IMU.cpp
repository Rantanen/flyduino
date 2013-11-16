
#include "IMU.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#include <MPU6050_6Axis_MotionApps20.h>
#pragma GCC diagnostic pop

#include "debug.h"

#define UINT16_MAX 0xffff

_IMU IMU;

namespace IMUInterrupt {
	_IMU* registeredIMU = 0;
	void interruptHandler()
	{
		if( registeredIMU != 0 )
		{
			registeredIMU->interruptFlag = true;
		}
	}
}

_IMU::_IMU(): yawRates_i( 0 ), pitchRates_i( 0 ), rollRates_i( 0 ), mpu( new MPU6050() )
{
	for( uint8_t i = 0; i < IMU_GYRO_SAMPLES; i++ )
	{
		yawRates[i] = 0.0;
		pitchRates[i] = 0.0;
		rollRates[i] = 0.0;
	}
}

bool _IMU::setup()
{
	Wire.begin();
	TWBR = 24;

	// Init the MPU
	INFO_F( "Initializing MPU..." );
	mpu->initialize();

	INFO_F( "Testing device connections..." );
	if( !mpu->testConnection() )
	{
		ERROR( "MPU6050 connection failed" );
		return false;
	}

	INFO_F( "MPU6050 connection successful" );

	// Init the DMP
	INFO_F( "Initializing DMP" );
	uint8_t devStatus = mpu->dmpInitialize();

	// Set rough axis offsets
	mpu->setXAccelOffset(-215);
	mpu->setYAccelOffset(-321);
	mpu->setZAccelOffset(2160);
	mpu->setXGyroOffset(39);
	mpu->setYGyroOffset(-40);
	mpu->setZGyroOffset(7);

	// Enable the DMP
	if( devStatus == 0 ) {
		INFO_F( "Enabling DMP..." );
		mpu->setDMPEnabled( true );

		INFO_F( "DMP ready! Waiting first interrupt..." );
		fifoPacketSize = mpu->dmpGetFIFOPacketSize();
	} else {
		ERROR( "DMP initialization failed (code %i)", devStatus );
		return false;
	}

	INFO_F( "MPU initialized!" );
	return true;
}

bool _IMU::setupInterrupt()
{
	if( IMUInterrupt::registeredIMU != 0 ) {
		IMUInterrupt::registeredIMU->hasInterruptHandler = false;
	}

	IMUInterrupt::registeredIMU = this;
	attachInterrupt( 0, IMUInterrupt::interruptHandler, RISING );
	hasInterruptHandler = true;

	return true;
}

bool _IMU::readData()
{
	if( (lastRead != UINT16_MAX) && (millis() > (lastRead + 1000) ) ) {
		lastRead = UINT16_MAX;
		FATAL( "No new IMU data for a second" );
	}

	// Check for overflow
	int8_t result;

	if( !interruptFlag ) { return true; }

	fifoCount = mpu->getFIFOCount( &result );
	if( result < 0 ) return false;

	uint8_t mpuIntStatus = mpu->getIntStatus( &result );
	if( result < 0 ) return false;

	// Give a warning in case the FIFO buffer is too full.
	if( fifoCount > 1024/2 )
		WARN( "FIFO buffer half full" );

	if( (mpuIntStatus & 0x10) || fifoCount == 1024 ){
		result = -1;
		while( result < 0 ) {
			INFO_F("Overflow reset");
			mpu->resetFIFO( &result );
		}

		//ERROR( "FIFO overflow! Last read was %i milliseconds ago.", millis() - lastRead );
		return false;
	}

	// Wait for there to be enough packets.
	if( fifoCount < fifoPacketSize || (mpuIntStatus & 0x02) == 0 ) return true;
	interruptFlag = false;

	// There's enough fifo bytes and the packet should be ready, but we're mis-aligned.
	// In this case FIFO might contain bad data so reset it and ignore it for now.
	if( fifoCount % fifoPacketSize != 0 ) {
		//WARN( "Fifo count not a multiple of %i: %i", fifoPacketSize, fifoCount );
		INFO_F("Unaligned reset");
		mpu->resetFIFO( &result );
		return false;
	}

	// Wohoo! We get to read new data. \o/
	uint8_t fifoBuffer[64];
	while( fifoCount >= fifoPacketSize )
	{
		mpu->getFIFOBytes( fifoBuffer, fifoPacketSize, &result );
		if( result < 0 ) return false;

		fifoCount -= fifoPacketSize;

		// Read orientation
		mpu->dmpGetQuaternion( &orientation, fifoBuffer );

		// Read gyro data
		int32_t gyroData[3];
		mpu->dmpGetGyro( gyroData, fifoBuffer );

		rollRate -= rollRates[ rollRates_i ];
		rollRate += ( rollRates[ rollRates_i++ ] = (gyroData[0] / 32768.0f * 2*3.14159/360) / (float)IMU_GYRO_SAMPLES );
		rollRates_i %= IMU_GYRO_SAMPLES;

		pitchRate -= pitchRates[ pitchRates_i ];
		pitchRate += ( pitchRates[ pitchRates_i++ ] = (gyroData[1] / 32768.0f * 2*3.14159/360) / (float)IMU_GYRO_SAMPLES );
		pitchRates_i %= IMU_GYRO_SAMPLES;

		yawRate -= yawRates[ yawRates_i ];
		yawRate += ( yawRates[ yawRates_i++ ] = (gyroData[2] / 32768.0f * 2*3.14159/360) / (float)IMU_GYRO_SAMPLES );
		yawRates_i %= IMU_GYRO_SAMPLES;
	}

	// Mark the last read for diagnostic purposes.
	// TODO: Remove this in case it seems useless.
	if( lastRead == UINT16_MAX ) {
		INFO_F( "IMU data resumed" );
	}
	lastRead = millis();
	return true;
}





