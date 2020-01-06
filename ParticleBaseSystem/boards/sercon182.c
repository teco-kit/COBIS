/**
* board file for the seRCON board 182
*
* Author: krohn@teco.edu
* Date: 2004-04-14
*
*/


#ifndef __SERCON182_C__
#define	__SERCON182_C__					001


enum serial_software_speed{SERCON_115200,SERCON_19200,SERCON_9600};

serial_software_speed sercon_speed;

/**
* Initializes the SeRCON connector board
*/
void SerConInit();

/**
* Initializes the SeRCON connector board
*/
int SerConSetMode(serial_software_speed speed);


/**
* returns the CTS state
*/
boolean SerConCTS();

/**
* sets the rts signal
*/
void SerConRTS();

/**
* clears the RTS signal (no rts)
*/
void SerConNoRTS();




void SerConInit()
{
	bit_clear(TRIS_SERCON_TX);		//tx is output
	bit_set(TRIS_SERCON_RX);		//rx is input
	bit_clear(TRIS_SERCON_RTS);		//rts is output
	bit_set(TRIS_SERCON_CTS);		//cts is input

}




int SerConSetMode(serial_software_speed speed)
{
	sercon_speed=speed;
}


void SerCon(char c)
{

	switch(sercon_speed)
	{
		case SERCON_115200:
			#use RS232 (baud = 115200, XMIT = PIN_SERCON_TX, RCV = PIN_SERCON_RX, FORCE_SW)
			putc(c);
		break;

		case SERCON_9600:
			#use RS232 (baud =   9600, XMIT = PIN_SERCON_TX, RCV = PIN_SERCON_RX, FORCE_SW)
			putc(c);
		break;

		case SERCON_19200:
			#use RS232 (baud =  19200, XMIT = PIN_SERCON_TX, RCV = PIN_SERCON_RX, FORCE_SW)
			putc(c);
		break;

	}

}

char SerConGetC()
{
	switch(sercon_speed)
	{
		case SERCON_115200:
			#use RS232 (baud = 115200, XMIT = PIN_SERCON_TX, RCV = PIN_SERCON_RX, FORCE_SW)
			return getc();
		break;

		case SERCON_9600:
			#use RS232 (baud =   9600, XMIT = PIN_SERCON_TX, RCV = PIN_SERCON_RX, FORCE_SW)
			return getc();
		break;

		case SERCON_19200:
			#use RS232 (baud =  19200, XMIT = PIN_SERCON_TX, RCV = PIN_SERCON_RX, FORCE_SW)
			return getc();
		break;

	}


}


boolean SerConCTS()
{
	if (input(PIN_SERCON_CTS)) return true; else return false;
}

void SerConRTS()
{
	output_high(PIN_SERCON_RTS);
}

void SerConNoRTS()
{
	output_low(PIN_SERCON_RTS);
}




#endif