/**
* Driver for real time clock in the new hardware layout (external timer3)
*
* Author: Christian Decker (cdecker@teco.edu)
* Date: 2003-06-10 (yyyy-mm-dd)
*
* Current state: template
*
* 007:	changed prototype RTCUpdate() to 'void'.
* 		2005-09-29 by sabin (wendhack@teco.edu)
*/

#ifndef __RTC_C__
#define	__RTC_C__					007

#define	REAL_TIME_CLOCK								//important for prs
#define RTC_SLOTS_TO_WAIT_FOR_TIMESERVER	100

void 			RTCInit();
void 			RTCSetClock( unsigned int hour, minute, second, unsigned long millisecond );
void 			RTCSetDate( unsigned int day, month, year );
void 			RTCGetClock( unsigned int* hour, unsigned int* minute, unsigned int* second, unsigned long* millisecond );
void 			RTCGetDate( unsigned int* day, unsigned int* month, unsigned int* year );
unsigned int 	RTCGetTimeFromServer();
void 			RTCAddTimeStampACL();
void 			RTCAddTimeStampSpecificACL( unsigned int chose_data );

void RTCUpdate();

unsigned long RTCGetMillisecond();
unsigned int RTCGetMonth();
unsigned int RTCGetYear();

void RTCSetMillisecond( unsigned long millisec );
void RTCSetMonth( unsigned int mon );
void RTCSetYear( unsigned int yea );
void RTCIncYear();

unsigned int RTC_second;
unsigned int RTC_minute;
unsigned int RTC_hour;
unsigned int RTC_day;
unsigned int RTC_month_year;
short	RTC_got_time = 0;

short	RTC_updated;

/**
* Initializes real time clock.
* @return returns 0 if no error
*/
void RTCInit()
{
	ClockTimerInit(CLOCK_32kHz);
	//bit_set( PIC_T1CON, 3 );
	//PIC_T3CON = 0b01110111;
	DelayUs( 50 );
	RTCSetClock( 0, 0, 0, 0 );
	RTCSetDate( 0, 0, 3 );
	ACLSubscribe( ACL_TYPE_CTA_HI, ACL_TYPE_CTA_LO );
	if( RTCGetTimeFromServer()) RTC_got_time = 1;
}

void RTCSetClock( unsigned int hour, minute, second, unsigned long millisecond )
{
	RTC_hour = hour;
	RTC_minute = minute;
	RTC_second = second;
	RTCSetMillisecond( millisecond );
}

void RTCSetDate( unsigned int day, month, year )
{
	RTC_day = day;
	RTCSetMonth( month );
	RTCSetYear( year );
}

void RTCGetClock( unsigned int* hour, unsigned int* minute, unsigned int* second, unsigned long* millisecond )
{
	*hour = RTC_hour;
	*minute = RTC_minute;
	*second = RTC_second;
	*millisecond = RTCGetMillisecond();
}

void RTCGetDate( unsigned int* day, unsigned int* month, unsigned int* year )
{
	*day = RTC_day;
	*month = RTC_month_year & 0b00001111;
	*year = ( RTC_month_year & 0b11110000 ) >> 4;
}

unsigned int RTCGetTimeFromServer()
{
	char* buff;
	unsigned long wait;
	unsigned int result = 0;

	wait = RTC_SLOTS_TO_WAIT_FOR_TIMESERVER;

	RTCSetClock( 0, 0, 0, 0 );
	RTCSetDate( 0, 0, 0 );

	while ( wait )
	{
		if( !ACLSendingBusy())
		{
			ACLClearSendData();
			ACLAddNewType( ACL_TYPE_CTR_HI, ACL_TYPE_CTR_LO );
			ACLSendPacket( 20 );
			wait--;
		}
		if ( ACLDataIsNew() )
		{
			if( ACLFoundReceivedType( ACL_TYPE_CTA_HI, ACL_TYPE_CTA_LO ))
			{
				buff = ACLGetReceivedData( ACL_TYPE_CTA_HI, ACL_TYPE_CTA_LO );
				RTCSetClock( buff[3], buff[4], buff[5], 0 );
				RTCSetDate( buff[2], buff[1], buff[0] );
				result = 1;
				break;	// got time from server;
			}
		}
	}
	return result;
}

void RTCAddTimeStampACL()
{
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	unsigned long millisecond;
	unsigned int day;
	unsigned int month;
	unsigned int year;

	RTCGetClock( &hour, &minute, &second, &millisecond );
	RTCGetDate( &day, &month, &year );

	if( ACLGetRemainingPayloadSpace() >= 11 )
	{
		ACLAddNewType( ACL_TYPE_CTS_HI, ACL_TYPE_CTS_LO );
		ACLAddData( year );
		ACLAddData( month );
		ACLAddData( day );
		ACLAddData( hour );
		ACLAddData( minute );
		ACLAddData( second );
		ACLAddData( hi( millisecond ));
		ACLAddData( lo( millisecond ));
	}
}

void RTCAddTimeStampSpecificACL( unsigned int chose_data )
{

// chose_data: bit 	0 set: 		year 		required in time stamp.
//					1		 	month
//					2			day
//					3			hour
//					4			minute
//					5			second
//					6			milisecond

	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	unsigned long millisecond;
	unsigned int day;
	unsigned int month;
	unsigned int year;
	unsigned long sum = 3;		// bytes for type & length.
	unsigned int i;

	RTCGetClock( &hour, &minute, &second, &millisecond );
	RTCGetDate( &day, &month, &year );

	for( i = 0; i < 7; ++i )
	{
		if( bit_test( chose_data, i )) ++sum;
	}

	if( bit_test( chose_data, 6 )) sum += 2;

	if( ACLGetRemainingPayloadSpace() >= sum )
	{
		ACLAddNewType( ACL_TYPE_CTS_HI, ACL_TYPE_CTS_LO );
		if( bit_test( chose_data, 0 )) ACLAddData( year );
		if( bit_test( chose_data, 1 )) ACLAddData( month );
		if( bit_test( chose_data, 2 )) ACLAddData( day );
		if( bit_test( chose_data, 3 )) ACLAddData( hour );
		if( bit_test( chose_data, 4 )) ACLAddData( minute );
		if( bit_test( chose_data, 5 )) ACLAddData( second );
		if( bit_test( chose_data, 6 ))
		{
			ACLAddData( hi( millisecond ));
			ACLAddData( lo( millisecond ));
		}
	}
}

/**
* Gets real time clock value
* @param value
* @return returns 0 if no error
*/
void RTCUpdate()
{
	unsigned int day;
	unsigned int month;
	unsigned int year;
	unsigned int* buff;

		RTC_updated = 0;
		while( ClockTimerGetHi() >= 16 )
		{
			RTC_updated = 1;
			ClockTimerSetHi(ClockTimerGetHi() - 16);
			++RTC_second;
			if( RTC_second == 60 )
			{
				RTC_second = 0;
				++RTC_minute;
				if( RTC_minute == 60 )
				{
					RTC_minute = 0;
					++RTC_hour;
					if( RTC_hour == 24 )
					{
						RTC_hour = 0;
						++RTC_day;
						if( RTC_day > 28 )
						{
							if( RTC_day > 31 )
							{
								RTC_day = 1;
								++RTC_month_year;
							}
							if(( RTC_day > 30 ) && ((( RTC_month_year & 0b00001111 ) == 4 ) || (( RTC_month_year & 0b00001111 ) == 6 ) || (( RTC_month_year & 0b00001111 ) == 9 ) || (( RTC_month_year & 0b00001111 ) == 11 )))
							{
								RTC_day = 1;
								++RTC_month_year;
							}
							if( RTC_month_year == 2 )
							{
								if( !( RTC_month_year & 0b00110000 ) && ( RTC_month_year & 0b11110000 ))
								{
									if( day > 29 )
									{
										RTC_day = 1;
										++RTC_month_year;
									}
								}
								else if( day > 28 )
								{
									RTC_day = 1;
									++RTC_month_year;
								}
							}
							if(( RTC_month_year & 0b00001111 ) > 12 )
							{
								RTC_month_year &= 0b11110000;
								RTC_month_year += 17;
							}
						}
					}
				}
			}
		}

		if( ACLDataIsNewNow())
		{
			if( ACLFoundReceivedType( ACL_TYPE_CTA_HI, ACL_TYPE_CTA_LO ))
			{
				buff = ACLGetReceivedData( ACL_TYPE_CTA_HI, ACL_TYPE_CTA_LO );
				RTCSetClock( buff[3], buff[4], buff[5], 0 );
				RTCSetDate( buff[2], buff[1], buff[0] );
				RTC_got_time = 1;
			}
		}
}

unsigned long RTCGetMillisecond()
{
	unsigned int image_TMR3L;
	unsigned long result;

		image_TMR3L = ClockTimerGetLo();
		result = ( long )( ClockTimerGetHi() ) * 625;
		result += (( long )( image_TMR3L ) * 24 / 10 );	// ungenau... 244, irgendwas mü
		return( result / 10 );
}

unsigned int RTCGetMonth()
{
	return( RTC_month_year & 0b00001111 );
}

unsigned int RTCGetYear()
{
	return(( RTC_month_year & 0b11110000 ) >> 4 );
}

void RTCSetMillisecond( unsigned long millisec )
{
	unsigned long image_TMR3H;
	unsigned long image_TMR3L;

		if( millisec > 999 ) return;
		image_TMR3H = millisec * 10 / 625;
		image_TMR3L = millisec - ( image_TMR3H * 625 / 10 );
		image_TMR3L = image_TMR3L * 100 / 25;				// ungenau... 244, irgendwas mü

		ClockTimerSetHi( ( int )image_TMR3H);
		ClockTimerSetLo( ( int )image_TMR3L);
}

void RTCSetMonth( unsigned int mon )
{
	if( mon > 12 ) return;
	RTC_month_year &= 0b11110000;
	RTC_month_year |= mon;
}

void RTCSetYear( unsigned int yea )
{
	if( yea > 15 ) return;
	RTC_month_year &= 0b00001111;
	RTC_month_year |= ( yea << 4 );
}

void RTCIncYear()
{
	RTC_month_year += 16;
}




