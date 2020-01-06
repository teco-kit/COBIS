/**
* Sensor driver for TSOP36236TR
*
* Provides low and high level functions for initializing and sampling of the
* TSOP36236TR IR sensor.
*
* Author: Christian Decker (cdecker@teco.edu)
* Date: 2005-04-01 (yyyy-mm-dd)
*
* version 001 (2005-04-01, cdecker)
*   - initial revision
*
*/


#define IR_SENSOR			001

/**
* Switches the force sensor on
*/
#define IRSensorOn()		output_high(PIN_TSOP36236TR_POWER)

/**
* Switches the force sensor off
*/
#define IRSensorOff()		output_low(PIN_TSOP36236TR_POWER)


/**
* Initializes the TSOP36236TR sensor.
* Sets power to output and sampling pin to input.
*
* @return returns 0 if no error
*/
#ifdef PIN_TSOP36236TR_POWER
int IRSensorInit()
{
	bit_clear(TRIS_TSOP36236TR_POWER);		// set power pin to output

	IRSensorOff();							// power off
	return 0;								// indicate no error
}
#else
	#error "No TSOP36236TR sensor declared"
#endif



/**
* Gets the sensor value
* @param value the sensor value
* @return returns 0 if no error, otherwise no value was obtained
*/
int IRSensorGet(int* value)
{

	Uart1CheckError();
	if ( Uart1DataIsThere() )
	{
		*value = Uart1GetByte();
		return 0;
	}
	else
	{
		return 1;
	}

}



/**
* Setups the microcontroller for sampling
* @return returns 0 if no error
*/
int IRSensorPrepare()
{
	return 0;
}


/**
* Packs the  sensor value directly in the sendbuffer
* as an ACL packet which is ready to be sent.
* @param value force sensor value
* @return error code, 0 if no error
*
* Note: The format is described in acltypes.h
*/
/* - not necessary here
int ForceSensorPackInACL(int value)
{

	if ( ACLGetRemainingPayloadSpace() > 4 )
	{
		ACLAddNewType(ACL_TYPE_SFC_HI,ACL_TYPE_SFC_LO); // SFC
		ACLAddData(value);								// value
		ACLAddData(0);									// index = 0
	}
	else
	{
		return 1; 	// not enough space left
	}

	return 0; // no error
}
*/
