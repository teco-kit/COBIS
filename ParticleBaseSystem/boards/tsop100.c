/**
* Board driver for TSOP36236TR
*
* Provides basic functions for the TSOP36236TR IR sensor board.
*
* Author: Christian Decker (cdecker@teco.edu)
* Date: 2005-04-01 (yyyy-mm-dd)
*
* version 001 (2005-04-01, cdecker)
*   - initial revision
*
*/

#ifndef __TSOP100_C__
#define __TSOP100_C__		001


/**
* Switches the force sensor on
*/
#define TSOP100SensorsOn()		output_high(PIN_TSOP36236TR_POWER)

/**
* Switches the force sensor off
*/
#define TSOP100SensorsOff()		output_low(PIN_TSOP36236TR_POWER)

/**
* Init the UART1 on DigiClip
*/
void TSOP100Uart1Init();


/**
* Initializes the TSOP36236TR sensor board.
* Sets power to output, initialize the UART.
*
* @return returns 0 if no error
*/
void TSOP100Init()
{
	bit_clear(TRIS_TSOP36236TR_POWER);		// set power pin to output
	// init serial port for receiving IR commands
	TSOP100Uart1Init();
}

/**
* Init the UART1 on DigiClip
*/
void TSOP100Uart1Init()
{

		PIC_SPBRG1=129;			// 2400bit
		bit_set(PIC_TRISC,6);		// pins are input!, uart controls them
		bit_set(PIC_TRISC,7);
		bit_clear(PIC_TXSTA1,6);	// 8 bit transmit
		bit_clear(PIC_TXSTA1,2);		// set low speed
		bit_clear(PIC_TXSTA1,4);	// set asynchronous mode
		bit_set(PIC_TXSTA1,5);		// transmit enabled
		bit_clear(PIC_RCSTA1,6);	// 8 bit reception
		bit_set(PIC_RCSTA1,4);		// enable receiver
		bit_set(PIC_RCSTA1,7);		// enable serial port (power on)
}


#endif //__TSOP100_C__