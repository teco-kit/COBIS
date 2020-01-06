/**
* Sensor driver for light sensor tsl250
*
* Provides low and high level functions for initializing and sampling of the
* light sensor.
*
* Author: Christian Decker (cdecker@teco.edu)
* Date: 2003-06-10 (yyyy-mm-dd)
*
* Adapted by sabin (wendhack@teco.edu)
* 2004-02-05
*
* Current state: template
*/


#define LIGHT_SENSOR_B


/**
* Switches the light sensor on
*/
#define LightSensorOn_B()	output_high(PIN_LIGHT_TSL250_POWER_B)

/**
* Switches the light sensor off
*/
#define LightSensorOff_B()	output_low(PIN_LIGHT_TSL250_POWER_B)


/**
* Initializes the TSL250 light sensor.
* Sets power to output and sampling pin to input.
*
* @return returns 0 if no error
*/
#ifdef PIN_LIGHT_TSL250_OUT_B
int LightSensorInit_B()
{
	bit_clear(TRIS_LIGHT_TSL250_POWER_B);		// set power pin to output
	bit_set(TRIS_LIGHT_TSL250_OUT_B);			// set sample pin to input
	LightSensorOff_B();							// power off for light

	return 0;								// indicate no error
}
#endif



/**
* Gets the sensor value
* @param value the sensor value
* @return returns 0 if no error
*/
int LightSensorGet_B(int* value)
{

	*value = ADCRead8();
	return 0;
}



/**
* Setups the microcontroller for sampling
* Sets the correct AD channel, if neccessary
* @return returns 0 if no error
*/
int LightSensorPrepare_B()
{
	// set channel
	if ( !ADCIsChSet(ADCH_LIGHT_TSL250_B) ) {
		ADCSetCh(ADCH_LIGHT_TSL250_B);
	}

	// set conversion clock
	ADCSetClock(0);

	// start ADC
	ADCEnable();

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
* Packs the light sensor value directly in the sendbuffer
* as an ACL packet which is ready to be sent.
* @param value force sensor value
* @return error code, 0 if no error
*
* Note: The format is described in acltypes.h
*/
#ifndef LIGHT_SENSOR_A
int LightSensorPackInACL(int sensortype, int value)
{

	int ret;

	ret = LightGenericSensorPackInACL( sensortype, &value, 1, 0 );  // 50 = TSL250

	return ret;
}
#endif

