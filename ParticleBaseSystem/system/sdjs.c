/**
* Driver for SDJS.
*
* Provides low and high level functions for using SDJS.
* Please include after awarecon#xxx.c and ack.c are included.
*
* Author: sabin (wendhack@teco.edu)
* Date: 2004-10-27 (yyyy-mm-dd)
*
* ver: 000
* ver: 001
*/


// macros
#define SDJS_NO_VALUE 	255
#define SDJS_LEAVE_VALUE	111

#define SDJS_BUFFER_SIZE			64
#define SDJS_REQUEST_PARAMETERS 	4

#define SDJS_NO_SIGNS		0
#define SDJS_ONLY_LED		1
#define SDJS_ONLY_PIN		2
#define SDJS_LED_AND_PIN	3

#define SDJS_RF_TIME		70

// globals
unsigned int sdjs_evoke;
unsigned int sdjs_led;
unsigned int sdjs_symbol;
unsigned int sdjs_fieldstrength;
unsigned int sdjs_probability_1;
unsigned int sdjs_value_1_min;
unsigned int sdjs_value_1_max;
unsigned int sdjs_probability_2;
unsigned int sdjs_value_2;
unsigned int sdjs_probability_3;
unsigned int sdjs_value_3;
unsigned int sdjs_probability_4;
unsigned int sdjs_value_4;
unsigned int sdjs_value_5;
unsigned int sdjs_value_6;
unsigned int sdjs_value_7;
boolean sdjs_send_1;
boolean sdjs_send_2;
boolean sdjs_send_3;
boolean sdjs_send_4;
boolean sdjs_send_5;
boolean sdjs_send_6;
boolean sdjs_send_7;
unsigned int sdjs_random;
unsigned int sdjs_counter;
unsigned int sdjs_buffer[ SDJS_BUFFER_SIZE ];
boolean sdjs_on = FALSE;
unsigned int sdjs_request[ SDJS_REQUEST_PARAMETERS ];
boolean sdjs_new_data;

// functions
void SDJSInit();
void SDJSRun();
void SDJSConfigure( unsigned int* config_data, unsigned int number_config_data );
void SDJSConfigureValues( unsigned int type, unsigned int evoke, unsigned int led, unsigned int symbol, unsigned int fieldstrength, unsigned int probability_1, unsigned int value_1_min, unsigned int value_1_max, unsigned int probability_2, unsigned int value_2, unsigned int probability_3, unsigned int value_3, unsigned int probability_4, unsigned int value_4, unsigned int value_5, unsigned int value_6, unsigned int value_7 );
void SDJSConfigureBigSlots( unsigned int first_value, unsigned int number_big_slots, unsigned int number_random_slots, unsigned int value );
boolean SDJSDataIsNew();
void SDJSSetDataToOld();
void SDJSAnswerRequest( unsigned int* parameters );
void SDJSSetSends();
void SDJSEvoke();
void SDJSRF();
unsigned int SDJSRFGetRandomRange( unsigned int min, unsigned int max );


/**
* Init SDJS.
* Put this in main() of your application.
*/
void SDJSInit()
{
		ACLSubscribe( ACL_TYPE_APS_HI, ACL_TYPE_APS_LO );
		ACLSubscribe( ACL_TYPE_CJC_HI, ACL_TYPE_CJC_LO );
		ACLSubscribe( ACL_TYPE_CJR_HI, ACL_TYPE_CJR_LO );

		sdjs_type = 0;
		sdjs_led = 0;
		sdjs_value_1_min = SDJS_NO_VALUE;
		sdjs_value_1_max = SDJS_NO_VALUE;
		sdjs_value_2 = SDJS_NO_VALUE;
		sdjs_value_3 = SDJS_NO_VALUE;
		sdjs_value_4 = SDJS_NO_VALUE;
		sdjs_value_5 = SDJS_NO_VALUE;
		sdjs_value_6 = SDJS_NO_VALUE;
		sdjs_value_7 = SDJS_NO_VALUE;
		sdjs_send_1 = FALSE;
		sdjs_send_2 = FALSE;
		sdjs_send_3 = FALSE;
		sdjs_send_4 = FALSE;
		sdjs_send_5 = FALSE;
		sdjs_send_6 = FALSE;
		sdjs_send_7 = FALSE;

		sdjs_evoke = 0;
		sdjs_counter = 0;
		sdjs_request[ 0 ] = SDJS_NO_VALUE;

		sdjs_on = TRUE;
		sdjs_new_data = FALSE;
}


/**
* Updates SDJS configuration.
* Stores requests for SDJS results.
* Checks sdjs_evoke.
* Answers requests for SDJS results.
* Put this in SlotEndCallBack() of your application.
*/
void SDJSRun()
{
	unsigned int i;

		if( sdjs_on )
		{
			if( ACLAckedDataIsNewNow())
			{
				if( ACLFoundReceivedType( ACL_TYPE_APS_HI, ACL_TYPE_APS_LO ))
				{
					if( ACLFoundReceivedType( ACL_TYPE_CJC_HI, ACL_TYPE_CJC_LO ))
					{
						SDJSConfigure( ACLGetReceivedData( ACL_TYPE_CJC_HI, ACL_TYPE_CJC_LO ), ACLGetReceivedDataLength( ACL_TYPE_CJC_HI, ACL_TYPE_CJC_LO ));
						if( bit_test( sdjs_led, 0 )) PCLedBlueOn();
						if( bit_test( sdjs_led, 1 )) bit_clear( TRIS_SDJS_IN_PROCESS );
					}
					if( ACLFoundReceivedType( ACL_TYPE_CJR_HI, ACL_TYPE_CJR_LO ))
					{
						for( i = 0; i < 4; ++i ) sdjs_request[ i ] = ACLGetReceivedByte( ACL_TYPE_CJR_HI, ACL_TYPE_CJR_LO, i );
					}
				}
			}
			if( sdjs_evoke )
			{
				if( ++sdjs_counter == sdjs_evoke )
				{
					SDJSEvoke();
					sdjs_counter = 0;
				}
			}
			if( sdjs_request[ 0 ] != SDJS_NO_VALUE ) SDJSAnswerRequest( sdjs_request );
		}
}

/**
* Configures SDJS.
* Wants a buffer containing configuration parameters and number of configuration parameters.
* Configuration parameters are:
* - type (0: sdjs switched off, 1: now implemented sdjs-version).
* - evoke (0: doesn't evoke sdjs, x>0: evokes sdjs every x slot).
* - led (0: no sdjs-led, 1: switches on led starting sdjs and receiving new configuration, 2: rise pin_sdjs_in_process when doing sdjs, 3: 1 and 2 (led and pin).
* - symbol (the symbol sent in a sdjs-slot).
* - fieldstrength
* (and optionally:)
* - probability_1 (probability between 0 and 100 the following value is sent with).
* - value_1_min (first value is chosen between value_1_min and value_1_max, if a fixed value is wanted, please put it as min and as max).
* - value_1_max
* - probability_2
* - value_2
* - probability_3
* - value_3
* - probability_4
* - value_4
* - value_5 (for the last three possible sdjs-values there's no probability available).
* - value_6
* - value_7
*/
void SDJSConfigure( unsigned int* config_data, unsigned int number_config_data )
{
	unsigned int dummy;

		if( *config_data != SDJS_LEAVE_VALUE ) sdjs_type = *config_data;
		if( *( ++config_data ) != SDJS_LEAVE_VALUE )
		{
			sdjs_evoke = *config_data;
			sdjs_counter = 0;
		}
		if( *( ++config_data ) != SDJS_LEAVE_VALUE ) sdjs_led = *config_data;
		if( sdjs_led > 3 ) sdjs_led = 0;
		if( *( ++config_data ) != SDJS_LEAVE_VALUE ) sdjs_symbol = *config_data;
		if( *( ++config_data ) != SDJS_LEAVE_VALUE )	sdjs_fieldstrength = *config_data;
		if( sdjs_fieldstrength > 32 ) sdjs_fieldstrength = 32;

		if( number_config_data > 6 )
		{
			if( *( ++config_data ) != SDJS_LEAVE_VALUE )	sdjs_probability_1 = *config_data;
			if( sdjs_probability_1 > 100 ) sdjs_probability_1 = 100;
			if( *( ++config_data ) != SDJS_LEAVE_VALUE )	sdjs_value_1_min = *config_data;
			if(( sdjs_value_1_min + 1 ) && ( sdjs_value_1_min > 63 )) sdjs_value_1_min = 63;
			if( *( ++config_data ) != SDJS_LEAVE_VALUE ) sdjs_value_1_max = *config_data;
			if(( sdjs_value_1_max + 1 ) && ( sdjs_value_1_max > 63 )) sdjs_value_1_max = 63;
			if( sdjs_value_1_max < sdjs_value_1_min )
			{
				dummy = sdjs_value_1_max;
				sdjs_value_1_max = sdjs_value_1_min;
				sdjs_value_1_min = dummy;
			}

			if( number_config_data > 9 )
			{
				if( *( ++config_data ) != SDJS_LEAVE_VALUE ) sdjs_probability_2 = *config_data;
				if( sdjs_probability_2 > 100 ) sdjs_probability_2 = 100;
				if( *( ++config_data ) != SDJS_LEAVE_VALUE ) sdjs_value_2 = *config_data;
				if(( sdjs_value_2 + 1 ) && ( sdjs_value_2 > 63 )) sdjs_value_2 = 63;

				if( number_config_data > 11 )
				{
					if( *( ++config_data ) != SDJS_LEAVE_VALUE ) sdjs_probability_3 = *config_data;
					if( sdjs_probability_3 > 100 ) sdjs_probability_3 = 100;
					if( *( ++config_data ) != SDJS_LEAVE_VALUE ) sdjs_value_3 = *config_data;
					if(( sdjs_value_3 + 1 ) && ( sdjs_value_3 > 63 )) sdjs_value_3 = 63;

					if( number_config_data > 13 )
					{
						if( *( ++config_data ) != SDJS_LEAVE_VALUE ) sdjs_probability_4 = *config_data;
						if( sdjs_probability_4 > 100 ) sdjs_probability_4 = 100;
						if( *( ++config_data ) != SDJS_LEAVE_VALUE ) sdjs_value_4 = *config_data;
						if(( sdjs_value_4 + 1 ) && ( sdjs_value_4 > 63 )) sdjs_value_4 = 63;

						if( number_config_data > 14 )
						{
							if( *( ++config_data ) != SDJS_LEAVE_VALUE ) sdjs_value_5 = *config_data;
							if(( sdjs_value_5 + 1 ) && ( sdjs_value_5 > 63 )) sdjs_value_5 = 63;

							if( number_config_data > 15 )
							{
								if( *( ++config_data ) != SDJS_LEAVE_VALUE ) sdjs_value_6 = *config_data;
								if(( sdjs_value_6 + 1 ) && ( sdjs_value_6 > 63 )) sdjs_value_6 = 63;

								if( number_config_data > 16 )
								{
									if( *( ++config_data ) != SDJS_LEAVE_VALUE ) sdjs_value_7 = *config_data;
									if(( sdjs_value_7 + 1 ) && ( sdjs_value_7 > 63 )) sdjs_value_7 = 63;
								}
							}
						}
					}
				}
			}
		}

		if( number_config_data < 16 ) 	sdjs_value_7 = SDJS_NO_VALUE;
		if( number_config_data < 15 ) 	sdjs_value_6 = SDJS_NO_VALUE;
		if( number_config_data < 14 ) 	sdjs_value_5 = SDJS_NO_VALUE;
		if( number_config_data < 13 ) 	sdjs_value_4 = SDJS_NO_VALUE;
		if( number_config_data < 11 ) 	sdjs_value_3 = SDJS_NO_VALUE;
		if( number_config_data < 9 ) 	sdjs_value_2 = SDJS_NO_VALUE;
		if( number_config_data < 6 )
		{
			sdjs_value_1_min = SDJS_NO_VALUE;
			sdjs_value_1_max = SDJS_NO_VALUE;
		}

		SDJSSetSends();
		ACLSetFieldStrength( sdjs_fieldstrength );
}


/**
* Configures SDJS.
* Wants configuration parameters.
* Configuration parameters are:
* - type (0: sdjs switched off, 1: now implemented sdjs-version).
* - evoke (0: doesn't evoke sdjs, x>0: evokes sdjs every x slot).
* - led (0: no sdjs-led, x!=0: switches on led starting sdjs and receiving new configuration).
* - symbol (the symbol sent in a sdjs-slot).
* - fieldstrength
* (and optionally:)
* - probability_1 (probability between 0 and 100 the following value is sent with).
* - value_1_min (first value is chosen between value_1_min and value_1_max, if a fixed value is wanted, please put it as min and as max).
* - value_1_max
* - probability_2
* - value_2
* - probability_3
* - value_3
* - probability_4
* - value_4
* - value_5 (for the last three possible sdjs-values there's no probability available).
* - value_6
* - value_7
*/
void SDJSConfigureValues( unsigned int type, unsigned int evoke, unsigned int led, unsigned int symbol, unsigned int fieldstrength, unsigned int probability_1, unsigned int value_1_min, unsigned int value_1_max, unsigned int probability_2, unsigned int value_2, unsigned int probability_3, unsigned int value_3, unsigned int probability_4, unsigned int value_4, unsigned int value_5, unsigned int value_6, unsigned int value_7 )
{

	unsigned int config_data[ 17 ];

		config_data[ 0 ] = 		type;
		config_data[ 1 ] = 		evoke;
		config_data[ 2 ] = 		led;
		config_data[ 3 ] = 		symbol;
		config_data[ 4 ] =		fieldstrength;
		config_data[ 5 ] = 		probability_1;
		config_data[ 6 ] = 		value_1_min;
		config_data[ 7 ] = 		value_1_max;
		config_data[ 8 ] = 		probability_2;
		config_data[ 9 ] = 		value_2;
		config_data[ 10 ] = 	probability_3;
		config_data[ 11 ] = 	value_3;
		config_data[ 12 ] = 	probability_4;
		config_data[ 13 ] = 	value_4;
		config_data[ 14 ] = 	value_5;
		config_data[ 15 ] = 	value_6;
		config_data[ 16 ] = 	value_7;

		SDJSConfigure( config_data, 17 );
}


/** Locates a certain value in the SDJS-slots available, which represent a certain scale.
* SDJS-slot 0 represents first_value.
* Number_big_slots values are represented.
* Every value has a certain number_random_slots among which is chosen by random.
* (Useful for example for temperature measurements: SDJS-slots may represent degrees from 23 to 30
* with six random slot possibilities for each degree: ACLSDJSConfigureBigSlots(23, 8, 6, measured value)).
*/
void SDJSConfigureBigSlots( unsigned int first_value, unsigned int number_big_slots, unsigned int number_random_slots, unsigned int value )
{
		if(( value >= first_value ) && ( value < ( first_value + number_big_slots )))
		{
			value -= first_value;
			value *= number_random_slots;
			SDJSConfigureValues( SDJS_LEAVE_VALUE, SDJS_LEAVE_VALUE, SDJS_LEAVE_VALUE, SDJS_LEAVE_VALUE, SDJS_LEAVE_VALUE, SDJS_LEAVE_VALUE, value, ( value + number_random_slots - 1 ), SDJS_LEAVE_VALUE, SDJS_LEAVE_VALUE, SDJS_LEAVE_VALUE, SDJS_LEAVE_VALUE, SDJS_LEAVE_VALUE, SDJS_LEAVE_VALUE, SDJS_LEAVE_VALUE, SDJS_LEAVE_VALUE, SDJS_LEAVE_VALUE );
		}
}


/**
* Checks if there was a SDJS slot.
*/
boolean SDJSDataIsNew()
{
	return sdjs_new_data;
}


/**
* Marks SDJS data as old.
*/
void SDJSSetDataToOld()
{
	sdjs_new_data = FALSE;
}


/**
* Answers request for SDJS results.
* Whereas the first parameter indicates the type of result wanted:
* 0: send own SDJS configuration.
* 1: send number of bytes found during last SDJS.
* 2: send the last SDJS buffer received. Attention: As the answer needs six bytes of types, only the 58 first of 64 bytes in buffer will be sent.
* 3: wants three more parameters and sends results analogously to SDJSConfigureBigSlots():
* 	1st: number of big slots representing values.
* 	2nd: number of random slots for each big slot (each value).
* 	3rd: number of bits that have to be set in order to count a byte.
* 	Sends the number of bytes counted for every big slot.
*/
void SDJSAnswerRequest()
{
	unsigned int* 	buffer;
	unsigned int 	bit_counter;
	unsigned int	byte_counter;
	unsigned int 	i;
	unsigned int 	j;
	unsigned int 	k;

		if( !ACLSendingBusy())
		{
			ACLAddNewType( ACL_TYPE_APS_HI, ACL_TYPE_APS_LO );
			ACLAddNewType( ACL_TYPE_CJA_HI, ACL_TYPE_CJA_LO );
			ACLAddData( sdjs_request[ 0 ]);

			switch( sdjs_request[ 0 ])
			{

				case 0:
						ACLAddData( sdjs_type );
						ACLAddData( sdjs_evoke );
						ACLAddData( sdjs_led );
						ACLAddData( sdjs_symbol );
						ACLAddData( sdjs_fieldstrength );
						ACLAddData( sdjs_probability_1 );
						ACLAddData( sdjs_value_1_min );
						ACLAddData( sdjs_value_1_max );
						ACLAddData( sdjs_probability_2 );
						ACLAddData( sdjs_value_2 );
						ACLAddData( sdjs_probability_3 );
						ACLAddData( sdjs_value_3 );
						ACLAddData( sdjs_probability_4 );
						ACLAddData( sdjs_value_4 );
						ACLAddData( sdjs_value_5 );
						ACLAddData( sdjs_value_6 );
						ACLAddData( sdjs_value_7 );
						break;

				case 1:
						byte_counter = 0;
						buffer = sdjs_buffer;
						for( i = 0; i < SDJS_BUFFER_SIZE; ++i )
						{
							if( *buffer ) ++byte_counter;
							++buffer;
						}
						ACLAddData( byte_counter );		// put in orogin adress from HELO
						break;

				case 2:
						buffer = sdjs_buffer;
						while( ACLGetRemainingPayloadSpace())
						{
							ACLAddData( *buffer );
							++buffer;
						}
						break;

				case 3:
						bit_counter = 0;
						byte_counter = 0;
						buffer = sdjs_buffer;

						for( i = 0; i < sdjs_request[ 1 ]; ++i )
						{
							for( j = 0; j < sdjs_request[ 2 ]; ++j )
							{
								for( k = 0; k < 8; ++k )
								{
									if( bit_test( *buffer, k )) ++bit_counter;
								}

								if( bit_counter >= sdjs_request[ 3 ]) ++byte_counter;
								bit_counter = 0;
								++buffer;
							}
							ACLAddData( byte_counter );
							byte_counter = 0;
						}
						break;
			}
			ACLSendPacket( 20 );
			if( ACLGetSendSuccess()) sdjs_request[ 0 ] = SDJS_NO_VALUE;
		}
}


/**
* Checks which slots to be taken in the following SDJS slot.
* Checks probabilities.
*/
void SDJSSetSends()
{

		if(( sdjs_value_1_max == SDJS_NO_VALUE ) || ( sdjs_value_1_min == SDJS_NO_VALUE )) sdjs_send_1 = FALSE;
		else
		{
			if(( sdjs_probability_1 == 100 ) || ( sdjs_probability_1 > SDJSRFGetRandomRange( 0, 99 ))) sdjs_send_1 = TRUE;
			else sdjs_send_1 = FALSE;
		}
		if( sdjs_send_1 ) sdjs_random = SDJSRFGetRandomRange( sdjs_value_1_min, sdjs_value_1_max );

		if( sdjs_value_2 == SDJS_NO_VALUE ) sdjs_send_2 = FALSE;
		else
		{
			if(( sdjs_probability_2 == 100 ) || ( sdjs_probability_2 > SDJSRFGetRandomRange( 0, 99 ))) sdjs_send_2 = TRUE;
			else sdjs_send_2 = FALSE;
		}

		if( sdjs_value_3 == SDJS_NO_VALUE ) sdjs_send_3 = FALSE;
		else
		{
			if(( sdjs_probability_3 == 100 ) || ( sdjs_probability_3 > SDJSRFGetRandomRange( 0, 99 ))) sdjs_send_3 = TRUE;
			else sdjs_send_3 = FALSE;
		}

		if( sdjs_value_4 == SDJS_NO_VALUE ) sdjs_send_4 = FALSE;
		else
		{
			if(( sdjs_probability_4 == 100 ) || ( sdjs_probability_4 > SDJSRFGetRandomRange( 0, 99 ))) sdjs_send_4 = TRUE;
			else sdjs_send_4 = FALSE;
		}

		if( sdjs_value_5 == SDJS_NO_VALUE ) sdjs_send_5 = FALSE;
		else sdjs_send_5 = TRUE;

		if( sdjs_value_6 == SDJS_NO_VALUE ) sdjs_send_6 = FALSE;
		else sdjs_send_6 = TRUE;

		if( sdjs_value_7 == SDJS_NO_VALUE ) sdjs_send_7 = FALSE;
		else sdjs_send_7 = TRUE;
}


/**
* Sends SDJS-starting type in next slot.
* Attention: Aborts stored ACL-packet.
*/
void SDJSEvoke()
{
	if( ACLSendingBusy()) ACLAbortSending();
	ACLAddNewType( ACL_TYPE_SJS_HI, ACL_TYPE_SJS_LO );
	ACLAddData( 99 );
	ACLSendPacket( 20 );
}


/**
* Function that runs SDJS on RF level.
*/
void SDJSRF()
{
	int *p_buff;

		//prepare
		memset(sdjs_buffer, 0, 64);					// clear buffer

		if( sdjs_type == 1 )
		{
			if( sdjs_led ) PCLedBlueOn();

			if( sdjs_send_1 ) sdjs_buffer[ sdjs_random ] = sdjs_symbol;			//SYMBOL_SDJS;		//set the jam somewhere
			if( sdjs_send_2 ) sdjs_buffer[ sdjs_value_2 ] = sdjs_symbol;
			if( sdjs_send_3 ) sdjs_buffer[ sdjs_value_3 ] = sdjs_symbol;
			if( sdjs_send_4 ) sdjs_buffer[ sdjs_value_4 ] = sdjs_symbol;
			if( sdjs_send_5 ) sdjs_buffer[ sdjs_value_5 ] = sdjs_symbol;
			if( sdjs_send_6 ) sdjs_buffer[ sdjs_value_6 ] = sdjs_symbol;
			if( sdjs_send_7 ) sdjs_buffer[ sdjs_value_7 ] = sdjs_symbol;

			p_buff = sdjs_buffer;

			RFSpiOn();
			RFUartOn();
			rf_tx=0;
			RFWaitForNextBitTimeExact();

			RFSpiSendByte();							//to set the output tx pin to zero
			RFSpiWait();

			for (rf_shift=0;rf_shift<64;rf_shift++)
			{
				RFWaitForTimer1Exact(SDJS_RF_TIME +rf_shift*2);

				if( bit_test( sdjs_led, 1)) output_low(PIN_SDJS_IN_PROCESS);
/*				#asm
				btfsc	sdjs_led, 1
				bsf		PIC_PORTD, 5
				#endasm
*/
				if((*p_buff)==0)
				{
					RFSetModeSensitive();
					RFUartStartContinuous();
					RFUartClearFifo();
					while (RF_TIMER1L<150) ;
					RFUartClearFifo();
					RFUartStartContinuous();
					RFReceiveByte();
					*p_buff=rf_rx1;
				}
				else
				{
					RFSetModeAsk();
					while (RF_TIMER1L<150) ;
					rf_tx=*p_buff;
					RFSpiSendByte();
				}
				p_buff++;
			}

			if( bit_test( sdjs_led, 1)) output_low(PIN_SDJS_IN_PROCESS);
			SDJSSetSends();
			sdjs_new_data = TRUE;
		}
}


unsigned int SDJSRFGetRandomRange( unsigned int min, unsigned int max )
{
	unsigned int up_to;
	unsigned int pattern;
	unsigned int counter;
	unsigned int random_h;
	unsigned int random_l;

	up_to = max - min;
	if( up_to == 0 ) return max;

	pattern = 0b11111111;

	for( counter = 7; counter >= 0; --counter )
	{
		if( !bit_test( up_to, counter )) bit_clear( pattern, counter );
		else break;
	}

	counter = 0;

	while( TRUE )
	{
		RFNewRandom();
		random_l = rf_random_l & pattern;
		random_h = rf_random_h & pattern;
		if( random_h <= up_to ) return ( min + random_h );
		if( random_l <= up_to ) return ( min + random_l );
		if(( counter == 20 ) && ( pattern > 1 )) pattern >>= 1;
		++counter;
	}

}





