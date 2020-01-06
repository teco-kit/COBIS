/**
*	2004-01-21 Adapted to SPart by sabin (wendhack@teco.edu)
*
**/

#ifndef __SPART202_H__
#define	__SPART202_H__					001


#use delay(clock=20000000)


// pins for LEDs
#define LED_SPART_RED			PIN_C5
#define LED_SPART_BLUE			PIN_D3
#define RED						LED_SPART_RED			//for convenience
#define BLUE					LED_SPART_BLUE


//pins for pc i2c (eeprom etc)
#define PIN_I2C_SPART_SDA		PIN_C4
#define TRIS_I2C_SPART_SDA		PIC_TRISC,4
#define PIN_I2C_SPART_SCL		PIN_C3
#define TRIS_I2C_SPART_SCL		PIC_TRISC,3
#define	PIN_I2C_SPART_POWER		PIN_C2
#define	TRIS_I2C_SPART_POWER	PIC_TRISC,2

#define	XROM_ID				0

//connectors defines for conan
#define PIN_CONN_01                             // VCC
#define PIN_CONN_02                             PIN_B0
#define PIN_CONN_03                             PIN_B1
#define PIN_CONN_04                             PIN_B2
#define PIN_CONN_05
#define PIN_CONN_06
#define PIN_CONN_07
#define PIN_CONN_08                             PIN_B6
#define PIN_CONN_09                             PIN_B7
#define PIN_CONN_10                             // MCLR
#define PIN_CONN_11
#define PIN_CONN_12
#define PIN_CONN_13
#define PIN_CONN_14
#define PIN_CONN_15
#define PIN_CONN_16
#define PIN_CONN_17
#define PIN_CONN_18
#define PIN_CONN_19                             PIN_C7
#define PIN_CONN_20                             PIN_C6
#define PIN_CONN_21                             // GND

#define TRIS_CONN_01                    // VCC
#define TRIS_CONN_02                    PIC_TRISB,0
#define TRIS_CONN_03                    PIC_TRISB,1
#define TRIS_CONN_04                    PIC_TRISB,2
#define TRIS_CONN_05
#define TRIS_CONN_06
#define TRIS_CONN_07
#define TRIS_CONN_08                    PIC_TRISB,6
#define TRIS_CONN_09                    PIC_TRISB,7
#define TRIS_CONN_10                    // MCLR
#define TRIS_CONN_11
#define TRIS_CONN_12
#define TRIS_CONN_13
#define TRIS_CONN_14
#define TRIS_CONN_15
#define TRIS_CONN_16
#define TRIS_CONN_17
#define TRIS_CONN_18
#define TRIS_CONN_19                    PIC_TRISC,7
#define TRIS_CONN_20                    PIC_TRISC,6
#define TRIS_CONN_21                    // GND

//for RPC SERVER
#define TRIS_RPC_HANDSHAKE_CLIENT		TRIS_CONN_04
#define PIN_RPC_HANDSHAKE_CLIENT		PIN_CONN_04

// pin defs for sensors

// accelerator ADXL210
#define PIN_ACCL_ADXL_POWER				PIN_D6
#define PIN_ACCL_ADXL_XOUT				PIN_D7
#define PIN_ACCL_ADXL_YOUT				PIN_B3

#define TRIS_ACCL_ADXL_POWER			PIC_TRISD, 6
#define TRIS_ACCL_ADXL_XOUT				PIC_TRISD, 7
#define TRIS_ACCL_ADXL_YOUT				PIC_TRISB, 3

//extra accl
#define PIN_ACCL_ADXL_POWER_Z			PIN_D6
#define PIN_ACCL_ADXL_ZOUT				PIN_A4

#define TRIS_ACCL_ADXL_POWER_Z			PIC_TRISD, 6
#define TRIS_ACCL_ADXL_ZOUT				PIC_TRISA, 4


// light sensor TSL250_A
#define PIN_LIGHT_TSL250_POWER_A		PIN_D5
#define PIN_LIGHT_TSL250_OUT_A			PIN_A0

#define TRIS_LIGHT_TSL250_POWER_A		PIC_TRISD, 5
#define TRIS_LIGHT_TSL250_OUT_A			PIC_TRISA, 0

#define ADCH_LIGHT_TSL250_A				ADCH_PIN_A0


// light sensor TSL250_B
#define PIN_LIGHT_TSL250_POWER_B		PIN_D5
#define PIN_LIGHT_TSL250_OUT_B			PIN_A2

#define TRIS_LIGHT_TSL250_POWER_B		PIC_TRISD, 5
#define TRIS_LIGHT_TSL250_OUT_B			PIC_TRISA, 2

#define ADCH_LIGHT_TSL250_B				ADCH_PIN_A2


// light sensor TSL2550
#define PIN_LIGHT_TSL2550_POWER			PIN_D1
#define PIN_LIGHT_TSL2550_SDA			PIN_D4
#define PIN_LIGHT_TSL2550_SCLK			PIN_E2

#define TRIS_LIGHT_TSL2550_POWER		PIC_TRISD, 1
#define TRIS_LIGHT_TSL2550_SDA			PIC_TRISD, 4
#define TRIS_LIGHT_TSL2550_SCLK			PIC_TRISE, 2


// temperature sensor TC74
#define PIN_TEMP_TC74_POWER				PIN_D1
#define PIN_TEMP_TC74_SDA				PIN_D4
#define PIN_TEMP_TC74_SCLK				PIN_E2

#define TRIS_TEMP_TC74_POWER			PIC_TRISD, 1
#define TRIS_TEMP_TC74_SDA				PIC_TRISD, 4
#define TRIS_TEMP_TC74_SCLK				PIC_TRISE, 2


// force sensor
#define PIN_FORCE_FSR_POWER				PIN_D0
#define PIN_FORCE_FSR_OUT				PIN_A3

#define TRIS_FORCE_FSR_POWER			PIC_TRISD, 0
#define TRIS_FORCE_FSR_OUT				PIC_TRISA, 3

#define ADCH_FORCE_FSR					ADCH_PIN_A3


// audio
#define PIN_AUDIO_MIC_POWER				PIN_B4
#define PIN_AUDIO_MIC_OUT				PIN_A1

#define TRIS_AUDIO_MIC_POWER			PIC_TRISB, 4
#define TRIS_AUDIO_MIC_OUT				PIC_TRISA, 1

#define ADCH_AUDIO_MIC					ADCH_PIN_A1


// generic sensor
#define PIN_GENERIC_POWER				PIN_D5
#define PIN_GENERIC_OUT					PIN_A2

#define TRIS_GENERIC_POWER				PIC_TRISD, 5
#define TRIS_GENERIC_OUT				PIC_TRISA, 2

#define ADCH_GENERIC_OUT				ADCH_PIN_A2


// i2c sensors:
#define PIN_SENSOR_I2C_SDA          	PIN_D4
#define TRIS_SENSOR_I2C_SDA         	PIC_TRISD, 4

#define PIN_SENSOR_I2C_SCL          	PIN_E2
#define TRIS_SENSOR_I2C_SCL         	PIC_TRISE, 2

#define PIN_SENSOR_I2C_POWER        	PIN_D1
#define TRIS_SENSOR_I2C_POWER       	PIC_TRISD, 1


#use fast_io(A)
#use fast_io(B)
#use fast_io(C)
#use fast_io(D)
#use fast_io(E)

