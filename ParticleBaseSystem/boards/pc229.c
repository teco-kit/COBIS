/**
* This is the board file for particle computer communication board 229
*
* file version: 001 (2004-11-03, sabin)
*				identical to pc228.c 002
*
*/


#ifndef __PC229_C__
#define __PC229_C__             001




#inline
void RFSetModeSleep();
#inline
void RFPowerModuleOn();





enum analog_settings {ANALOG_ALL_DIGITAL,ANALOG_CONN_11,ANALOG_CONN_11_12,ANALOG_CONN_11_12_14,ANALOG_CONN_11_12_13_14};
enum port_settings {NORMAL,SELFTEST,ALLIN};

/**
* reflects the possible states for a digital pin
*/
//enum pinstate {low, high, in};

boolean selftest_active;



void PCInit();
int PCGetID(byte* p_id);
int PCSetDigitalPorts(port_settings choice);
int PCSelfTestBoardIsThere();
int PCSelfTest(byte* result);




void PCLedRedOn()
{
	output_high(RED);
}

void PCLedBlueOn()
{
	output_high(BLUE);
}

void PCLedRedOff()
{
	output_low(RED);
}

void PCLedBlueOff()
{
	output_low(BLUE);
}



/**
* Initializes the board.
* Sets connector all pins: digital pins are set to default input,
* analog pins must be defined by the user
*/
void PCInit();

/**
* Sets the analog pins.
* AN0 and AN1 are always set to analog
*
* 0: AN2..AN5 are digital
* 1: AN2 is analog, AN3..AN5 are digital
* 2: AN2..AN3 are analog, AN4..AN5 are digital
* 3: AN2..AN4 are analog, AN5 is digital
* 4: AN2..AN5 are analog
*
* @param config config byte (one of [0..4])
*/
void PCSetAnalogPorts(analog_settings config);

/**
* Sets a given pin to a given digital state.
* Digital state is one of low, high, input.
* @param pin a valid pin define
* @param state one of low, high, input
*
* Note: Analog pins on PORTA are checked and ignored if set to a digital state.
*/
//void PCSetDigitalPin(long pin, pinstate state);


void PCInit()
{


	ADCInit();

	PCSetDigitalPorts(NORMAL);
	PCSetAnalogPorts(ANALOG_ALL_DIGITAL);

	FlashInit();
	BuzzerInit();
	BuzzerPlayASong(1);		// the particle start song
	IDChipInit();
	BallswitchInit();
	//PCI2CInit();
	//PCEepromInit();

	//bit_clear(TRIS_RF_TX);		//set RF TX to output ... macht ports setting schon

	selftest_active=false;
}



int PCSetDigitalPorts(port_settings choice)
{
	switch(choice)
	{
		case NORMAL:

			PIC_TRISA=0b11101111;	PIC_PORTA=0b00000000;
			PIC_TRISB=0b11111111;	PIC_PORTB=0b00000000;
			PIC_TRISC=0b11010011;	PIC_PORTC=0b00000000;
			PIC_TRISD=0b01110011;	PIC_PORTD=0b00001100;
			PIC_TRISE=0b11001100;	PIC_PORTE=0b00000000;
			PIC_TRISF=0b00001011;	PIC_PORTF=0b00000000;
			PIC_TRISG=0b11100100;	PIC_PORTG=0b00000000;
			// the defaults for A/D Port
			//PIC_ADCON0=0b00000001;		// Fosc/64;ch=AN0;stop;turnon
			//PIC_ADCON1=0b00001011;		// Fosc/64;Aport is all analog ref=Vdd; left jsutified
			//PIC_ADCON2=0b00000110;
			break;

		case SELFTEST:

			PIC_TRISA=0b11010011;	PIC_PORTA=0b00000000;
			PIC_TRISB=0b11111111;	PIC_PORTB=0b00000000;
			PIC_TRISC=0b11000011;	PIC_PORTC=0b00000000;
			PIC_TRISD=0b00000010;	PIC_PORTD=0b10000100;
			PIC_TRISE=0b00001100;	PIC_PORTE=0b00000000;
			PIC_TRISF=0b00011010;	PIC_PORTF=0b00000000;
			PIC_TRISG=0b11100100;	PIC_PORTG=0b00000000;
			//set all digital
			//PIC_ADCON0=0b00000000;		// Fosc/64;ch=AN0;stop;turnoff
			//PIC_ADCON1=0b00001111;		// all digital; vrefs are supply
			//PIC_ADCON2=0b00000110;		// whatever
			break;

		case ALLIN:
			PIC_TRISA=0b11111111;	PIC_PORTA=0b11111111;
			PIC_TRISB=0b11111111;	PIC_PORTB=0b11111111;
			PIC_TRISC=0b11111111;	PIC_PORTC=0b11111111;
			PIC_TRISD=0b11111111;	PIC_PORTD=0b11111111;
			PIC_TRISE=0b11111111;	PIC_PORTE=0b11111111;
			PIC_TRISF=0b11111111;	PIC_PORTF=0b11111111;
			PIC_TRISG=0b11111111;	PIC_PORTG=0b11111111;
			//set all digital
			//PIC_ADCON0=0b00000000;		// Fosc/64;ch=AN0;stop;turnoff
			//PIC_ADCON1=0b00001111;		// all digital; vrefs are supply
			//PIC_ADCON2=0b00000110;		// whatever

			break;
	}

	return 1;
}




//Attention: this function sets tris to different values
int PCSelfTestBoardIsThere()
{
	byte remember_TRISD;
	byte remember_TRISB;
	byte remember_PORTB;
	byte remember_PORTD;

	boolean isthere;

	remember_TRISD=PIC_TRISD;		//store old values
	remember_TRISB=PIC_TRISB;
	remember_PORTD=PIC_PORTD;		//store old values
	remember_PORTB=PIC_PORTB;


	bit_set(PIC_TRISB,7);
	bit_clear(PIC_TRISD,5);

	//check to see if selftestboard is there

	isthere=true;

	output_high(PIN_D5);
	DelayUs(10);
	if (!input(PIN_B7)) isthere=false;
	output_low(PIN_D5);
	DelayUs(10);
	if (input(PIN_B7)) isthere=false;


	PIC_TRISD=remember_TRISD;		//return to old values
	PIC_TRISB=remember_TRISB;
	PIC_PORTD=remember_PORTD;		//return to old values
	PIC_PORTB=remember_PORTB;

	if (isthere==true)
	{
		selftest_active=true;
		return 1;
	}
	else
	{
		selftest_active=false;
		return 0;
	}
}





//return value 0: no selftest board found, 1: selftest board is there
//
//  int*result: a 10 byte array is expected
#use RS232 (baud = 19200, XMIT = PIN_B1, RCV = PIN_B2, FORCE_SW)
int PCSelfTest(byte* result)
{

	int buff,nirvana,i;


	PCSetDigitalPorts(SELFTEST);		//set ports to selftest
	PCSetAnalogPorts(ANALOG_ALL_DIGITAL);

	PCLedRedOff();

	PCLedBlueOn();
 	DelayMs(100);
 	PCLedBlueOff();
	DelayMs(100);
	PCLedBlueOn();
 	DelayMs(100);
 	PCLedBlueOff();
	DelayMs(100);


	PCLedBlueOn();		//start selftest



//new:
// 09-18,
// B7-D5,B6-D4,B5-C4,B4-C3,B3-F0,B2-A3,B1-A2,B0-A5  B2-TX,B1-RX
// 1     0     1     0     1     0     1     0 (result0)
// 0     1     0     1     0     1     0     1 (result1)
//----------------------------------------------------

output_high(PIN_CONN_18);
output_low(PIN_CONN_17);
output_high(PIN_CONN_16);
output_low(PIN_CONN_15);
output_high(PIN_CONN_13);
output_low(PIN_CONN_12);
output_high(PIN_CONN_11);
output_low(PIN_CONN_14);

result[0]=PIC_PORTB; //expected: 10101010

output_low(PIN_CONN_18);
output_high(PIN_CONN_17);
output_low(PIN_CONN_16);
output_high(PIN_CONN_15);
output_low(PIN_CONN_13);
output_high(PIN_CONN_12);
output_low(PIN_CONN_11);
output_high(PIN_CONN_14);

result[1]=PIC_PORTB;  //expected: 0b01010101

//----------------------------------------------------
//now serial
PIC_TRISA=255;
PIC_TRISB=0b11111101;
output_high(PIN_B1);			// idle for serial

// uart init
#asm
	movlw	0x40			//129: "19200"	 //movlw 	0x15			// 0d21 =56.818 kbit ="57600"
	movwf 	PIC_SPBRG1		//

	bcf		PIC_TXSTA1,6		// 8 bit transmit
	bsf		PIC_TXSTA1,2		// set high  speed
	bcf		PIC_TXSTA1,4		// set asynchronous mode
	bsf		PIC_TXSTA1,5		// transmit enabled


	bsf		PIC_RCSTA1,7		// enable serial port (power on)
	bcf		PIC_RCSTA1,6		// 8 bit reception
	bsf     PIC_RCSTA1,4		// enable receiver


	bcf		PIC_TRISC,6		//c6 is output for tx
	bsf		PIC_TRISC,7		//c7 is input for rx

#endasm



//check Framing error, if so, read receivebuffer
if (bit_test(PIC_RCSTA1,2))
{
	buff=PIC_RCREG1;
}

//check Overrun error, if so, clear CREN bit
if (bit_test(PIC_RCSTA1,1))
{
	bit_clear(PIC_RCSTA1,4);
}

// set CREN (Continous Receive ENable)
bit_set(PIC_RCSTA1,4);

PIC_TXREG1=255;							// just any data

//FLUSH rx
if (bit_test(PIC_PIR1,5))  nirvana=PIC_RCREG1;
if (bit_test(PIC_PIR1,5))  nirvana=PIC_RCREG1;

//send one byte hardware -> software serial
//while (!(bit_test(PIC_PIR1,4))) ; 		// wait for free
while (!(bit_test(PIC_TXSTA1,1))) ; 		// wait for free TRMT
DelayUs(250);
DelayUs(250);
//PCLedRedOn();
//DelayMs(100);

buff=255;
PIC_TXREG1=139;							// just any data

while ((buff--) && !kbhit());

if (kbhit()) {buff=getch();result[2]=buff;} else result[2]=255;

//PCLedRedOff();
//DelayMs(100);


//send one byte sortware -> hardware serial
putc(117);		// now send on software serial
DelayUs(100);
if (bit_test(PIC_PIR1,5)) result[3]=PIC_RCREG1; else result[3]=255;

//
//----------------------------------------------------
// now analog
//PIC_ADCON0=PIC_ADCON0_SYSTEM_VALUE;
//PIC_ADCON1=PIC_ADCON1_SYSTEM_VALUE;
//PIC_ADCON2=PIC_ADCON2_SYSTEM_VALUE

result[4]=0;//AppGetSupplyVoltage();

//
//----------------------------------------------------
// now I2C, FRAM
result[5]=0;
PIC_TRISD=0b00000111;
PIC_TRISB=255;


RFPowerModuleOn();
RFSetModeSleep();

//the flash test here:
SPIInit(SPI_MODE_FAST);
FlashInit();
FlashWriteToBuffer1(55,28);
result[5]=0;
result[5]=FlashReadFromBuffer1(55);


//result[5]=28;

//PCLedRedOff();

//
//----------------------------------------------------
// now show result

	PIC_TRISB=0b10111111;
	buff=0;
	if (result[0]==0b10101010) buff++;	//170

	//debuggiveout(result[0]);

	if (result[1]==0b01010101) buff++;	//85

//		debuggiveout(result[1]);

	if (result[2]==139) buff++;

//		debuggiveout(result[2]);

	if (result[3]==117) buff++;

//		debuggiveout(result[3]);

	if (result[5]==28) buff++;

	//debuggiveout(result[5]);


	if (buff==5) result[9]=0; else result[9]=255;

	DelayMs(800);
	PCLedBlueOff();
	DelayMs(800);

	//PCLedBlueOn();DelayMs(200);PCLedBlueOff();DelayMs(200);
	//PCLedBlueOn();DelayMs(200);PCLedBlueOff();DelayMs(200);
	//PCLedBlueOn();DelayMs(200);PCLedBlueOff();DelayMs(200);

	if (result[9]==0) PCLedBlueOn(); else PCLedRedOn();
	DelayMs(800);
	PCLedBlueOff();
	PCLedRedOff();




return 1;

}



void PCSetAnalogPorts(analog_settings config)
{
        byte temp;

        // delete bits 0-3
        temp = PIC_ADCON1 & 0b11110000;

        // set bits 0-3


        switch(config)
        {
                case ANALOG_ALL_DIGITAL:
                      ADCConfig(AN0_AN1);  //PIC_ADCON1 = temp | 0b00001101;
                break;
                case ANALOG_CONN_11:
                      ADCConfig(AN0_AN1_AN2);//  PIC_ADCON1 = temp | 0b00001100;
                break;
                case ANALOG_CONN_11_12:
                      ADCConfig(AN0_AN1_AN2_AN3);  //PIC_ADCON1 = temp | 0b00001011;
                break;
                case ANALOG_CONN_11_12_14:
                       ADCConfig(AN0_AN1_AN2_AN3_AN4);  //PIC_ADCON1 = temp | 0b00001010;	//an0-an4 analog
                break;
                case ANALOG_CONN_11_12_13_14:
                        ADCConfig(AN0_AN1_AN2_AN3_AN4_AN5); //PIC_ADCON1 = temp | 0b00001001;	//an0-AN5 analog
                break;
        }
}





/// for RPC Server:: match UART to RPC


int RPCServerSerialWaitForRx(long wait_time)
{
	return Uart1WaitForRx(wait_time);
}

void RPCServerSerialEnable()
{
	Uart1Enable();
}
void RPCServerSerialDisable()
{
	Uart1Disable();
}
void RPCServerSerialInit(serial_speed speed, serial_mode sync_mode, int bits)
{
	Uart1Init(speed, sync_mode, bits);
}
int RPCServerSerialCheckError()
{
	return Uart1CheckError();
}
void RPCServerSerialFlushReceive()
{
	Uart1FlushReceive();
}
void RPCServerSerialWaitForTxEnd()
{
	Uart1WaitForTxEnd();
}
int RPCServerSerialDataIsThere()
{
	return Uart1DataIsThere();
}
void RPCServerSerialSendByte(byte data)
{
	Uart1SendByte(data);
}
int RPCServerSerialGetByte()
{
	return Uart1GetByte();
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


/*
 *
 *	Power RF Module
 *
 */
#inline
void RFPowerModuleOn()
{
	output_low(PIN_RF_POWER);	//inverse logic!!
	output_high(PIN_POTI_POWER);
}

#inline
void RFPowerModuleOff()
{
	output_high(PIN_RF_POWER);	//inverse logic!!
	output_low(PIN_POTI_POWER);
}



int PCGetID(byte* p_id)
{
	return IDChipGetId(p_id);		// get the id and write it to p_id
}






#endif // __PC229_C__




