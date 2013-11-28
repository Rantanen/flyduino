
#ifndef _COMMON_H_
#define _COMMON_H_

// Define LIVE to allow engines to respond to throttle input.
#define LIVE

/************************
 * Configuration tweaks
 */

//! Time required for the ESC to recognize 0-position.
#define ENGINE_ZERO_DELAY 5000

//! Maximum amount of engines supported
#define MAX_ENGINES 8
#define MAX_CHANNELS 7

#define MIN_ESC_PWM_WIDTH 1000
#define ESC_PWM_LIMIT 2000
#define MAX_ESC_PWM_WIDTH 4000

#define ENABLE_CALIBRATION

// Throttle tuning values
//
// Engine throttle = constrain(
// 		thrust + yawPIDValue + pitchPIDValue + rollPIDValue,
// 		0, 1000 )
//
// thrust = thrust (MAX_CONTROL_POWER - MIN_CONTROL_POWER) *
// 			orientationScale (0.5 - 1.5)
//
// yawPIDValue, pitchPIDValue, rollPIDValue =
// 		PID_X_RATE_KP * Offset +
// 		Integral( PID_X_RATE_KI * dOffset );
// 		PID_X_RATE_KD * dOffset
//

// PID tuning parameters
// X_X_KP - Proportional Gain
// X_X_KI - Integral gain
// X_X_KD - Derivative gain

#define PID_YAW_OFFSET_KP 3
#define PID_YAW_OFFSET_KI 0
#define PID_YAW_OFFSET_KD 0
#define PID_YAW_OFFSET_LIMIT 10

#define PID_TILT_OFFSET_KP 4
#define PID_TILT_OFFSET_KI 0.000
#define PID_TILT_OFFSET_KD 0
#define PID_TILT_OFFSET_LIMIT 10

#define PID_YAW_RATE_KP 75
#define PID_YAW_RATE_KI 0
#define PID_YAW_RATE_KD 0
#define PID_YAW_RATE_LIMIT 250

#define PID_TILT_RATE_KP 30
#define PID_TILT_RATE_KI 0.000
#define PID_TILT_RATE_KD 150
#define PID_TILT_RATE_LIMIT 250

#define STABLE_MODE

#define ACRO_YAW_GAIN 1
#define ACRO_ROLL_GAIN 5

//! Yaw control gain
#define YAW_CONTROL_GAIN 5

//! Maximum power available to power stick (1000 is maximum power).
// Some reserve should be left for corrective actions at full power and
// angular thrust compensation.
#define MAX_CONTROL_POWER 650
#define MIN_CONTROL_POWER 350

//! Amount of samples averaged for input channels
#define CHANNEL_AVERAGE_SAMPLES 4
#define IMU_GYRO_SAMPLES 4

//! Deadzone for the input channels
#define CHANNEL_DEADZONE 0.01

/************************
 * State change times
 */

#define STATE_CHANGE_TIME_OFF_TO_ARMED 4000
#define STATE_CHANGE_TIME_ARMED_TO_OFF 2000
#define STATE_CHANGE_TIME_OFF_ON_IDLE_THROTTLE 30000

/************************
 * Radio configuration
 */

//! Radio sample rate (Hz)
#define RADIO_SAMPLE_RATE 60
//! Pins to which different radio channels are attached to.
// NOTE: These need to be enabled in PinStatus for pin change
//       interrupts separately.
#define CHN1 4
#define CHN2 11
#define CHN3 7
#define CHN4 8
#define CHN5 3

/**********************
 * Miscellaneous values
 */

#define CHANNEL_EEPROM_START 100
#define UINT_MAX 0xffff


/********************
 * Diagnostic output
 */

//! Status led pin
#define LED_PIN 13

//! Trace information
/**
 * 0 - No error reporting
 * 1 - Fatal errors only
 * 2 - Include Error-level tracing
 * 3 - Include Warning-level tracing
 * 4 - Include Info-level tracing
 * 5 - Include Debug-level tracing
 */
#define ERRORLEVEL 0

//! Sensor diagnostic output
// #define DIAGNOSTICS

#define DIAGNOSTIC_OUTPUT_RATE 16
#define SERIAL_RATE 115200

#endif

