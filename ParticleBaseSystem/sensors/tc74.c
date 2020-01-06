/**
* Sensor driver for tc74 il2c temp sensor
*
* Provides low level functions for reading the tc74 temperature sensor
* via I2C.
*
* Author: Albert Krohn  (krohn@teco.edu)
* Date: 2003-07-06 (yyyy-mm-dd)
*
* modified by cdecker@teco.edu
*
* version 001:
*   - adapt driver to new i2c wrapper
*
* version 002:
*   - ACL pack function
*
*/


#define TEMPERATURE_SENSOR		002

#define I2C_TEMPERATURE_ID		0b10010000

/**
* Switches the temperature sensor on
*/
#define TemperatureSensorOn()	output_high(PIN_TEMP_TC74_POWER)

/**
* Switches the temperature sensor off
*/
#define TemperatureSensorOff()	output_low(PIN_TEMP_TC74_POWER)


/**
* Initializes the temperature sensor
* Uses the Sensor I2C Bus
*
* @return returns 0 if no error
*/

int TemperatureSensorInit()
{
#ifdef PIN_TEMP_TC74_POWER
	SensorI2CInit();						// needs i2c

	bit_clear(TRIS_TEMP_TC74_POWER);
	TemperatureSensorOff();

	return 0;								// indicate no error
#endif
}


/**
* Setups the I2C bus for the temperature sensor
* @return returns 0 if no error
*/
int TemperatureSensorPrepare()
{
	SensorI2COn();
	return 0;
}


/**
* Gets the sensor value
* @param value the sensor value
* @return returns 0 if no error
*/
int TemperatureSensorGet(signed int* value)
{

	signed int data;



   	SensorI2CStart();

	if (SensorI2CWrite(I2C_TEMPERATURE_ID)) {SensorI2CStop();return 2;}		//error: no ack, write to device
	if (SensorI2CWrite(0)) {SensorI2CStop();return 3;}				//error: no ack, read from temp reg

	SensorI2CStart();
	if (SensorI2CWrite(I2C_TEMPERATURE_ID+1)) {SensorI2CStop();return 4;}		//error: no ack, read from device
	data=SensorI2CRead(0);

	SensorI2CStop();

	*value=(data-3); // actually data-3.4 degress celsius

	return 0;
}




/**
* Packs the temperature sensor value directly in the sendbuffer
* as an ACL packet which is ready to be sent.
* @param value temperature sensor value
* @return error code, 0 if no error
*
* Note: The format is described in acltypes.h
*/
int TemperatureSensorPackInACL(signed int value)
{

	if ( ACLGetRemainingPayloadSpace() > 4 )
	{
		ACLAddNewType(ACL_TYPE_STE_HI,ACL_TYPE_STE_LO); // STE
		ACLAddData(value);								// value
		ACLAddData(0);									// index = 0
	}
	else
	{
		return 1; 	// not enough space left
	}

	return 0; // no error
}
