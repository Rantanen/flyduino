
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

	// Init serial and wait for it to be up
	// (Required for Leonardo)
	Serial.begin( 115200 );
	while( !Serial );

	// Init the MPU
	PRINT( "Initializing MPU..." );
	mpu->initialize();

	PRINT( "Testing device connections..." );
	if( !mpu->testConnection() )
	{
		PRINT( "MPU6050 connection failed" );
		return false;
	}

	PRINT( "MPU6050 connection successful" );

	// Init the DMP
	PRINT( "Initializing DMP" );
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
		PRINT( "Enabling DMP..." );
		mpu->setDMPEnabled( true );

		PRINT( "DMP ready! Waiting first interrupt..." );
		fifoPacketSize = mpu->dmpGetFIFOPacketSize();
	} else {
		PRINT( "DMP initialization failed (code %i)", devStatus );
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

	uint8_t mpuIntStatus = mpu->getIntStatus();
	if( (mpuIntStatus & 0x10) || fifoCount == 1024 ) {
		mpu->resetFIFO();
		PRINT( "FIFO overflow!" );
		return;
	}

	if( mpuIntStatus & 0x20 == 0 ) return;

	uint8_t fifoBuffer[64];
	while( fifoCount >= fifoPacketSize )
	{
		mpu->getFIFOBytes( fifoBuffer, fifoPacketSize );
		fifoCount -= fifoPacketSize;

		mpu->dmpGetQuaternion( &orientation, fifoBuffer );
	}
}





