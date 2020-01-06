/**
* header file for the simple sensor board 202
* based on ssimp192.h version 001
* includes pin definitions for both temp sensors (tc74/mcp9800)
*
* wendhack@teco.edu
* Date: 2005-03-08
*
* version: 001
*
*/


#ifndef __SSIMP202_H__
#define	__SSIMP202_H__					001

// pins for pc sensors
#undef PIN_VOLTAGE
#undef ADCH_VOLTAGE
#define PIN_VOLTAGE			PIN_CONN_14
#define ADCH_VOLTAGE		ADCH_CONN_14

// TSOP
#define PIN_TSOP36236TR_POWER			PIN_CONN_07
#define	TRIS_TSOP36236TR_POWER			TRIS_CONN_07

// pin defs for sensors

// accelerator ADXL210
#define PIN_ACCL_ADXL_POWER				PIN_CONN_15
#define PIN_ACCL_ADXL_XOUT				PIN_CONN_05
#define PIN_ACCL_ADXL_YOUT				PIN_CONN_06

#define TRIS_ACCL_ADXL_POWER			TRIS_CONN_15
#define TRIS_ACCL_ADXL_XOUT				TRIS_CONN_05
#define TRIS_ACCL_ADXL_YOUT				TRIS_CONN_06

#if 0
//extra accl
#define PIN_ACCL_ADXL_POWER_Z			PIN_CONN_04
#define PIN_ACCL_ADXL_ZOUT				PIN_CONN_07

#define TRIS_ACCL_ADXL_POWER_Z			TRIS_CONN_04
#define TRIS_ACCL_ADXL_ZOUT				TRIS_CONN_07
#endif
#if 0
// light sensor TSL250
#define PIN_LIGHT_TSL250_POWER			PIN_CONN_08
#define PIN_LIGHT_TSL250_OUT			PIN_CONN_14

#define TRIS_LIGHT_TSL250_POWER			TRIS_CONN_08
#define TRIS_LIGHT_TSL250_OUT			TRIS_CONN_14

#define ADCH_LIGHT_TSL250				ADCH_CONN_14
#endif


// light sensor TSL2550
#define PIN_LIGHT_TSL2550_POWER			PIN_CONN_04
#define PIN_LIGHT_TSL2550_SDA			PIN_CONN_13
#define PIN_LIGHT_TSL2550_SCLK			PIN_CONN_09

#define TRIS_LIGHT_TSL2550_POWER		TRIS_CONN_04
#define TRIS_LIGHT_TSL2550_SDA			TRIS_CONN_13
#define TRIS_LIGHT_TSL2550_SCLK			TRIS_CONN_09


// temperature sensor MCP9800
#define PIN_TEMP_MCP9800_POWER			PIN_CONN_04
#define PIN_TEMP_MCP9800_SDA			PIN_CONN_13
#define PIN_TEMP_MCP9800_SCLK			PIN_CONN_09

#define TRIS_TEMP_MCP9800_POWER			TRIS_CONN_04
#define TRIS_TEMP_MCP9800_SDA			TRIS_CONN_13
#define TRIS_TEMP_MCP9800_SCLK			TRIS_CONN_09

#if 0
// temperature sensor TC74
#define PIN_TEMP_TC74_POWER				PIN_CONN_04
#define PIN_TEMP_TC74_SDA				PIN_CONN_13
#define PIN_TEMP_TC74_SCLK				PIN_CONN_09

#define TRIS_TEMP_TC74_POWER			TRIS_CONN_04
#define TRIS_TEMP_TC74_SDA				TRIS_CONN_13
#define TRIS_TEMP_TC74_SCLK				TRIS_CONN_09
#endif


#if 0
// force sensor
#define PIN_FORCE_FSR_POWER				PIN_CONN_17
#define PIN_FORCE_FSR_OUT				PIN_CONN_12

#define TRIS_FORCE_FSR_POWER			TRIS_CONN_17
#define TRIS_FORCE_FSR_OUT				TRIS_CONN_12

#define ADCH_FORCE_FSR					ADCH_CONN_12	//channel AN2
#endif

// audio
#define PIN_AUDIO_MIC_POWER				PIN_CONN_17
#define PIN_AUDIO_MIC_OUT				PIN_CONN_11

#define TRIS_AUDIO_MIC_POWER			TRIS_CONN_17
#define TRIS_AUDIO_MIC_OUT				TRIS_CONN_11

#define ADCH_AUDIO_MIC					ADCH_CONN_11


// generic sensor
#define PIN_GENERIC_POWER				PIN_CONN_17
#define PIN_GENERIC_OUT					PIN_CONN_11

#define TRIS_GENERIC_POWER				TRIS_CONN_17
#define TRIS_GENERIC_OUT				TRIS_CONN_11
#if 0
#define ADCH_GENERIC_OUT				ADCH_CONN_14
#endif

#define PIN_LED_RED_POWER				PIN_CONN_19
#define TRIS_LED_RED_POWER				TRIS_CONN_19

#define PIN_LED_GREEN_POWER				PIN_CONN_18
#define TRIS_LED_GREEN_POWER			TRIS_CONN_18

#define PIN_LED_ONE_POWER				PIN_CONN_16
#define TRIS_LED_ONE_POWER				TRIS_CONN_16

#define PIN_LED_TWO_POWER				PIN_CONN_08
#define TRIS_LED_TWO_POWER				TRIS_CONN_08
// i2c sensors:

//pc i2c for eeprom etc
#define PIN_SENSOR_I2C_SDA          	PIN_CONN_13
#define TRIS_SENSOR_I2C_SDA         	TRIS_CONN_13

#define PIN_SENSOR_I2C_SCL          	PIN_CONN_09
#define TRIS_SENSOR_I2C_SCL         	TRIS_CONN_09

#define PIN_SENSOR_I2C_POWER        	PIN_CONN_04
#define TRIS_SENSOR_I2C_POWER       	TRIS_CONN_04

#define PIN_BUTTON_OUT					PIN_CONN_03
#define TRIS_BUTTON_OUT					TRIS_CONN_03


#endif // __SSIMP192_H__
