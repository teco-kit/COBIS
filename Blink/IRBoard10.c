#define internalStep()	{output_high(INTERNAL_IR_STEP); output_low(INTERNAL_IR_STEP); output_high(INTERNAL_IR_STEP);}
#define externalStep()	{output_high(EXTERNAL_IR_STEP); output_low(EXTERNAL_IR_STEP); output_high(EXTERNAL_IR_STEP);}
#define internalDOWN()	{output_low(INTERNAL_IR_DIR);}		// means: increment each step
#define externalDOWN()	{output_low(EXTERNAL_IR_DIR);}		// means: increment each step
#define internalUP()	{output_high(INTERNAL_IR_DIR);}		// means: decrement each step
#define externalUP()	{output_high(EXTERNAL_IR_DIR);}		// means: decrement each step

// control IR leds
#define internalOn()	{output_high(INTERNAL_IR_ENABLE);}
#define internalOff()	{output_low(INTERNAL_IR_ENABLE);}
#define externalOn()	{output_high(EXTERNAL_IR_ENABLE);}
#define externalOff()	{output_low(EXTERNAL_IR_ENABLE);}


#define INITIAL_IR_STRENGTH		16


// set the IR field strength (1..32), (INTERNAL_IR_LED, EXTERNAL_IR_LED)
void IRSetFieldStrength(unsigned int strength, unsigned int internal)
{
	if (internal==INTERNAL_IR_LED)
	{
		// internal IR LED
		unsigned int s;
		// step down
		internalDOWN();
		for (s=0; s<32; s++)
			internalStep();

		internalUP();
		// step up
		for (s=0; s<strength; s++)
			internalStep();
	}
	else
	{
		// external IR LED
		unsigned int s;
		// step down
		externalDOWN();
		for (s=0; s<32; s++)
			externalStep();

		externalUP();
		// step up
		for (s=0; s<strength; s++)
		externalStep();
	}
}


// set pin directions etc.
void IRBoardInit()
{
	// beacon power control pins
	bit_clear(TRIS_INTERNAL_IR_STEP);
	bit_clear(TRIS_INTERNAL_IR_DIR);
	bit_clear(TRIS_INTERNAL_IR_ENABLE);
	bit_clear(TRIS_EXTERNAL_IR_STEP);
	bit_clear(TRIS_EXTERNAL_IR_DIR);
	bit_clear(TRIS_EXTERNAL_IR_ENABLE);

	// init both ir leds (internal led on - external off)
	externalOff();						// switch off external ir led
	internalOn();						// internal ir led on
	internalDOWN();						// dir: down
	externalDOWN();						// dir: down
	output_high(INTERNAL_IR_STEP);		// init -> high
	output_high(EXTERNAL_IR_STEP);		// init -> high

	// set potis to initial values
	IRSetFieldStrength(INITIAL_IR_STRENGTH, INTERNAL_IR_LED);
	IRSetFieldStrength(INITIAL_IR_STRENGTH, EXTERNAL_IR_LED);


	// init the uart, 2400 bit/s, for IR transmission
	PIC_SPBRG1=129;				// 625kbit ;-) *scherz*
	bit_set(PIC_TRISC,6);		// pins are input!, uart controls them
	bit_set(PIC_TRISC,7);
	bit_clear(PIC_TXSTA1,6);	// 8 bit transmit
	bit_clear(PIC_TXSTA1,2);		// set low speed
	bit_clear(PIC_TXSTA1,4);	// set asynchronous mode
	bit_set(PIC_TXSTA1,5);		// transmit enabled
	bit_clear(PIC_RCSTA1,6);	// 8 bit reception
	bit_set(PIC_RCSTA1,4);		// enable receiver
	bit_set(PIC_RCSTA1,7);		// enable serial port (power on)
	bit_clear(TRIS_CONN_18);	//conn 18 to output
	output_high(PIN_CONN_18);	// safe state (NOR -> high)
}


// send byte over IR
void IRsend38khz(byte b)
{
	disable_interrupts(global);
	Uart1SendByte(b);

//	PIC_TMR1H=0;
//	PIC_TMR1L=0;

	//while (PIC_TMR1H<82)

	while(Uart1TxBusy())
	{
		if (bit_test(PIC_TMR1L,6))	output_low(PIN_CONN_18); else output_high(PIN_CONN_18); //2^6 * FOSC/4
	}
	output_high(PIN_CONN_18);
	enable_interrupts(global);
}
