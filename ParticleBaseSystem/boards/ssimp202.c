/**
* board file for the simple sensor board 202
* based on ssimp192.c version 004
*
* sabin (wendhack@teco.edu)
* Date: 2005-03-08
*
* version: 001
*/


#ifndef __SSIMP202_C__
#define	__SSIMP202_C__					001

/**
* Initializes the SSimp
*/
void SSimpInit();

/**
* Switches the amber LED on
*/
void SSimpLEDAmberOn();

/**
* Switches the amber LED off
*/
void SSimpLEDAmberOff();

/**
* Initializes all sensors on SSimp
*/
void SSimpensorsInit();

/**
* Turn on all sensors on SSimp
*/
void SSimpSensorsOn();


void SSimpInit()
{

	// number of analog pins on the connector [0..4]
	// 0: CONN_11..CONN_14 are digital
	// 1: CONN_11 is analog, CONN_12..CONN_14 are digital
	// 2: CONN_11..CONN_12 are analog, CONN_13..CONN_14 are digital
	// 3: CONN_11..CONN_13 are analog, CONN_14 is digital
	// 4: CONN_11..CONN_14 are analog

	// set analog pins
	PCSetAnalogPorts(ANALOG_CONN_11_12_14);

	// set digital pins

	// comment in to test the memory consumption of the gigantic switch case function #-)
	// preliminary result: 8% more memory consumption
	//PCSetDigitalPin(PIN_ACCL_ADXL_POWER, high);

	bit_clear(TRIS_ACCL_ADXL_POWER);
	output_high(PIN_ACCL_ADXL_POWER);
	bit_set(TRIS_ACCL_ADXL_XOUT);
	bit_set(TRIS_ACCL_ADXL_YOUT);

	bit_clear(TRIS_ACCL_ADXL_POWER_Z);
	output_high(PIN_ACCL_ADXL_POWER_Z);
	bit_set(TRIS_ACCL_ADXL_ZOUT);

	bit_clear(TRIS_LIGHT_TSL250_POWER);
	output_high(PIN_LIGHT_TSL250_POWER);

	bit_clear(TRIS_LIGHT_TSL2550_POWER);
	output_high(PIN_LIGHT_TSL2550_POWER);

	bit_set(TRIS_LIGHT_TSL2550_SDA);
	bit_set(TRIS_LIGHT_TSL2550_SCLK);

	bit_clear(TRIS_TEMP_MCP9800_POWER);
	output_high(PIN_TEMP_MCP9800_POWER);

	bit_set(TRIS_TEMP_MCP9800_SDA);
	bit_set(TRIS_TEMP_MCP9800_SCLK);

	bit_clear(TRIS_FORCE_FSR_POWER);
	output_high(PIN_FORCE_FSR_POWER);

	bit_set(TRIS_FORCE_FSR_OUT);

	bit_clear(TRIS_AUDIO_MIC_POWER);
	output_high(PIN_AUDIO_MIC_POWER);

	bit_set(TRIS_AUDIO_MIC_OUT);

	bit_clear(TRIS_LED_AMBER_POWER);
	output_high(PIN_LED_AMBER_POWER);


	bit_clear(TRIS_SENSOR_I2C_POWER);
	output_high(PIN_SENSOR_I2C_POWER);

	bit_set(TRIS_SENSOR_I2C_SDA);
	bit_set(TRIS_SENSOR_I2C_SCL);


}

void SSimpLEDAmberOn()
{
	output_high(PIN_LED_AMBER_POWER);
}


void SSimpLEDAmberOff()
{
	output_low(PIN_LED_AMBER_POWER);
}

void SSimpSensorsInit()
{

	#ifdef LIGHT_SENSOR
		LightSensorInit();		//inits the light and sets the tris etc..
	#endif

	#ifdef AUDIO_SENSOR
		AudioSensorInit();		//inits the light and sets the tris etc..
	#endif

	#ifdef FORCE_SENSOR
		ForceSensorInit();		//inits the light and sets the tris etc..
	#endif

	#ifdef ACCL_SENSOR
		AcclSensorInit();		//inits the light and sets the tris etc..
	#endif

	#ifdef ACCL_Z_SENSOR
		AcclZSensorInit();		//inits the light and sets the tris etc..
	#endif

	#ifdef TEMPERATURE_SENSOR
		TemperatureSensorInit();		//inits the light and sets the tris etc..
	#endif

	#ifdef AMBIENT_LIGHT_SENSOR
		AmbientLightSensorInit();
	#endif

}


void SSimpSensorsOn()
{


	#ifdef LIGHT_SENSOR
		LightSensorOn();
	#endif

	#ifdef AUDIO_SENSOR
		AudioSensorOn();
	#endif

	#ifdef FORCE_SENSOR
		ForceSensorOn();
	#endif

	#ifdef ACCL_SENSOR
		AcclSensorOn();
	#endif

	#ifdef ACCL_Z_SENSOR
		AcclZSensorOn();
	#endif

	#ifdef TEMPERATURE_SENSOR
		TemperatureSensorOn();
	#endif

	#ifdef AMBIENT_LIGHT_SENSOR
		AmbientLightSensorOn();
	#endif
}


#endif // __SSIMP192_C__