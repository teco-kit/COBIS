/**
* header file for the breakout board "proto 1/80"
*
* Author: krohn@teco.edu
* Date: 2004-04-13
*
* version: 001
*   - sensor pin defs
*
*/


#ifndef __BROUT180_H__
#define	__BROUT180_H__					001


// pin defs for sensors


// i2c sensors:


//pc i2c for eeprom etc
#define PIN_SENSOR_I2C_SDA          	PIN_CONN_13
#define TRIS_SENSOR_I2C_SDA         	TRIS_CONN_13

#define PIN_SENSOR_I2C_SCL          	PIN_CONN_09
#define TRIS_SENSOR_I2C_SCL         	TRIS_CONN_09

#define PIN_SENSOR_I2C_POWER        	PIN_CONN_04
#define TRIS_SENSOR_I2C_POWER       	TRIS_CONN_04


#endif // __BROUT180_H