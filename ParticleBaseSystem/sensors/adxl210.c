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


#define ACCL_SENSOR			001


/**
* Switches the ACCL on
*/
#define AcclSensorOn()	output_high(PIN_ACCL_ADXL_POWER)

/**
* Switches the Adxl off
*/
#define AcclSensorOff()	output_low(PIN_ACCL_ADXL_POWER)


/**
* Initializes the Adxl
* Sets power to output and sampling pins to input.
*
* @return returns 0 if no error
*/
#ifdef PIN_ACCL_ADXL_XOUT
int AcclSensorInit()
{
	bit_clear(TRIS_ACCL_ADXL_POWER);		// set power pin to output
	bit_set(TRIS_ACCL_ADXL_XOUT);			// set sample pinx to input
	bit_set(TRIS_ACCL_ADXL_YOUT);			// set sample piny to input

 	AcclSensorOff();						// power off for light

	return 0;								// indicate no error
}
#endif


/**
* Setups the microcontroller for sampling
* @return returns 0 if no error
*/
int AcclSensorPrepare()
{

	//nothing to do here??? yes: wait for adxl to settle after turn on
	return 0;
}





/**
* Gets the duty cycle (raw value) from the accel sensor value
* @param dutyHi duration in the duty cycle where the signal is high
* @param dutyLo duration in the duty cycle where the signal is low
* @return returns 0 if no error
*/
int AcclXSensorGetDuty(unsigned long *dutyHi, unsigned long *dutyLo)
{
	unsigned long xt1,xt2;


	xt1=0;							// two commands for saving assembler space
	xt2=0;


	while( input(PIN_ACCL_ADXL_XOUT) );				// wait for adx = low
	while( !input(PIN_ACCL_ADXL_XOUT) );				// wait for adx = high
	while( input(PIN_ACCL_ADXL_XOUT))	++xt1;			// count duration
	while( !input(PIN_ACCL_ADXL_XOUT))	++xt2;			// count duration

	*dutyHi = xt1;
	*dutyLo = xt2;

	return 0;
}

/**
* Gets the duty cycle (raw value) from the accel sensor value
* @param dutyHi duration in the duty cycle where the signal is high
* @param dutyLo duration in the duty cycle where the signal is low
* @return returns 0 if no error
*/
int AcclYSensorGetDuty(unsigned long *dutyHi, unsigned long *dutyLo)
{
	unsigned long yt1,yt2;


	yt1=0;							// two commands for saving assembler space
	yt2=0;


	while( input(PIN_ACCL_ADXL_YOUT) );				// wait for adx = low
	while( !input(PIN_ACCL_ADXL_YOUT) );				// wait for adx = high
	while( input(PIN_ACCL_ADXL_YOUT))	++yt1;			// count duration
	while( !input(PIN_ACCL_ADXL_YOUT))	++yt2;			// count duration

	*dutyHi = yt1;
	*dutyLo = yt2;

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
#ifndef ACCL_Z_SENSOR
int AcclSensorGetGravity(signed long* g, unsigned long dutyHi, unsigned long dutyLo)
{

/*
	unsigned long yt1,yt2;
	float full;
	float middle;
	float first_half;
	float f_g;

	yt1=dutyHi;
	yt2=dutyLo;

	full=(float) yt1+yt2;
	middle=(float) ((yt1+yt2)>>1);
	first_half= (float) yt1;

	f_g=((first_half-middle)/full);		//gives back the +- percent

	f_g=f_g*8000; //=*1000/0.125;							//how many time 12.5%

	*g=(signed long)f_g;

	return 0;
*/

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
#endif // ACCL_SENSOR_Z

/**
* Returns the acceleration in g scale
* Unit: for one g (scale=10 means +-1g is +-10 digits)
* @param g the accl value in g
* @return returns 0 if no error
*/
int AcclXSensorGetGravity(signed long* g)
{
	unsigned long dutyHi,dutyLo;
	int ret;

	AcclXSensorGetDuty(&dutyHi, &dutyLo);
	ret = AcclSensorGetGravity(g, dutyHi, dutyLo);

	return ret;
}

/**
* Returns the acceleration in g scale
* Unit: for one g (scale=10 means +-1g is +-10 digits)
* @param g the accl value in g
* @return returns 0 if no error
*/
int AcclYSensorGetGravity(signed long* g)
{
	unsigned long dutyHi,dutyLo;
	int ret;

	AcclYSensorGetDuty(&dutyHi, &dutyLo);
	ret = AcclSensorGetGravity(g, dutyHi, dutyLo);

	return ret;
}


/**
* Packs the accl sensor value for the x axis directly in the sendbuffer
* as an ACL packet which is ready to be sent.
* @param value temperature sensor value
* @return error code, 0 if no error
*
* Note: The format is described in acltypes.h
*/
int AcclXSensorPackInACL(signed long value)
{

	if ( ACLGetRemainingPayloadSpace() > 5 )
	{
		ACLAddNewType(ACL_TYPE_SGX_HI,ACL_TYPE_SGX_LO); // SGX
		ACLAddData((byte)(value>>8));					// value
		ACLAddData((byte)(value&0x00FF));				// value
		ACLAddData(0);									// index = 0
	}
	else
	{
		return 1; 	// not enough space left
	}

	return 0; // no error
}


/**
* Packs the accl sensor value for the y axis directly in the sendbuffer
* as an ACL packet which is ready to be sent.
* @param value temperature sensor value
* @return error code, 0 if no error
*
* Note: The format is described in acltypes.h
*/
int AcclYSensorPackInACL(signed long value)
{

	if ( ACLGetRemainingPayloadSpace() > 5 )
	{
		ACLAddNewType(ACL_TYPE_SGY_HI,ACL_TYPE_SGY_LO); // SGY
		ACLAddData((byte)(value>>8));					// value
		ACLAddData((byte)(value&0x00FF));				// value
		ACLAddData(0);									// index = 0
	}
	else
	{
		return 1; 	// not enough space left
	}

	return 0; // no error
}
