/**
* Sensor template driver example for a user sensor
*
* Provides low and high level functions for initializing and sampling of the example sensor
* the sensor has a power line on conn2 and an analogue value on conn11.
*
* Author: Al krohn@teco.edu
* Date: 2004-04-13 (yyyy-mm-dd)

*
*/


#define MY_SENSOR			001					// define the name and version of your sensor


/**
* Switches the sensor on
*/
#define MySensorOn()	output_high(PIN_CONN_02);

/**
* Switches the sensor off
*/
#define MySensorOff()	output_low(PIN_CONN_02);


/**
* Initializes my sensor
* Sets power to output and sampling pin to input.
*
* @return returns 0 if no error
*/

int MySensorInit()
{
	bit_clear(TRIS_CONN_02);					// set power pin to output
	bit_set(TRIS_CONN_03);					// set sample pin to input

	MySensorOff();							// power off

	return 0;								// indicate no error
}




/**
* Gets the sensor value
* @param value the sensor value
* @return returns 0 if no error
*/
int MySensorGet(int* value)
{

	*value = ADCRead8();
	return 0;
}



/**
* Setups the microcontroller for sampling
* Sets the correct AD channel, if neccessary
* @return returns 0 if no error
*/
int MySensorPrepare()
{
	// set channel
	if ( !ADCIsChSet(ADCH_CONN_11) ) {
		ADCSetCh(ADCH_CONN_11);
	}

	// set conversion clock
	ADCSetClock(0);

	// start ADC
	ADCEnable();

	return 0;
}

