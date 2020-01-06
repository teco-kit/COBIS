/**
* Sensor driver for measuring the battery voltage
*
* This is a particle core board driver, i.e. the voltage measurement is done
* on the core board and not on a sensor board.
* Provides low and high level functions for initializing and sampling of the
* voltage measurment.
*
* Author: Christian Decker (cdecker@teco.edu)
* Date: 2003-12-11 (yyyy-mm-dd)
*
* version 001 (2003-12-11, cdecker)
*   - init version
*
* version 002 (2004-04-05) by Klaus Schmidt (schmidtk@teco.edu)
*	- SPart & Particle version
*
* version 003 (2004-11-03) by Michael Demel	(demel@teco.edu)
*   - Voltage Sensor adjust to 3,3V (VRef)
*/


#define VOLTAGE_SENSOR			002

/**
* On/Off functions are not need, because this feature is not supported
*/

/**
* Initializes the Voltage sensor.
* @return returns 0 if no error
*/
#ifdef PIN_VOLTAGE
int VoltageSensorInit()
{
	// nothing to be done here, since PCInit() function
	return 0;
}
#else
	#error "No Voltage sensor declared"
#endif


/**
* Packs the voltage directly in the sendbuffer
* as an ACL packet which is ready to be sent.
* @param value voltage measurement [mV]
* @return error code, 0 if no error
*
* Note: The format is described in acltypes.h
*/
int VoltageSensorPackInACL(long value)
{

	if ( ACLGetRemainingPayloadSpace() > 4 )
	{
		ACLAddNewType(ACL_TYPE_SVC_HI,ACL_TYPE_SVC_LO); // SVC
		ACLAddData((byte)(value>>8));					// value
		ACLAddData((byte)(value&0x00FF));				// value
	}
	else
	{
		return 1; 	// not enough space left
	}

	return 0; // no error
}


#ifndef __SPart202_C__		// normal voltage-driver for coreboard
/**
* Gets the raw sensor value
* @param value the sensor value
* @return returns 0 if no error
*/
int SampledVoltageSensorGet(long* value)
{

	*value = ADCRead10();
	return 0;
}

/**
* Gets the voltage in millivolt (mV)
* @param value voltage in millivolt (mV)
* @return returns 0 if no error
*/
int VoltageSensorGet(long* value)
{
	long v,v2;
	int ret;

	ret = SampledVoltageSensorGet(&v);

	// 3300 mV are sampled as 1024
	// Formula v2 = (v * 3) / 4  returns 3328
	// Error: 0.8%

	v2 = v<<1;
	v2 += v;
	v2 += (v>>2);

	*value = v2;

	return ret;
}

/**
* Setups the microcontroller for sampling
* Sets the correct AD channel, if neccessary
* @return returns 0 if no error
*/
int VoltageSensorPrepare()
{
	// set channel
	if ( !ADCIsChSet(ADCH_VOLTAGE) ) {
		ADCSetCh(ADCH_VOLTAGE);
	}

	// set conversion clock
	ADCSetClock(0);

	// start ADC
	ADCEnable();

	return 0;
}
#endif		// __SPart202_C__ (normal voltage-driver for coreboard)


#ifdef __SPart202_C__		// voltage-driver-functions to use from SPart via RPC
/**
* Setups the microcontroller of the Coreboard for sampling
* Sets the correct AD channel, if neccessary
* @result returns 0 if no error
*/
int VoltageSensorPrepare() {

	int result;

	result = RPC(20000, RPC_VoltageSensorPrepare,0 ,0 ,0);

	return result;
}

/**
* Gets the value of the voltage sensor in mV over RPC, saves
* it to get it with RPC_ReturnPointerResult
* @*value : the voltage value as a long pointer
* @result : returns 0 if no error
*/
int VoltageSensorGet(long* value)
{
	int result;
	unsigned long temp;

	result = RPC(20000, RPC_VoltageSensorGet, 0, 0, 0);

	temp = ( 0x00 | RPC(20000,RPC_ReturnPointerResult, 1, 0, 0) );
	*value = (0x00 | (RPC(20000, RPC_ReturnPointerResult, 0, 0, 0) << 8) );

	*value = ( *value | temp );

	return result;

}
#endif			// __SPart202_C__