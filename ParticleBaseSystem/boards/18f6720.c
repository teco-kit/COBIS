/**
* Processor file for 18f6720.
* Contains processor specific funtions like ADC control
*
* file version: 001 (2004-04-08, sabin (wendhack@teco.edu))
* 				002 2004,06,20 albert, including spi
*				003
*				004 2204 07 07 albert, spi modified to speed up
*				005 2004-10-10 uwe, DelayUs renew
*				006 2004-11-07 sabin, DelayUs new. Inserted global mue with reserved register.
*				007	2004_11_10 sabin, DelayUsLong new. Added global mue_hi with reserved register.
*				008 2004-12-22 sabin, inserted ADCInProgress().
*/

//ADCInProgress() addded.


#ifndef __18f6720_C__
#define __18f6720_C__                           008


// ensures aquisition time
#define SAVETIMINGS
//#define boolean BOOLEAN
//#define byte BYTE

//this is the file for functions of the processor


// all other
void WriteIntEeprom(int address, int data);
int ReadIntEeprom(int address);
#inline
void DelayMs(long Minizekunden);
#inline
void DelayUs( unsigned int muesekunden );
#inline
void DelayUsLong( unsigned long muesekunden );


// to make DelayUs as independent as possible from ccs compiler........
#reserve	0x33
#byte		mue_hi = 0x33
#reserve	0x34
#byte		mue_lo = 0x34

#define hi(x)  (*(&x+1))
#define lo(x)  (*(&x))
#define EEPROM_ADDRESS_ID			0		// start addresss for ID in internal EEPROM


enum timer_config{CLOCK_32kHz,AWARECON_SLOT_TIMER,AWARECON_BIT_TIMER};
enum analog_config
{
	NONE,
	AN0,
	AN0_AN1,
	AN0_AN1_AN2,
	AN0_AN1_AN2_AN3,
	AN0_AN1_AN2_AN3_AN4,
	AN0_AN1_AN2_AN3_AN4_AN5
};


#define PIN_0_ON	#asm   bsf PIC_PORTB,0 #endasm
#define PIN_0_OFF   #asm   bcf PIC_PORTB,0 #endasm
#define PIN_1_ON	#asm   bsf PIC_PORTB,1 #endasm
#define PIN_1_OFF   #asm   bcf PIC_PORTB,1 #endasm
#define PIN_2_ON	#asm   bsf PIC_PORTB,2 #endasm
#define PIN_2_OFF   #asm   bcf PIC_PORTB,2 #endasm
#define PIN_3_ON	#asm   bsf PIC_PORTB,3 #endasm
#define PIN_3_OFF   #asm   bcf PIC_PORTB,3 #endasm
#define PIN_4_ON	#asm   bsf PIC_PORTB,4 #endasm
#define PIN_4_OFF   #asm   bcf PIC_PORTB,4 #endasm
#define PIN_5_ON	#asm   bsf PIC_PORTB,5 #endasm
#define PIN_5_OFF   #asm   bcf PIC_PORTB,5 #endasm
#define PIN_6_ON	#asm   bsf PIC_PORTB,6 #endasm
#define PIN_6_OFF   #asm   bcf PIC_PORTB,6 #endasm
#define PIN_7_ON	#asm   bsf PIC_PORTB,7 #endasm
#define PIN_7_OFF   #asm   bcf PIC_PORTB,7 #endasm


#inline
void DelayMs(long Minizekunden)  //ZEKUNDEN ZZZZEKUNDEN
{
	for (;Minizekunden;Minizekunden--)
	{
		DelayUs(250);
		DelayUs(250);
		DelayUs(250);
		DelayUs(250);
	}
}


// works only with 3 microsecs and more. absolutely 0,2 microsecs too much.
#inline
void DelayUs( unsigned int muesekunden )
{
		#asm
		movff	muesekunden, mue_lo
		movlw	2
		subwf	mue_lo
		nop
		nop
		nop
		nop

	HERE:
		nop
		nop
		decfsz 	mue_lo
		goto 	HERE
		#endasm
}


// 0,2 microsecs too much per muesekunden_hi.
#inline
void DelayUsLong( unsigned long muesekunden )
{
		muesekunden -= 3;
		mue_lo = ( int )muesekunden;
		mue_hi = ( int )( muesekunden >> 8 );
		++mue_hi;

		#asm
	LOW:
		nop
		nop

	HIGH:
		decfsz 	mue_lo
		goto 	LOW
		decfsz	mue_hi
		goto 	HIGH
		#endasm
}


void WriteIntEeprom(int address, int data)
{
	bit_clear(PIC_EECON1, CFGS); 					//access prog or data memory
	bit_clear(PIC_PIR2, EEIF); 					//make sure that the int flag is cleared
	PIC_EEADR = address; 						//load address
	PIC_EEDATA = data; 						//load data
	bit_clear(PIC_EECON1, EEPGD); 					//point to data memory
	bit_set(PIC_EECON1, WREN); 					//enable write

	PIC_EECON2 = 0x55; 						//write 0x55
	PIC_EECON2 = 0xAA; 						//write 0xAA
	bit_set(PIC_EECON1, WR); 						//set WR to begin write


	while (!bit_test(PIC_PIR2, EEIF));				//wait for the write to complete
	bit_clear(PIC_EECON1, WREN);					//disable writes
	bit_clear(PIC_PIR2, EEIF);
}


int ReadIntEeprom(int address)
{
	bit_clear(PIC_EECON1, CFGS); 					//access data memory
	PIC_EEADR = address; 						//load address
	bit_clear(PIC_EECON1, EEPGD); 					//point to data memory
	bit_set(PIC_EECON1, RD); 						//enable read

	return(PIC_EEDATA); 							//return data
}



/**
* Enables the A/D converter
*/
void ADCEnable();

/**
* Checks whether the given channel is already set for
* A/D conversion.
* @param channel channel to check
* @return true if channel is already set
*/
boolean ADCIsChSet(int channel);

/**
* Sets a given channel for A/D conversion
* @param channel channel for conversion
*/
void ADCSetCh(int channel);

/**
* Sets the sampling speed for A/D conversion
* @param speed sampling rate
*
* Note: the parameter is currently ignored. The sampling speed is set to maximum
* for 18f6620 at 20 MHz
*/
void ADCSetClock(int speed);

/**
* Reads a sampled value from the A/D converter. Only one byte.
* @return sampled value as byte
*/
byte ADCRead8();

/**
* Reads a sampled value from the A/D converter. Full 10bit of the ADC
* @return sampled value as long, where the bit 3-7 are set to 0.
*/
long ADCRead10();




/**
* Initialises the A/D converter
* Set some reasonable values
*/
void ADCInit();

/**
* Disables the A/D converter
*/
void ADCDisable();


void ADCDisable()
{
        bit_clear(PIC_ADCON0,0);                // disable A/D
}

void ADCInit()
{
        PIC_ADCON0 = 0b00000000; // AN0 channel selected
        PIC_ADCON1 = 0b00001101; // AN0 and AN1 are analog
        PIC_ADCON2 = 0b00000110; // left-justified, fosc/64 highest sampling speed fro 20 MHz
}

void ADCConfig(analog_config conf)
{

	switch(conf)
	{
		case 0:
			PIC_ADCON1=0b00001111;
		break;
		case AN0:
			PIC_ADCON1=0b00001110;
		break;
		case AN0_AN1:
			PIC_ADCON1=0b00001101;
		break;
		case AN0_AN1_AN2:
			PIC_ADCON1=0b00001100;
		break;
		case AN0_AN1_AN2_AN3:
			PIC_ADCON1=0b00001011;
		break;
		case AN0_AN1_AN2_AN3_AN4:
			PIC_ADCON1=0b00001010;
		break;
		case AN0_AN1_AN2_AN3_AN4_AN5:
			PIC_ADCON1=0b00001001;
		break;
	}

}


void ADCEnable()
{
        bit_set(PIC_ADCON0,0);          // enable A/D
}


boolean ADCIsChSet(int channel)
{
        byte actual;

        actual=((PIC_ADCON0&0b00111100)>>channel);
        return (actual!=channel)?0:1;
}


void ADCSetCh(int channel)
{
        PIC_ADCON0&=0b00000000;
        PIC_ADCON0|=(channel<<2);

        #ifdef SAVETIMINGS
        DelayUs(50);                                           //find out correct time?????
        #endif
}

void ADCSetClock(int speed)
{
        PIC_ADCON2&=0b11111000;                                 // fosc/64 fastest sampling rate for 20 Mhz
        PIC_ADCON2|=0b00000110;
}

byte ADCRead8()
{
        byte value;

        PIC_ADCON2&=0b01111111;                         // set result to left-justified

        bit_set(PIC_ADCON0,1);                          // set GO/~DONE - start conversion
        while (bit_test(PIC_ADCON0,1)) ;        // wait for ready
        value=PIC_ADRESH;
        return value;
}

long ADCRead10()
{
        long value;
        byte temp;

        PIC_ADCON2|=0b10000000;                         // set to right-justified

        bit_set(PIC_ADCON0,1);                          // set GO/~DONE - start conversion
        while (bit_test(PIC_ADCON0,1)) ;                // wait for ready

        value = 0;
        value=PIC_ADRESH;
        value = value << 8;
        temp = PIC_ADRESL;
        value |= ((long)temp) & 0x00FF;
        return value;
}

unsigned int ADCInProgress()
{
	return bit_test( PIC_ADCON0,1 );
}


//UART1

enum serial_speed{SERIAL_625000,SERIAL_19200};
enum serial_mode{SERIAL_ASYNCHRONOUS,SERIAL_SYNCHRONOUS};



int Uart1WaitForRx(long wait_time);
void Uart1Enable();
void Uart1Disable();
void Uart1Init(serial_speed speed, serial_mode sync_mode, int bits);
int Uart1CheckError();
void Uart1FlushReceive();
void Uart1WaitForTxEnd();
int Uart1DataIsThere();
void Uart1SendByte(byte data);
byte Uart1GetByte();



//void RPCSerialSendByte(int data)
void Uart1SendByte(byte data)
{
	while (!(bit_test(PIC_PIR1,4))) ; 		// wait for free	TX1IF
	PIC_TXREG1=data;
}

byte Uart1GetByte()
{
	return PIC_RCREG1;
}


//int RPCSerialDataIsThere()
int Uart1DataIsThere()
{
	if (bit_test(PIC_PIR1,5)) return 1; else return 0;
}

int Uart1TxBusy()
{
	return !bit_test(PIC_TXSTA1,1);
}

//void RPCSerialWaitForTxEnd()
void Uart1WaitForTxEnd()
{
	while (!bit_test(PIC_TXSTA1,1));
}



// deletes all crap in the buffer
//void RPCSerialFlushUart()
void Uart1FlushReceive()
{
	int nirvana;
	if (Uart1DataIsThere()) nirvana=PIC_RCREG1;
	if (Uart1DataIsThere()) nirvana=PIC_RCREG1;

}



//int RPCSerialUartCheckError()
int Uart1CheckError()
{
	int error;
	int buff;

	error=0;
	//check Framing error, if so, read receivebuffer
	if (bit_test(PIC_RCSTA1,2))
	{
		buff=PIC_RCREG1;
		bit_set(error,0);
	}

	//check Overrun error, if so, clear CREN bit (this is for asynchronous mode)
	if (bit_test(PIC_RCSTA1,1))
	{
		bit_clear(PIC_RCSTA1,4);
		bit_set(PIC_RCSTA1,4);
		bit_set(error,1);
	}

	return error;
}


//void RPCSerialInitUart()
// inits and turns on
void Uart1Init(serial_speed speed, serial_mode sync_mode, int bits)
{
//	output_high( pin_led_1 );

	if ((speed==SERIAL_625000) && (sync_mode==SERIAL_ASYNCHRONOUS) && (bits==8))
	{
		PIC_SPBRG1=0x01;			// 625kbit
		bit_set(PIC_TRISC,6);		// pins are input!, uart controls them
		bit_set(PIC_TRISC,7);
		bit_clear(PIC_TXSTA1,6);	// 8 bit transmit
		bit_set(PIC_TXSTA1,2);		// set high speed
		bit_clear(PIC_TXSTA1,4);	// set asynchronous mode
		bit_set(PIC_TXSTA1,5);		// transmit enabled
		bit_clear(PIC_RCSTA1,6);	// 8 bit reception
		bit_set(PIC_RCSTA1,4);		// enable receiver
		bit_set(PIC_RCSTA1,7);		// enable serial port (power on)
	}

}



//void RPCSerialDisableUart()
void Uart1Disable()
{
	bit_clear(PIC_RCSTA1,7);		// disable serial port (power off)  //DONT ever play with trisc6,7 during uart operation!!!
	bit_set(PIC_TRISC,6);		//tx in input (to make sure)
	bit_set(PIC_TRISC,7);		//rx is input
	bit_clear(PIC_TXSTA1,5);		//disable tx
	bit_clear(PIC_RCSTA1,4);		// disable rx

}


// checks everything , clears errors and makes the port ready
void Uart1Enable()
{
	Uart1CheckError();
	Uart1FlushReceive();
	Uart1WaitForTxEnd();
	bit_set(PIC_TXSTA1,5);		// transmit enabled
	bit_set(PIC_RCSTA1,4);		// enable receiver
	bit_set(PIC_RCSTA1,7);		// enable serial port (power on)
}



// returns 1 if byte is there in waittime
//int RPCLookForRx(long waittime)
int Uart1WaitForRx(long wait_time)
{
	while (wait_time--)
	{
		Uart1CheckError();
		if (Uart1DataIsThere() ) return 1;
		//if (!input(PIN_RPC_SERIAL_COMM));
	}
	return 0;
}


///TIMER THINGS
//--------------------------------------------------------------------------------------------------------
void Timer3Init(timer_config conf)
{
	switch (conf)
	{
		case CLOCK_32kHz:
			bit_set( PIC_T1CON, 3 );	//TIMER1 oszillator enable (is for timer3 as well)
			PIC_T3CON = 0b01110111;		//timer3: 2*8bit acces,1:8 prescale,no external sync, input from timer 1 oszi, enable timer
			//setting of tris c0,1 for timer oszi is not necessary, oszillator forces them to input
		break;
	}
}

int Timer3GetHi()
{
	return PIC_TMR3H;
}
int Timer3GetLo()
{
	return PIC_TMR3L;
}
void Timer3SetHi(int val)
{
	PIC_TMR3H=val;
}
void Timer3SetLo(int val)
{
	PIC_TMR3L=val;
}






//----------------------------------------------------
// the spi
//only master modes are supported

enum SPI_MODE{SPI_MODE_TIMER2,SPI_MODE_FAST};

void SPISetMode(SPI_MODE mode)
{
	//first set the pins correctly

	if (mode==SPI_MODE_TIMER2)
	{
		// this is the mode for rf
		PIC_SSPSTAT=0;
		PIC_SSPCON1=0x23;
	}

	if (mode==SPI_MODE_FAST)
	{
		//PIC_SSPCON1=0b00110010;	// idle is high, power on, speed in fosc/64
		PIC_SSPCON1=0b00110000;	// idle is high, power on, speed in fosc/4
		PIC_SSPSTAT=0b10000000;	// data is transmitted on rising edge, received on falling edge
	}



}

void SPIInit(SPI_MODE mode)
{

	//first set the pins correctly

	bit_clear(PIC_TRISC,5);		//the sdo
	bit_set(PIC_TRISC,4);		//the sdi
	bit_clear(PIC_TRISC,3);		//the clock

	SPISetMode(mode);

}




// shifts one byte out and the rx in at the same time (typical spi process)
byte SPIShift(byte out)
{
	byte in,dummy;

	dummy=PIC_SSPBUF;
	PIC_SSPBUF=out;
	#asm
			LOOP3:
				BTFSS 	RF_SSPSTAT_BF		// sspstat<bf> wait for free
				GOTO 	LOOP3
	#endasm

	in=PIC_SSPBUF;
	return in;
}












#endif // __18f6620_C__



