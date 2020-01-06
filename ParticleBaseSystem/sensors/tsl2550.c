/**
* Sensor driver for light sensor tsl25500
*
* Provides low and high level functions for initializing and sampling of the
* TSL2550 light sensor.
*
* Author: Christian Decker (cdecker@teco.edu)
*
* version 001 (2003-12-03, cdecker)
*   - driver on new software architecure
* version 002 (2004-02-05, sabin (wendhack@teco.edu))
* 	- pin corrected: PIN_LIGHT_TSL2550_SDA
* version 003 (2004-04-08, sabin (wendhack@teco.edu))
*	- gives long instead of int to AmbientLightIRSensorGet().
*/


#define AMBIENT_LIGHT_SENSOR			002


/**
* Switches the light sensor on
*/
#define AmbientLightSensorOn()	output_high(PIN_LIGHT_TSL2550_POWER)

/**
* Switches the light sensor off
*/
#define AmbientLightSensorOff()	output_low(PIN_LIGHT_TSL2550_POWER)


/**
* Initializes the TSL2550 light sensor.
* Sets power to output and sampling pin to input.
*
* @return returns 0 if no error
*/
#ifdef PIN_LIGHT_TSL2550_SDA
int AmbientLightSensorInit()
{
	SensorI2CInit();							// needs I2C

	bit_clear(TRIS_LIGHT_TSL2550_POWER);		// set power pin to output
	AmbientLightSensorOff();							// power off for light

	return 0;								// indicate no error
}
#endif


/**
* Supports the SensorGet functions in respect to the calculation
* of the light level for both the visible light and infrared light
* @param data sensor value from ambient light sensor
* @return light level from sensor value
*/
unsigned long AmbientLightCalcSensorValue(unsigned int data)
{
	unsigned int c, sv, sn;
	unsigned long ldata, cv;

	//Calculate C
	c=data;
	c&=0b01110000;
	c= c >> 4;
	//Calculate SV
	sv=0;
	bit_set(sv, c);
	//Calculate SN
	sn=data;
	sn&=0b00001111;
	//Calculate CV
	cv=16*((unsigned long)sv-1);
	//Calculate ldata
	ldata=cv+((unsigned long)sv*(unsigned long)sn);

	return ldata;

}

/**
* Gets the sensor value
* @param value the sensor value
* @return returns 0 if no error
*/
int AmbientLightVisibleSensorGet(unsigned long* value)
{

	unsigned int data;
	unsigned long ldata, cv;

	SensorI2CStart();
		if (SensorI2CWrite(0b01110010)) {SensorI2CStop();return 1;}		//error: no ack, write to device
		if (SensorI2CWrite(0b01000011)) {SensorI2CStop();return 2;}		//error: no ack, read from temp reg
	SensorI2CStop();

	SensorI2CStart();
		if (SensorI2CWrite(0b01110011)) {SensorI2CStop();return 3;}		//error: no ack, read from device
		data=SensorI2CRead(0);
	SensorI2CStop();

	*value=AmbientLightCalcSensorValue(data);

	return 0;
}

/**
* Gets the sensor value
* @param value the sensor value
* @return returns 0 if no error
*/
int AmbientLightIRSensorGet(unsigned long* value)
{

	unsigned int data, c, sv, sn;
	unsigned long ldata, cv;

	SensorI2CStart();
		if (SensorI2CWrite(0b01110010)) {SensorI2CStop();return 1;}		//error: no ack, write to device
		if (SensorI2CWrite(0b10000011)) {SensorI2CStop();return 2;}		//error: no ack, read from temp reg
	SensorI2CStop();

	SensorI2CStart();
		if (SensorI2CWrite(0b01110011)) {SensorI2CStop();return 3;}		//error: no ack, read from device
		data=SensorI2CRead(0);
	SensorI2CStop();


	*value=AmbientLightCalcSensorValue(data);

	return 0;
}

/**
* Setups the microcontroller for sampling
* Sets the correct AD channel, if neccessary
* @return returns 0 if no error
*/
int AmbientLightSensorPrepare()
{
	SensorI2COn();
	return 0;
}


/**
* Packs the light sensor value directly in the sendbuffer
* as an ACL packet which is ready to be sent.
* @param type type of audio sensor value (e.g. 22 for TSL2550 visible light)
* @param data pointer to data array of light value(s)
* @param dataLength length of data array
* @return error code, 0 if no error
*
* Note: The format is described in acltypes.h
*/
#ifndef LIGHTGENERICSENSORPACKINACL
#define LIGHTGENERICSENSORPACKINACL
int LightGenericSensorPackInACL(int type, byte* data, unsigned int dataLength, int index)
{
	int i;

	if ( ACLGetRemainingPayloadSpace() > 4 + dataLength)
	{
		ACLAddNewType(ACL_TYPE_SLI_HI,ACL_TYPE_SLI_LO); // SLI
		ACLAddData(type);								// type of sensor
		for (i = 0; i < dataLength; i++)
		{
			ACLAddData(data[i]);						// value(s)
		}
		ACLAddData(index);									// index = 0
	}
	else
	{
		return 1; 	// not enough space left
	}

	return 0; // no error
}
#endif // LIGHTGENERICSENSORPACKINACL


/**
* Packs the light sensor value for visible light directly in the sendbuffer
* as an ACL packet which is ready to be sent.
* @param value force sensor value
* @return error code, 0 if no error
*
* Note: The format is described in acltypes.h
*/
int AmbientLightVisibleSensorPackInACL(unsigned long value)
{

	byte tmp[2];
	int ret;

	tmp[0] = (byte)(value>>8);
	tmp[1] = (byte)(value&0x00FF);


	ret = LightGenericSensorPackInACL(22, tmp, 2, 0);  // 22 = TSL2550, Visible

	return ret;
}

/**
* Packs the light sensor value for visible light directly in the sendbuffer
* as an ACL packet which is ready to be sent.
* @param value force sensor value
* @return error code, 0 if no error
*
* Note: The format is described in acltypes.h
*/
int AmbientLightIRSensorPackInACL(unsigned long value)
{
	byte tmp[2];
	int ret;

	tmp[0] = (byte)(value>>8);
	tmp[1] = (byte)(value&0x00FF);


	ret = LightGenericSensorPackInACL(23, tmp, 2, 0);  // 23 = TSL2550, IR

	return ret;
}

