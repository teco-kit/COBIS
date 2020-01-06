/**
* Sensor driver for force sensor fsr
*
* Provides low and high level functions for initializing and sampling of the
* force sensor.
*
* Author: Christian Decker (cdecker@teco.edu)
* Date: 2003-11-26 (yyyy-mm-dd)
*
* version 001 (2003-11-28, cdecker)
*   - changed pin naming from old driver design
*
* version 002 (2003-12-02, cdecker)
*   - sensor runs on new software architecture
*
* version 003 (2003-12-03, cdecker)
*   - add function to pack sensor value directly into packet
*
*/


#define FORCE_SENSOR			003

/**
* Switches the force sensor on
*/
#define ForceSensorOn()	output_high(PIN_FORCE_FSR_POWER)

/**
* Switches the force sensor off
*/
#define ForceSensorOff()	output_low(PIN_FORCE_FSR_POWER)


/**
* Initializes the FSR force sensor.
* Sets power to output and sampling pin to input.
*
* @return returns 0 if no error
*/
#ifdef PIN_FORCE_FSR_OUT
int ForceSensorInit()
{
	bit_clear(TRIS_FORCE_FSR_POWER);		// set power pin to output

	bit_set(TRIS_FORCE_FSR_OUT);			// set sample pin to input

	ForceSensorOff();							// power off for light
	return 0;								// indicate no error
}
#else
	#error "No FSR sensor declared"
#endif



/**
* Gets the sensor value
* @param value the sensor value
* @return returns 0 if no error
*/
int ForceSensorGet(int* value)
{

	*value = ADCRead8();
	return 0;
}



/**
* Setups the microcontroller for sampling
* Sets the correct AD channel, if neccessary
* @return returns 0 if no error
*/
int ForceSensorPrepare()
{
	// set channel
	if ( !ADCIsChSet(ADCH_FORCE_FSR) ) {
		ADCSetCh(ADCH_FORCE_FSR);
	}

	// set conversion clock
	ADCSetClock(0);

	// start ADC
	ADCEnable();

	return 0;
}


/**
* Packs the force sensor value directly in the sendbuffer
* as an ACL packet which is ready to be sent.
* @param value force sensor value
* @return error code, 0 if no error
*
* Note: The format is described in acltypes.h
*/
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

