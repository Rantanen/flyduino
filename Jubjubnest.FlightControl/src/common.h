
#ifndef _COMMON_H_
#define _COMMON_H_

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

// PID tuning parameters
// X_X_KP - Proportional Gain
// X_X_KI - Integral gain
// X_X_KD - Derivative gain

#define PID_YAW_KP 50
#define PID_YAW_KI 0
#define PID_YAW_KD 0

#define PID_PITCH_KP 120
#define PID_PITCH_KI 0.002
#define PID_PITCH_KD 0 //300

#define PID_ROLL_KP 120
#define PID_ROLL_KI 0.002
#define PID_ROLL_KD 0 //300

//! Yaw control gain
#define YAW_CONTROL_GAIN 5

//! Maximum power available to power stick (255 is maximum power).
// Some reserve should be left for corrective actions at full power.
#define MAX_CONTROL_POWER 750

#define MIN_CONTROL_POWER 250

//! Amount of samples averaged for input channels
#define CHANNEL_AVERAGE_SAMPLES 4

//! Deadzone for the input channels
#define CHANNEL_DEADZONE 0.01

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
#define ERRORLEVEL 3

//! Sensor diagnostic output
// #define DIAGNOSTICS

#define DIAGNOSTIC_OUTPUT_RATE 16
#define SERIAL_RATE 115200

#endif

