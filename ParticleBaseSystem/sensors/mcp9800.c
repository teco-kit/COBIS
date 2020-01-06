/**
* Sensor driver for mcp9800 il2c temp sensor
*
* Provides low level functions for reading the mcp9800 temperature sensor
* via I2C.
*
* Author: sabin  (wendhack@teco.edu)
* Date: 2005-02-22 (yyyy-mm-dd)
*
* version 001
*/


#define TEMPERATURE_SENSOR		001

#define I2C_TEMPERATURE_ID		0b10010000

/**
* Switches the temperature sensor on
*/
#define TemperatureSensorOn()	output_high(PIN_TEMP_MCP9800_POWER)

/**
* Switches the temperature sensor off
*/
#define TemperatureSensorOff()	output_low(PIN_TEMP_MCP9800_POWER)


/**
* Initializes the temperature sensor
* Uses the Sensor I2C Bus
*
* @return returns 0 if no error
*/

#define MCP9800_REG_CONFIG 1

#define MCP9800_CONFIG_ONE_SHOT (1<<7) 

#define MCP9800_CONFIG_9BIT (0<<5) 
#define MCP9800_CONFIG_10BIT (1<<5) 
#define MCP9800_CONFIG_11BIT (2<<5) 
#define MCP9800_CONFIG_12BIT (3<<5) 

#define MCP9800_CONFIG_FAULTQ_1 (0<<3) 
#define MCP9800_CONFIG_FAULTQ_2 (1<<3) 
#define MCP9800_CONFIG_FAULTQ_4 (2<<3) 
#define MCP9800_CONFIG_FAULTQ_6 (3<<3) 

#define MCP9800_CONFIG_ALERT_HIGH (1<<2) 
#define MCP9800_CONFIG_ALERT_LOW (0<<2) 

#define MCP9800_CONFIG_INT_MODE (1<<1) 
#define MCP9800_CONFIG_CMP_MODE (0<<1) 

#define MCP9800_CONFIG_SHUTDOWN (1<<0) 

int TemperatureSensorInit()
{
#ifdef PIN_TEMP_MCP9800_POWER
	SensorI2CInit();						// needs i2c

	bit_clear(TRIS_TEMP_MCP9800_POWER);
	TemperatureSensorOff();

// configure for 12bit resolution (=max, default is 9bit)
	SensorI2COn();
   	SensorI2CStart();

	if (SensorI2CWrite(I2C_TEMPERATURE_ID)) {SensorI2CStop();return 2;}		//error: no ack, write to device
	if (SensorI2CWrite(1)) {SensorI2CStop();return 3;}				//error: no ack, read from temp reg, send pointer to config register.

	if (SensorI2CWrite(MCP9800_CONFIG_12BIT)) {SensorI2CStop();return 4;}				//error: no ack, read from temp reg, send config register value (12bit mode).


	SensorI2CStop();

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

	unsigned int data1;
	unsigned int data2;


	SensorI2CStart();
	if (SensorI2CWrite(I2C_TEMPERATURE_ID)) {SensorI2CStop();return 4;}		//error: no ack, read from device
	if (SensorI2CWrite(0)) {SensorI2CStop();return 3;}				//error: no ack, read from temp reg
	SensorI2CStart();
	if (SensorI2CWrite(I2C_TEMPERATURE_ID+1)) {SensorI2CStop();return 4;}		//error: no ack, read from device
	data1=SensorI2CRead(1); //first byte acked
	data2=SensorI2CRead(0); //second byte not acked as to indicate end of packet.

	SensorI2CStop();

	*value = data1;

	return 0;
}




/**
* Gets the sensor value
* @param value the sensor value
* @return returns 0 if no error
*/
int TemperatureSensorGet12bit(signed long* value)
{

	unsigned int data1;
	unsigned int data2;
	unsigned long data;


	SensorI2CStart();
	if (SensorI2CWrite(I2C_TEMPERATURE_ID)) {SensorI2CStop();return 4;}		//error: no ack, read from device
	if (SensorI2CWrite(0)) {SensorI2CStop();return 3;}				//error: no ack, read from temp reg
	SensorI2CStart();
	if (SensorI2CWrite(I2C_TEMPERATURE_ID+1)) {SensorI2CStop();return 4;}		//error: no ack, read from device
	data1=SensorI2CRead(1); //first byte acked
	data2=SensorI2CRead(0); //second byte not acked as to indicate end of packet.

	SensorI2CStop();

	data = (((long)(data1)) << 8 ) + ( long )( data2 );
	*value = data;

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

	if ( ACLGetRemainingPayloadSpace() > 5 )
	{
		ACLAddNewType(ACL_TYPE_STE_HI,ACL_TYPE_STE_LO); // STE
		ACLAddData( value );								// value
		ACLAddData(0);									// index = 0
	}
	else
	{
		return 1; 	// not enough space left
	}

	return 0; // no error
}


/**
* Packs the temperature sensor value directly in the sendbuffer
* as an ACL packet which is ready to be sent.
* @param value temperature sensor value
* @return error code, 0 if no error
*
* Note: The format is described in acltypes.h
*/
int TemperatureSensorPackInACL12bit(signed long value)
{

	if ( ACLGetRemainingPayloadSpace() > 5 )
	{
		ACLAddNewType(ACL_TYPE_STE_HI,ACL_TYPE_STE_LO); // STE
		ACLAddData((byte)(value>>8));								// value
		ACLAddData((byte)(value&0x00FF));								// value
		ACLAddData(0);									// index = 0
	}
	else
	{
		return 1; 	// not enough space left
	}

	return 0; // no error
}
