
#include "IMU.h"
#include <MPU6050_6Axis_MotionApps20.h>

#include "debug.h"

namespace IMUInterrupt {
	IMU* registeredIMU = 0;
	void interruptHandler()
	{
		if( registeredIMU != 0 )
		{
			registeredIMU->interruptFlag = true;
		}
	}
}

IMU::IMU(): mpu( new MPU6050() )
{
}

bool IMU::setup()
{
	Wire.begin();
	TWBR = 24;

	// Init the MPU
	INFO( "Initializing MPU..." );
	mpu->initialize();

	INFO( "Testing device connections..." );
	if( !mpu->testConnection() )
	{
		ERROR( "MPU6050 connection failed" );
		return false;
	}

	INFO( "MPU6050 connection successful" );

	// Init the DMP
	INFO( "Initializing DMP" );
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
		INFO( "Enabling DMP..." );
		mpu->setDMPEnabled( true );

		INFO( "DMP ready! Waiting first interrupt..." );
		fifoPacketSize = mpu->dmpGetFIFOPacketSize();
	} else {
		ERROR( "DMP initialization failed (code %i)", devStatus );
		return false;
	}

	return true;
}

bool IMU::setupInterrupt()
{
	if( IMUInterrupt::registeredIMU != 0 ) {
		IMUInterrupt::registeredIMU->hasInterruptHandler = false;
	}

	IMUInterrupt::registeredIMU = this;
	attachInterrupt( 0, IMUInterrupt::interruptHandler, RISING );
	hasInterruptHandler = true;

	return true;
}

bool IMU::hasData()
{
	if( !interruptFlag ) return false;

	fifoCount = mpu->getFIFOCount();
	if( fifoCount > 1024/2 )
		WARN( "FIFO buffer half full" );

	return fifoCount >= fifoPacketSize;
}

void IMU::readData()
{
	interruptFlag = false;

	// Check for overflow
	if( (mpuIntStatus & 0x10) || fifoCount == 1024 ) {
		mpu->resetFIFO();
		ERROR( "FIFO overflow! Last read was %i milliseconds ago.", millis() - lastRead );
		return;
	}

	// Wait for there to be enough packets.
	if( fifoCount < fifoPacketSize || (mpuIntStatus & 0x02) == 0 ) return false;
	interruptFlag = false;

	// There's enough fifo bytes and the packet should be ready, but we're mis-aligned.
	// In this case FIFO might contain bad data so reset it and ignore it for now.
	if( fifoCount % fifoPacketSize != 0 ) {
		WARN( "Fifo count not a multiple of %i: %i", fifoPacketSize, fifoCount );
		mpu->resetFIFO();
		return false;
	}

	// Wohoo! We get to read new data. \o/
	uint8_t fifoBuffer[64];
	while( fifoCount >= fifoPacketSize )
	{
		mpu->getFIFOBytes( fifoBuffer, fifoPacketSize );
		fifoCount -= fifoPacketSize;

		mpu->dmpGetQuaternion( &orientation, fifoBuffer );
	}

	// Mark the last read for diagnostic purposes.
	// TODO: Remove this in case it seems useless.
	lastRead = millis();
}





