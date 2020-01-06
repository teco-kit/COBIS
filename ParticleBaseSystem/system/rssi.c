/**
* Driver for RSSI.
*
* Provides low and high level functions for using RSSI.
* Please include after awarecon#xxx.c and ack.c are included.
* See RSSI-docu.
*
* Author: sabin (wendhack@teco.edu)
* Date: 2005-02-07 (yyyy-mm-dd)
*
* version 000
*
* version 001: 	also suitable for pc230 (switch among resistor, capacitor and vdd at agc-control)
* 				2005-02-22 by sabin
* version 002: 	suitable for pc230/pc232 (switch among resistor, capacitor and vdd at agc-control)
*				added functions for the user, to configure rssi and use results directly in an application.
* 				2005-09-29 by sabin
* version 003: 	suitable for pc230/pc232 (switch among resistor, capacitor and vdd at agc-control)
*				bug fixed: initialize rssi pins in RSSIInit() to capacitor
* 				2005-12-19 by sabin
*
*/

#ifndef	__RSSI_C__
#define __RSSI_C__	003


// globals
boolean rssi_on = FALSE;
boolean rssi_request;
boolean	rssi_prepared;
boolean rssi_new_value;
unsigned int rssi_receive_config;
unsigned int rssi_receive_results;

unsigned long rssi_last_value;
unsigned long rssi_average_zero;
unsigned long rssi_average_one;
unsigned long rssi_buffer_zero[ RF_RSSI_SAMPLES ];
unsigned long rssi_buffer_one[ RF_RSSI_SAMPLES ];
unsigned int rssi_last_value_id[ 8 ];
unsigned int rssi_adc_save_hi;
unsigned int rssi_adc_save_lo;
unsigned int rssi_adc_save_channel;


// functions
// for the user.
void RSSIInit();
void RSSIRun();
void RSSIConfigureProcess( unsigned int send, unsigned int receive, unsigned int continuous, unsigned int led, unsigned int pin, unsigned int id );
void RSSIConfigureResults( unsigned int difference, unsigned int average_zero, unsigned int average_one, unsigned int samples_zero, unsigned int samples_one );
boolean RSSINewValue();
unsigned long* GetRSSISamplesZero();
unsigned long* GetRSSISamplesOne();
unsigned long GetRSSIAverageZero();
unsigned long GetRSSIAverageOne();
unsigned long GetRSSIDifference();

// internal.
void RSSIInitReceiver();
void RSSIConfigure( unsigned int* data );
void RSSICheckInRequest( unsigned int* data );
void RSSIAnswerRequest();
void RSSIPrepare( unsigned int mode );
void RSSIMeasure();
void RSSIStoreADC();
void RSSIRestoreADC();
unsigned long RSSIGetSamplesZero();
unsigned long RSSIGetSamplesOne();
unsigned long RSSIGetValue();
void ACLAddDataRSSISamplesZero( unsigned int samples );
void ACLAddDataRSSISamplesOne( unsigned int samples );
void ACLAddDataRSSIAverageZero();
void ACLAddDataRSSIAverageOne();
void ACLAddDataRSSIDifference();
void ACLSendPacketWithRSSI();


// macros
#define	RSSI_CONFIG_CONTINUOUSLY	0
#define	RSSI_CONFIG_ID				1
#define	RSSI_CONFIG_LED				2
#define	RSSI_CONFIG_PIN				3
#define	RSSI_RESULT_DIFFERENCE			0
#define	RSSI_RESULT_AV_ZERO				1
#define	RSSI_RESULT_AV_ONE				2
#define	RSSI_RESULT_SAMPLES_ZERO		3
#define	RSSI_RESULT_SAMPLES_ONE			4

#define RSSI_AGC_RESISTOR	1
#define RSSI_AGC_CAPACITOR	2
#define RSSI_AGC_VDD		3


/****************************************************************************************************************************
	functions for the user.
/****************************************************************************************************************************/
/**
* Inits rssi.
* Execute this in your main() before using rssi.
*/
void RSSIInit()
{
	unsigned int i;

		ACLSubscribe( ACL_TYPE_APS_HI, ACL_TYPE_APS_LO );
		ACLSubscribe( ACL_TYPE_CIC_HI, ACL_TYPE_CIC_LO );
		ACLSubscribe( ACL_TYPE_CIR_HI, ACL_TYPE_CIR_LO );

		rssi_send_on = FALSE;
		rssi_receive_on = FALSE;

		rssi_request = FALSE;
		rssi_prepared = FALSE;
		rssi_new_value = FALSE;
		for( i = 0; i < 8; ++i ) rssi_last_value_id[ i ] = 0;
		rssi_receive_results = 0;
		rssi_on = TRUE;

		bit_set( TRIS_RSSI_RES );
		bit_clear( TRIS_RSSI_CAP );
		bit_set( TRIS_RSSI_VDD );
		output_low( PIN_RSSI_CAP );
}

/**
* Updates rssi configuration.
* Stores requests for rssi results.
* Configures sending/ measurement of rssi.
* Put this in SlotEndCallBack() of your application.
*/
void RSSIRun()
{
	unsigned int i;

		if( rssi_on )
		{
			if( ACLAckedDataIsNewNow())
			{
				if( ACLFoundReceivedType( ACL_TYPE_APS_HI, ACL_TYPE_APS_LO ))
				{
					if( ACLFoundReceivedType( ACL_TYPE_CIC_HI, ACL_TYPE_CIC_LO ))
					{
						RSSIConfigure( ACLGetReceivedData( ACL_TYPE_CIC_HI, ACL_TYPE_CIC_LO ));
						if( bit_test( rssi_receive_config, RSSI_CONFIG_LED )) PCLedBlueOn();
						if( bit_test( rssi_receive_config, RSSI_CONFIG_PIN )) bit_clear( TRIS_RSSI_IN_PROCESS );
					}
					if( ACLFoundReceivedType( ACL_TYPE_CIR_HI, ACL_TYPE_CIR_LO ))
					{
						RSSICheckInRequest( ACLGetReceivedData( ACL_TYPE_CIR_HI, ACL_TYPE_CIR_LO ));
						if( bit_test( rssi_receive_config, RSSI_CONFIG_LED )) PCLedBlueOn();
						rssi_request = TRUE;
					}
				}
			}
			if( rssi_request || ( rssi_receive_on && bit_test( rssi_receive_config, RSSI_CONFIG_CONTINUOUSLY ) && rssi_new_value )) RSSIAnswerRequest();
		}
		if( rssi_send_on == 2 ) ACLSendPacketWithRSSI();
}

/**
* Configures rssi.
* Determines your rssi process, apply before using rssi.
*/
void RSSIConfigureProcess( unsigned int send, unsigned int receive, unsigned int continuous, unsigned int led, unsigned int pin, unsigned int id )
{
	rssi_send_on = send;
	rssi_receive_on = receive;
	if( rssi_receive_on ) RSSIInitReceiver();
	if( continuous ) bit_set( rssi_receive_config, RSSI_CONFIG_CONTINUOUSLY );
	else bit_clear( rssi_receive_config, RSSI_CONFIG_CONTINUOUSLY );
	if( led ) bit_set( rssi_receive_config, RSSI_CONFIG_ID );
	else bit_clear( rssi_receive_config, RSSI_CONFIG_ID );
	if( pin ) bit_set( rssi_receive_config, RSSI_CONFIG_LED );
	else bit_clear( rssi_receive_config, RSSI_CONFIG_LED );
	if( id ) bit_set( rssi_receive_config, RSSI_CONFIG_PIN );
	else bit_clear( rssi_receive_config, RSSI_CONFIG_PIN );
}

/**
* Configures rssi results.
* Determines the results of your rssi process, given via rf (ACL_TYPE_CIA).
*/
void RSSIConfigureResults( unsigned int difference, unsigned int average_zero, unsigned int average_one, unsigned int samples_zero, unsigned int samples_one )
{
	if( difference ) bit_set( rssi_receive_results, RSSI_RESULT_DIFFERENCE );
	else bit_clear( rssi_receive_results, RSSI_RESULT_DIFFERENCE );
	if( average_zero ) bit_set( rssi_receive_results, RSSI_RESULT_AV_ZERO );
	else bit_clear( rssi_receive_results, RSSI_RESULT_AV_ZERO );
	if( average_one ) bit_set( rssi_receive_results, RSSI_RESULT_AV_ONE );
	else bit_clear( rssi_receive_results, RSSI_RESULT_AV_ONE );
	if( samples_zero ) bit_set( rssi_receive_results, RSSI_RESULT_SAMPLES_ZERO );
	else bit_clear( rssi_receive_results, RSSI_RESULT_SAMPLES_ZERO );
	if( samples_one ) bit_set( rssi_receive_results, RSSI_RESULT_SAMPLES_ONE );
	else bit_clear( rssi_receive_results, RSSI_RESULT_SAMPLES_ONE );
}

/**
* Checks if there was a rssi-measurement.
* Is set 'false' as soon as any result of the measurement is read.
*/
boolean RSSINewValue()
{
	return rssi_new_value;
}

/**
* Returns a pointer on zero-samples of last measurement.
*/
unsigned long* GetRSSISamplesZero()
{
		rssi_new_value = FALSE;
		return rssi_buffer_zero;
}

/**
* Returns a pointer on one-samples of last measurement.
*/
unsigned long* GetRSSISamplesOne()
{
		rssi_new_value = FALSE;
		return rssi_buffer_one;
}

/**
* Returns average zero of last measurement.
*/
unsigned long GetRSSIAverageZero()
{
	rssi_new_value = FALSE;
	return rssi_average_zero;
}

/**
* Returns average one of last measurement.
*/
unsigned long GetRSSIAverageOne()
{
	rssi_new_value = FALSE;
	return rssi_average_one;
}

/**
* Returns difference of average one and average zero of last measurement.
*/
unsigned long GetRSSIDifference()
{
	rssi_new_value = FALSE;
	return rssi_last_value;
}


/****************************************************************************************************************************
	internal functions.
/****************************************************************************************************************************/
#ifdef __PC230_C__
void RSSIInitReceiver()
{
	ACLSubscribeAll();
	if( rssi_receive_config, 3 ) bit_clear( TRIS_RSSI_IN_PROCESS );
	bit_clear( TRIS_RSSI_POWER );
	bit_set( TRIS_RSSI_IN );
	bit_set( TRIS_RSSI_RES );
	bit_set( TRIS_RSSI_CAP );
	bit_set( TRIS_RSSI_VDD );
}
#else #ifdef __PC232_C__
void RSSIInitReceiver()
{
	ACLSubscribeAll();
	if( rssi_receive_config, 3 ) bit_clear( TRIS_RSSI_IN_PROCESS );
	bit_clear( TRIS_RSSI_POWER );
	bit_set( TRIS_RSSI_IN );
	bit_set( TRIS_RSSI_RES );
	bit_set( TRIS_RSSI_CAP );
	bit_set( TRIS_RSSI_VDD );
}
#else
void RSSIInitReceiver()
{
	ACLSubscribeAll();
	if( rssi_receive_config, 3 ) bit_clear( TRIS_RSSI_IN_PROCESS );
	bit_clear( TRIS_RSSI_POWER );
	bit_set( TRIS_RSSI_IN );
}
#endif

void RSSIConfigure( unsigned int* data )
{
	rssi_send_on = *data;
	rssi_receive_on = *( ++data );
	if( rssi_receive_on ) RSSIInitReceiver();
	if( *( ++data )) bit_set( rssi_receive_config, RSSI_CONFIG_CONTINUOUSLY );
	else bit_clear( rssi_receive_config, RSSI_CONFIG_CONTINUOUSLY );
	if( *( ++data )) bit_set( rssi_receive_config, RSSI_CONFIG_ID );
	else bit_clear( rssi_receive_config, RSSI_CONFIG_ID );
	if( *( ++data )) bit_set( rssi_receive_config, RSSI_CONFIG_LED );
	else bit_clear( rssi_receive_config, RSSI_CONFIG_LED );
	if( *( ++data )) bit_set( rssi_receive_config, RSSI_CONFIG_PIN );
	else bit_clear( rssi_receive_config, RSSI_CONFIG_PIN );
}

void RSSICheckInRequest( unsigned int* data )
{
	if( *data ) bit_set( rssi_receive_results, RSSI_RESULT_DIFFERENCE );
	else bit_clear( rssi_receive_results, RSSI_RESULT_DIFFERENCE );
	if( *( ++data )) bit_set( rssi_receive_results, RSSI_RESULT_AV_ZERO );
	else bit_clear( rssi_receive_results, RSSI_RESULT_AV_ZERO );
	if( *( ++data )) bit_set( rssi_receive_results, RSSI_RESULT_AV_ONE );
	else bit_clear( rssi_receive_results, RSSI_RESULT_AV_ONE );
	if( *( ++data )) bit_set( rssi_receive_results, RSSI_RESULT_SAMPLES_ZERO );
	else bit_clear( rssi_receive_results, RSSI_RESULT_SAMPLES_ZERO );
	if( *( ++data )) bit_set( rssi_receive_results, RSSI_RESULT_SAMPLES_ONE );
	else bit_clear( rssi_receive_results, RSSI_RESULT_SAMPLES_ONE );
}

/**
* Answers request for rssi results.
*/
void RSSIAnswerRequest()
{
	unsigned int 	dummy;
	unsigned int 	i;

		dummy = 0;

		if( !ACLSendingBusy())
		{
			ACLAddNewType( ACL_TYPE_APS_HI, ACL_TYPE_APS_LO );
			ACLAddNewType( ACL_TYPE_CIA_HI, ACL_TYPE_CIA_LO );

			if( rssi_new_value ) ACLAddData( 1 );
			else ACLAddData( 0 );

			ACLAddData( rssi_receive_on );

			if( bit_test( rssi_receive_results, RSSI_RESULT_DIFFERENCE )) ACLAddDataRSSIDifference();
			if( bit_test( rssi_receive_results, RSSI_RESULT_AV_ZERO )) ACLAddDataRSSIAverageZero();
			if( bit_test( rssi_receive_results, RSSI_RESULT_AV_ONE )) ACLAddDataRSSIAverageOne();
			if( bit_test( rssi_receive_results, RSSI_RESULT_SAMPLES_ZERO )) bit_set( dummy, 0 );
			if( bit_test( rssi_receive_results, RSSI_RESULT_SAMPLES_ONE )) bit_set( dummy, 1 );

			switch( dummy )
			{
				case 1:
					ACLAddDataRSSISamplesZero( 20 );
					break;
				case 2:
					ACLAddDataRSSISamplesOne( 20 );
					break;
				case 3:
					ACLAddDataRSSISamplesZero( 10 );
					ACLAddDataRSSISamplesOne( 10 );
					break;
			}
			if( bit_test( rssi_receive_config, RSSI_CONFIG_ID ) && ( ACLGetRemainingPayloadSpace() > 7 ))
			{
				for( i = 0; i < 8; ++i ) ACLAddData( rssi_last_value_id[ i ]);
			}

			ACLSendPacket( 20 );
			if( ACLGetSendSuccess()) rssi_request = FALSE;
		}
}

#ifdef __PC230_C__
void RSSIPrepare( unsigned int mode )
{
		if( !ADCInProgress())
		{
			output_high( PIN_RSSI_POWER );

			RSSIStoreADC();
			ADCSetCh( ADCH_RSSI );

			switch( mode )
			{
				case RSSI_AGC_RESISTOR:
					bit_clear( TRIS_RSSI_RES );
					bit_set( TRIS_RSSI_CAP );
					bit_set( TRIS_RSSI_VDD );
					output_low( PIN_RSSI_RES );
					break;
				case RSSI_AGC_CAPACITOR:
					bit_set( TRIS_RSSI_RES );
					bit_clear( TRIS_RSSI_CAP );
					bit_set( TRIS_RSSI_VDD );
					output_low( PIN_RSSI_CAP );
					break;
				case RSSI_AGC_VDD:
					bit_set( TRIS_RSSI_RES );
					bit_set( TRIS_RSSI_CAP );
					bit_clear( TRIS_RSSI_VDD );
					output_high( PIN_RSSI_VDD );
					break;
			}

			ADCSetClock( 0 );
			ADCEnable();

			rssi_prepared = TRUE;
		}
		else rssi_prepared = FALSE;
}
#else #ifdef __PC232_C__
void RSSIPrepare( unsigned int mode )
{
		if( !ADCInProgress())
		{
			output_high( PIN_RSSI_POWER );

			RSSIStoreADC();
			ADCSetCh( ADCH_RSSI );

			switch( mode )
			{
				case RSSI_AGC_RESISTOR:
					bit_clear( TRIS_RSSI_RES );
					bit_set( TRIS_RSSI_CAP );
					bit_set( TRIS_RSSI_VDD );
					output_low( PIN_RSSI_RES );
					break;
				case RSSI_AGC_CAPACITOR:
					bit_set( TRIS_RSSI_RES );
					bit_clear( TRIS_RSSI_CAP );
					bit_set( TRIS_RSSI_VDD );
					output_low( PIN_RSSI_CAP );
					break;
				case RSSI_AGC_VDD:
					bit_set( TRIS_RSSI_RES );
					bit_set( TRIS_RSSI_CAP );
					bit_clear( TRIS_RSSI_VDD );
					output_high( PIN_RSSI_VDD );
					break;
			}

			ADCSetClock( 0 );
			ADCEnable();

			rssi_prepared = TRUE;
		}
		else rssi_prepared = FALSE;
}
#else
void RSSIPrepare( unsigned int mode )
{
	//mode isn't used here.
		if( !ADCInProgress())
		{
			output_high( PIN_RSSI_POWER );

			RSSIStoreADC();

			ADCSetCh( ADCH_RSSI );
			ADCSetClock( 0 );
			ADCEnable();

			rssi_prepared = TRUE;
		}
		else rssi_prepared = FALSE;
}
#endif

void RSSIMeasure()
{
	unsigned long rssi_zero;
	unsigned long rssi_value;
	unsigned int i;

		if( !ADCInProgress())
		{
			if( rssi_prepared )
			{
				rssi_zero = RSSIGetSamplesZero();
				DelayUs( 108 );

				rssi_value = RSSIGetSamplesOne();
				rssi_average_zero = rssi_zero / RF_RSSI_SAMPLES;
				rssi_average_one = rssi_value / RF_RSSI_SAMPLES;

				rssi_last_value = rssi_average_one - rssi_average_zero;
				for( i = 0; i < 8; ++i ) rssi_last_value_id[ i ] = LL_header_received[ 3 + i ];
				rssi_new_value = TRUE;
			}
		}
		else rssi_last_value = 0;
		RSSIRestoreADC();
}

void RSSIStoreADC()
{
	rssi_adc_save_hi = PIC_ADRESH;
	rssi_adc_save_lo = PIC_ADRESL;

	rssi_adc_save_channel = 0b00111100 & PIC_ADCON0;
}

void RSSIRestoreADC()
{
	PIC_ADRESH = rssi_adc_save_hi;
	PIC_ADRESL = rssi_adc_save_lo;

	PIC_ADCON0 &= 0b11000011;
	PIC_ADCON0 |= rssi_adc_save_channel;
}

unsigned long RSSIGetSamplesZero()
{
	unsigned int i;
	unsigned long samples;

		if( bit_test( rssi_receive_config, 2 )) PCLedBlueOn();
		if( bit_test( rssi_receive_config, 3 )) output_high( PIN_RSSI_IN_PROCESS );

		samples = 0;

		for( i = 0; i < RF_RSSI_SAMPLES; i++ )
		{
			if( bit_test( rssi_receive_config, 3 ))
			{
				output_low( PIN_RSSI_IN_PROCESS );
				output_high( PIN_RSSI_IN_PROCESS );
			}
			rssi_buffer_zero[ i ] = ADCRead10();
		 	samples += rssi_buffer_zero[ i ];
		}

		if( bit_test( rssi_receive_config, 3 )) output_low( PIN_RSSI_IN_PROCESS );

		return samples;
}

unsigned long RSSIGetSamplesOne()
{
	unsigned int i;
	unsigned long samples;


		if( bit_test( rssi_receive_config, 3 )) output_high( PIN_RSSI_IN_PROCESS );

		samples = 0;

		for( i = 0; i < RF_RSSI_SAMPLES; i++ )
		{
			if( bit_test( rssi_receive_config, 3 ))
			{
				output_low( PIN_RSSI_IN_PROCESS );
				output_high( PIN_RSSI_IN_PROCESS );
			}
			rssi_buffer_one[ i ] = ADCRead10();
		 	samples += rssi_buffer_one[ i ];
		}

		if( bit_test( rssi_receive_config, 3 )) output_low( PIN_RSSI_IN_PROCESS );
		if( bit_test( rssi_receive_config, 2 )) PCLedBlueOff();

		return samples;
}

unsigned long RSSIGetValue()
{
		rssi_new_value = FALSE;
		return rssi_last_value;
}

void ACLAddDataRSSISamplesZero( unsigned int samples )
{
	unsigned int i;

		samples <<= 1;

		for( i = 0; i < samples; ++i )
		{
			++i;
			ACLAddData( hi( rssi_buffer_zero[ i ]));
			ACLAddData( lo( rssi_buffer_zero[ i ]));
		}
		rssi_new_value = FALSE;
}

void ACLAddDataRSSISamplesOne( unsigned int samples )
{
	unsigned int i;

	samples <<= 1;

	for( i = 0; i < samples; ++i )
	{
		++i;
		ACLAddData( hi( rssi_buffer_one[ i ] ));
		ACLAddData( lo( rssi_buffer_one[ i ] ));
	}
	rssi_new_value = FALSE;
}

void ACLAddDataRSSIAverageZero()
{
	ACLAddData( hi( rssi_average_zero ));
	ACLAddData( lo( rssi_average_zero ));
	rssi_new_value = FALSE;
}

void ACLAddDataRSSIAverageOne()
{
	ACLAddData( hi( rssi_average_one ));
	ACLAddData( lo( rssi_average_one ));
	rssi_new_value = FALSE;
}

void ACLAddDataRSSIDifference()
{
	unsigned long rssi_dummy;

		rssi_dummy = RSSIGetValue();
		ACLAddData( hi( rssi_dummy ));
		ACLAddData( lo( rssi_dummy ));
		rssi_new_value = FALSE;
}

void ACLSendPacketWithRSSI()
{
	if( !ACLSendingBusy())
	{
		ACLAddNewType( ACL_TYPE_SIS_HI, ACL_TYPE_SIS_LO );
		ACLAddData( 99 );
		ACLSendPacket( 20 );
	}
}