/**
* This is the board file for particle computer sensor board 202
*
* file version: 001 (2004-01-21, sabin (wendhack@teco.edu))
*  - pc202.c adapted to spart
*
*/


#ifndef __SPart202_C__
#define __SPart202_C__             001






enum analog_settings {ANALOG_AN0_AN1_AN2_AN3};
enum port_settings {NORMAL,ALLIN};
/**
* reflects the possible states for a digital pin
*/
//enum pinstate {low, high, in};

boolean selftest_active;



void SPartInit();
int SPartSetDigitalPorts(port_settings choice);
void SPartSetAnalogPorts(analog_settings config);
void SPartSensorsInit();
void SPartSensorsOn();
int SPartSelfTestBoardIsThere();
int SPartSelfTest(byte* result);




void SPartLedRedOn()
{
	output_high(RED);
}

void SPartLedBlueOn()
{
	bit_clear( PIC_TRISD, 3 );
	bit_set( PIC_PORTD, 3 );
//	output_high(BLUE);
}

void SPartLedsOn()
{
	bit_clear( PIC_TRISD, 3 );
	output_high(RED);
	output_high(BLUE);
}

void SPartLedRedOff()
{
	output_low(RED);
}

void SPartLedBlueOff()
{
	output_low(BLUE);
}

void SPartLedsOff()
{
	output_low(RED);
	output_low(BLUE);
}



void SPartInit()
{
	ADCInit();

	SPartSetDigitalPorts(NORMAL);
	SPartSetAnalogPorts(ANALOG_AN0_AN1_AN2_AN3);

	SPartEepromInit();

	selftest_active=false;

	bit_clear(TRIS_ACCL_ADXL_POWER);
	output_high(PIN_ACCL_ADXL_POWER);
	bit_set(TRIS_ACCL_ADXL_XOUT);
	bit_set(TRIS_ACCL_ADXL_YOUT);

	bit_clear(TRIS_ACCL_ADXL_POWER_Z);
	output_high(PIN_ACCL_ADXL_POWER_Z);
	bit_set(TRIS_ACCL_ADXL_ZOUT);

	bit_clear(TRIS_LIGHT_TSL250_POWER_A);
	output_high(PIN_LIGHT_TSL250_POWER_A);

	bit_clear(TRIS_LIGHT_TSL250_POWER_B);
	output_high(PIN_LIGHT_TSL250_POWER_B);

	bit_clear(TRIS_LIGHT_TSL2550_POWER);
	output_high(PIN_LIGHT_TSL2550_POWER);

	bit_set(TRIS_LIGHT_TSL2550_SDA);
	bit_set(TRIS_LIGHT_TSL2550_SCLK);

	bit_clear(TRIS_TEMP_TC74_POWER);
	output_high(PIN_TEMP_TC74_POWER);

	bit_set(TRIS_TEMP_TC74_SDA);
	bit_set(TRIS_TEMP_TC74_SCLK);

	bit_clear(TRIS_FORCE_FSR_POWER);
	output_high(PIN_FORCE_FSR_POWER);

	bit_set(TRIS_FORCE_FSR_OUT);

	bit_clear(TRIS_AUDIO_MIC_POWER);
	output_high(PIN_AUDIO_MIC_POWER);

	bit_set(TRIS_AUDIO_MIC_OUT);

	bit_clear(TRIS_SENSOR_I2C_POWER);
	output_high(PIN_SENSOR_I2C_POWER);

	bit_set(TRIS_SENSOR_I2C_SDA);
	bit_set(TRIS_SENSOR_I2C_SCL);

}


int SPartSetDigitalPorts(port_settings choice)
{
	switch(choice)
	{
		case NORMAL:

			PIC_TRISA=0b11111111;	PIC_PORTA=0b00000000;
			PIC_TRISB=0b11101111;	PIC_PORTB=0b00000000;
			PIC_TRISC=0b11011011;	PIC_PORTC=0b00000100;
			PIC_TRISD=0b10010100;	PIC_PORTD=0b00000000;
			PIC_TRISE=0b00000111;	PIC_PORTE=0b00000000;

			break;


		case ALLIN:
			PIC_TRISA=0b11111111;	PIC_PORTA=0b11111111;
			PIC_TRISB=0b11111111;	PIC_PORTB=0b11111111;
			PIC_TRISC=0b11111111;	PIC_PORTC=0b11111111;
			PIC_TRISD=0b11111111;	PIC_PORTD=0b11111111;
			PIC_TRISE=0b00000111;	PIC_PORTE=0b11111111;

			break;
	}
}


void SPartSetAnalogPorts(analog_settings config)
{
	ADCConfig(AN0_AN1_AN2_AN3);  //PIC_ADCON1 = temp | 0b00001010;	//an0-an4 analog
}


//Attention: this function sets tris to different values
int SPartSelfTestBoardIsThere()
{
		selftest_active=false;
		return 0;
}





//return value 0: no selftest board found, 1: selftest board is there
//
//  int*result: a 10 byte array is expected
#use RS232 (baud = 19200, XMIT = PIN_B1, RCV = PIN_B2, FORCE_SW)
int SPartSelfTest(byte* result)
{
}


void SPartSensorsInit()
{
	#ifdef LIGHT_SENSOR_A
		LightSensorInit_A();		//inits the light and sets the tris etc..
	#endif

	#ifdef LIGHT_SENSOR_B
		LightSensorInit_B();		//inits the light and sets the tris etc..
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


void SPartSensorsOn()
{
	#ifdef LIGHT_SENSOR_A
		LightSensorOn_A();
	#endif

	#ifdef LIGHT_SENSOR_B
		LightSensorOn_B();
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



/// for RPC Server:: match Uart to RPC


int RPCClientSerialWaitForRx(long wait_time)
{
	return UartWaitForRx(wait_time);
}

void RPCClientSerialEnable()
{
	UartEnable();
}
void RPCClientSerialDisable()
{
	UartDisable();
}
void RPCClientSerialInit(serial_speed speed, serial_mode sync_mode, int bits)
{
	UartInit(speed, sync_mode, bits);
}
int RPCClientSerialCheckError()
{
	return UartCheckError();
}
void RPCClientSerialFlushReceive()
{
	UartFlushReceive();
}
void RPCClientSerialWaitForTxEnd()
{
	UartWaitForTxEnd();
}
int RPCClientSerialDataIsThere()
{
	return UartDataIsThere();
}
void RPCClientSerialSendByte(byte data)
{
	UartSendByte(data);
}
int RPCClientSerialGetByte()
{
	return UartGetByte();
}



void ClockTimerInit(timer_config conf)
{
	 Timer3Init(conf);
}

int ClockTimerGetHi()
{
	return Timer3GetHi();
}
int ClockTimerGetLo()
{
	return Timer3GetLo();
}

void ClockTimerSetHi(int val)
{
	Timer3SetHi(val);
}
void ClockTimerSetLo(int val)
{
	Timer3SetLo(val);
}


#endif // __SPart202_C__




