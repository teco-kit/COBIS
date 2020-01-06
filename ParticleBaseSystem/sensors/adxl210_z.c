/**
* Sensor driver for adxl210
*
* Provides low and high level functions for initializing and sampling of the
* adxl acceleration sensor
*
* Author: Albert Krohn  (krohn@teco.edu)
* Date: 2003-06-11 (yyyy-mm-dd)
*
* version 001 (2003-12-03, cdecker)
*   - driver on new software architecure
*
*/


#define ACCL_Z_SENSOR			001


/**
* Switches the ACCL on
*/
#define AcclZSensorOn()	output_high(PIN_ACCL_ADXL_POWER_Z)

/**
* Switches the Adxl off
*/
#define AcclZSensorOff()	output_low(PIN_ACCL_ADXL_POWER_Z)


/**
* Initializes the Adxl
* Sets power to output and sampling pins to input.
*
* @return returns 0 if no error
*/
#ifdef PIN_ACCL_ADXL_ZOUT
int AcclZSensorInit()
{
	bit_clear(TRIS_ACCL_ADXL_POWER_Z);		// set power pin to output
	bit_set(TRIS_ACCL_ADXL_ZOUT);			// set sample pinx to input

	AcclZSensorOff();						// power off for light

	return 0;								// indicate no error
}
#endif


/**
* Setups the microcontroller for sampling
* @return returns 0 if no error
*/
int AcclZSensorPrepare()
{

	//nothing to do here???
	return 0;
}





/**
* Gets the duty cycle (raw value) from the accel sensor value
* @param dutyHi duration in the duty cycle where the signal is high
* @param dutyLo duration in the duty cycle where the signal is low
* @return returns 0 if no error
*/
int AcclZSensorGetDuty(unsigned long *dutyHi, unsigned long *dutyLo)
{
	unsigned long zt1,zt2;


	zt1=0;							// two commands for saving assembler space
	zt2=0;


	while( input(PIN_ACCL_ADXL_ZOUT) );				// wait for adx = low
	while( !input(PIN_ACCL_ADXL_ZOUT) );				// wait for adx = high
	while( input(PIN_ACCL_ADXL_ZOUT))	++zt1;			// count duration
	while( !input(PIN_ACCL_ADXL_ZOUT))	++zt2;			// count duration

	*dutyHi = zt1;
	*dutyLo = zt2;

	return 0;
}


/**
* Returns the acceleration in g scale
* Unit: for one g (scale=10 means +-1g is +-10 digits)
* @param g the g value
* @param dutyHi duration in the duty cycle where the signal is high
* @param dutyLo duration in the duty cycle where the signal is low
* @return returns 0 if no error
*
* Note: this is a new version from 15.sep 2003 Albert/ not very efficient, but better
*/
#ifndef ACCL_SENSOR
int AcclSensorGetGravity(signed long* g, unsigned long dutyHi, unsigned long dutyLo)
{
	unsigned long xt1,xt2;
	unsigned long sum_div_2;
	unsigned long acl_const;
	signed long res;

	xt1=dutyHi;
	xt2=dutyLo;


	sum_div_2 = ( xt1 + xt2 ) >> 1;
	acl_const = 12500 / sum_div_2;

	res = (xt1 - sum_div_2)*acl_const;

	*g = res;

	return 0;
}
#endif // ACCL_SENSOR


/**
* Returns the acceleration in g scale
* Unit: for one g (scale=10 means +-1g is +-10 digits)
* @param g the accl value in g
* @return returns 0 if no error
*/
int AcclZSensorGetGravity(signed long* g)
{
	unsigned long dutyHi,dutyLo;
	int ret;

	AcclZSensorGetDuty(&dutyHi, &dutyLo);
	ret = AcclSensorGetGravity(g, dutyHi, dutyLo);

	return ret;
}


/**
* Packs the accl sensor value for the Z axis directly in the sendbuffer
* as an ACL packet which is ready to be sent.
* @param value temperature sensor value
* @return error code, 0 if no error
*
* Note: The format is described in acltypes.h
*/
int AcclZSensorPackInACL(signed long value)
{

	if ( ACLGetRemainingPayloadSpace() > 5 )
	{
		ACLAddNewType(ACL_TYPE_SGZ_HI,ACL_TYPE_SGZ_LO); // SGZ
		ACLAddData((byte)(value>>8));					// value
		ACLAddData((byte)(value&0x00FF));				// value
		ACLAddData(0);									// index = 1
	}
	else
	{
		return 1; 	// not enough space left
	}

	return 0; // no error
}


