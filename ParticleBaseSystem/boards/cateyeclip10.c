/**
* board file for the simple sensor board 202
* based on ssimp192.c version 004
*
* sabin (wendhack@teco.edu)
* Date: 2005-03-08
*
* version: 001
*/


#ifndef __CATEYECLIP10_C__
#define	__CATEYECLIP10_C__					001

/**
* Initializes the CateyeClip
*/
void CateyeClipInit();

/**
* Initializes all sensors on CateyeClip
*/
void CateyeClipSensorsInit();

/**
* Turn on all sensors on CateyeClip
*/

void CateyeClipUart1Init();

void CateyeClipInit()
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
	//output_high(PIN_ACCL_ADXL_POWER);
	output_low(PIN_ACCL_ADXL_POWER);		// fixed power problem
	bit_set(TRIS_ACCL_ADXL_XOUT);
	bit_set(TRIS_ACCL_ADXL_YOUT);


	bit_clear(TRIS_LIGHT_TSL2550_POWER);
	//output_high(PIN_LIGHT_TSL2550_POWER);
	output_low(PIN_LIGHT_TSL2550_POWER);		// fixed power problem

	bit_set(TRIS_LIGHT_TSL2550_SDA);
	bit_set(TRIS_LIGHT_TSL2550_SCLK);

	bit_clear(TRIS_TEMP_MCP9800_POWER);
	//output_high(PIN_TEMP_MCP9800_POWER);
	output_low(PIN_TEMP_MCP9800_POWER);		// fixed power problem

	bit_set(TRIS_TEMP_MCP9800_SDA);
	bit_set(TRIS_TEMP_MCP9800_SCLK);

	bit_clear(TRIS_AUDIO_MIC_POWER);
	//output_high(PIN_AUDIO_MIC_POWER);
	output_low(PIN_AUDIO_MIC_POWER);		// fixed power problem

	bit_set(TRIS_AUDIO_MIC_OUT);


	// little output / input TRICK (uart uses this pin as transmitting pin)
	// led on -> set pin to output, led off -> set pin to input
	bit_set(TRIS_LED_RED_POWER);
	//output_low(PIN_LED_RED_POWER);

	bit_clear(TRIS_LED_GREEN_POWER);
	output_low(PIN_LED_GREEN_POWER);

	bit_clear(TRIS_LED_ONE_POWER);
	output_low(PIN_LED_ONE_POWER);

	bit_clear(TRIS_LED_TWO_POWER);
	output_low(PIN_LED_TWO_POWER);

	bit_set(TRIS_BUTTON_OUT);

	bit_clear(TRIS_SENSOR_I2C_POWER);
	//output_high(PIN_SENSOR_I2C_POWER);
	output_low(PIN_SENSOR_I2C_POWER);		// fixed power problem


	bit_set(TRIS_SENSOR_I2C_SDA);
	bit_set(TRIS_SENSOR_I2C_SCL);

	bit_clear(TRIS_TSOP36236TR_POWER);		// set power pin to output
	output_low(PIN_TSOP36236TR_POWER);		// fixed power problem
	// init serial port for receiving IR commands
	CateyeClipUart1Init();

}

void CateyeClipUart1Init()
{
	bit_clear(PIC_RCSTA1,7);		// disable serial port (power off)
	bit_set(TRIS_LED_RED_POWER);	// pin input (for uart !!)
}

void CateyeClipIROn()
{
	bit_set(PIC_RCSTA1,7);		// enable serial port (power on)
	bit_set(TRIS_LED_RED_POWER);	// needs to be done (because of uart tx pin, normally output!)
	IRSensorOn();
}

void CateyeClipIROff()
{
	bit_clear(PIC_RCSTA1,7);		// enable serial port (power on)
	IRSensorOff();
}

void CateyeClipIRInit()
{
	PIC_SPBRG1=129;				// 2400bit
	bit_set(PIC_TRISC,6);		// pins are input!, uart controls them
	bit_set(PIC_TRISC,7);		// that's not true! PIN_C6 always input
	bit_clear(PIC_TXSTA1,6);	// 8 bit transmit
	bit_clear(PIC_TXSTA1,2);		// set low speed
	bit_clear(PIC_TXSTA1,4);	// set asynchronous mode
	bit_clear(PIC_TXSTA1,5);		// transmit disabled
	bit_clear(PIC_RCSTA1,6);	// 8 bit reception
	bit_set(PIC_RCSTA1,4);		// enable receiver
	bit_set(PIC_RCSTA1,7);		// enable serial port (power on)
	IRSensorInit();
}


#define SSimpLEDAmberOn() CateyeClipLEDRedOn()
#define SSimpLEDAmberOff() CateyeClipLEDRedOff()

void CateyeClipLEDRedOn()
{
	// see uart init (do not modify!)
	bit_clear(TRIS_LED_RED_POWER);
	output_high(PIN_LED_RED_POWER);
}

void CateyeClipLEDRedOff()
{
	// see uart init (do not modify!)
	bit_set(TRIS_LED_RED_POWER);
}

void CateyeClipLEDGreenOn()
{
	output_high(PIN_LED_GREEN_POWER);
}

void CateyeClipLEDGreenOff()
{
	output_low(PIN_LED_GREEN_POWER);
}

void CateyeClipLEDs(char state)
{
	bit_test(state,0)?output_high(PIN_LED_ONE_POWER):output_low(PIN_LED_ONE_POWER);
	bit_test(state,1)?output_high(PIN_LED_TWO_POWER):output_low(PIN_LED_TWO_POWER);
}

int CateyeClipButtonPressed()
{
	if(input(PIN_BUTTON_OUT)) return 1;
	return 0;
}


void CateyeClipSensorsInit()
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

	#ifdef IR_SENSOR
		CateyeClipIRInit();
	#endif
}

void CateyeClipSensorsOn()
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

	#ifdef IR_SENSOR
		CateyeClipIROn();
	#endif
}

void CateyeClipSensorsOff()
{

	#ifdef LIGHT_SENSOR
		LightSensorOff();
	#endif

	#ifdef AUDIO_SENSOR
		AudioSensorOff();
	#endif

	#ifdef FORCE_SENSOR
		ForceSensorOff();
	#endif

	#ifdef ACCL_SENSOR
		AcclSensorOff();
	#endif

	#ifdef ACCL_Z_SENSOR
		AcclZSensorOff();
	#endif

	#ifdef TEMPERATURE_SENSOR
		TemperatureSensorOff();
	#endif

	#ifdef AMBIENT_LIGHT_SENSOR
		AmbientLightSensorOff();
	#endif

	#ifdef IR_SENSOR
		CateyeClipIROff();
	#endif

	// switch ADC off
	ADCDisable();
}


void CateyeClipActuatorsInit()
{
}

void CateyeClipActuatorsOn()
{
	output_high(PIN_LED_GREEN_POWER);
}

void CateyeClipActuatorsOff()
{
  CateyeClipLEDGreenOff();
  CateyeClipLEDRedOff();
  CateyeClipLEDs(0);
}

#endif // __SSIMP192_C__
