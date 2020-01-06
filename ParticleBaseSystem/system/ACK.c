/**
* Functions for acknowledged packets, to be used within ACL.
*
* Author: sabin (wendhack@teco.edu)
* Date: 2003-10-02 (yyyy-mm-dd)
*
* modified by Christian Decker (cdecker@teco.edu)
* Date: 2003-12-04
* - comment out test_pin implementation
*
* modified by sabin (wendhack@teco.edu)
* Date: 2004-03-03
* - inserted timeout to delete ACL_ACK_yet_received_list-seqencenr.
* - added third return value for sendsuccess-functions: 0 failed, 1 success, 2 still in queue.
* - created ACLACKCheckInPacket() as not to have two times the same code in ACLAckedDataIsNew() and ACLAckedDataIsNewNow().
* - fixed: ACLAckedDataIsNew() and ACLAckedDataIsNewNow() now return false when received DATA_ACK!!
* - explicitly switch on ACLACKRun() in ACLACKInit() by ACL_ACK_on.
* - fixed: inserted ACLSetDataToOld() when found ACLAckedDataIsNew()!
*
* Date:
*  2004_05_26 Albert:
*  fixed ACL_ACK_BUFFER_SIZE from 40 to 64(!)  (why wasn't it 25? or 33????? ... argh!!!!)
*
* Date:
*  2004_11_02 sabin
*  fixed: Check packet in yet received packets-list only one time a slot ( new: ACL_ACK_packet_checked_in_this_slot ).
*
* Date:
*  2005_09_29 sabin
*  fixed: changed prototype of ACLACKCheckInPacket() to 'unsigned int'.
*/

#define ACL_ACK_BUFFER_SIZE	64

#define ACL_ACK_TIMEOUT 50
#define ACL_ACK_TIMEOUT_TOLERANCE 20

#define ACL_ACK_RECEIVE_LIST_TIMEOUT ( ACL_ACK_TIMEOUT * ACL_ACK_TIMEOUT_TOLERANCE )

#define	ACL_ACK_HEADER 2
#define ACL_ADD_HEADER 8

#define ACL_ACK_YET_RECEIVED_LIST_SIZE	3

#define DATA_PACK 	0
#define DATA_ACK	1


/* Global variables. */

unsigned int 	ACL_ACK_timer;
unsigned int	ACL_ACK_timer_tolerance;
unsigned long 	ACL_ACK_yet_received_list_timeout;
byte 			ACL_ACK_secure_buffer[ ACL_ACK_BUFFER_SIZE ];
unsigned int 	ACL_ACK_secure_packet_length;
unsigned int 	ACL_ACK_own_seq_number = 0;
unsigned int 	ACL_ACK_status = 0;
short int		ACL_ACK_on = 0;
short int 		ACL_ACK_packet_checked_in_this_slot = 0;

#define ACL_ACK_SEND_QUEUED 		ACL_ACK_status, 0
#define ACL_ACK_SEND_SUCCESS 		ACL_ACK_status, 1
#define ACL_ACK_SEND_SUCCESS_NOW	ACL_ACK_status, 2
#define ACL_ACK_TIMEOUT_QUEUED 		ACL_ACK_status, 3
#define ACL_ACK_ERROR 				ACL_ACK_status, 4

struct {
unsigned int add1;
unsigned int add2;
unsigned int add3;
unsigned int add4;
unsigned int add5;
unsigned int add6;
unsigned int add7;
unsigned int add8;
unsigned int seq_nr;
} ACL_ACK_yet_received_list[ ACL_ACK_YET_RECEIVED_LIST_SIZE ];

unsigned int ACL_ACK_yet_received_list_position;
unsigned int dummy;

void	ACLACKInit();
int 	ACLAckedDataIsNew();
int 	ACLAckedDataIsNewNow();
int		ACLSendingBusyAcked();
int 	ACLSendPacketAcked( unsigned int add1, add2, add3, add4, add5, add6, add7, add8, timeout );
int 	ACLGetSendSuccessAcked();
int 	ACLGetSendSuccessAckedNow();

void 	ACLACKRun();
void	ACLACKCheckNewPacket();
void	ACLACKUpdateTimeouts();
void	ACLACKCheckRepeats();
void	ACLACKHandleError();

#inline
unsigned int	ACLACKCheckInPacket();
void 	ACLACKSendAck();
void 	ACLACKHandleAck();
void 	ACLACKSendPacketAgain();
void 	ACLACKSendReset();
void 	ACLACKReceiveReset();
void 	ACLACKReset();

/*
#inline
void TLTestMs( unsigned int millisek );
#inline
void TLTestUs( unsigned int muesek );
*/


void ACLACKInit()
{
	ACLSetControlMessagesBehaviour( 0, 1 );
	ACLACKReset();
	bit_clear( ACL_ACK_SEND_SUCCESS );
	ACL_ACK_on = 1;
}

int ACLAckedDataIsNew()
{
	if ( !ACLAdressedDataIsNew()) return 0;

	if ( ACLFoundReceivedType( ACL_TYPE_CAC_HI,ACL_TYPE_CAC_LO ))
	{
		if( *( ACLGetReceivedData( ACL_TYPE_CAC_HI, ACL_TYPE_CAC_LO )) == DATA_PACK )
		{
			ACLSetDataToOld();
			if( !ACL_ACK_packet_checked_in_this_slot ) ACLACKCheckInPacket();
			return 1;
		}
	}

	return 0;
}

int ACLAckedDataIsNewNow()
{
	if ( !ACLAdressedDataIsNewNow()) return 0;

	if ( ACLFoundReceivedType( ACL_TYPE_CAC_HI,ACL_TYPE_CAC_LO ))
	{
		if( *( ACLGetReceivedData( ACL_TYPE_CAC_HI, ACL_TYPE_CAC_LO )) == DATA_PACK )
		{
			if( !ACL_ACK_packet_checked_in_this_slot ) ACLACKCheckInPacket();
			return 1;
		}
	}

	return 0;
}

int ACLSendingBusyAcked()
{
	if( ACLSendingBusy()) return 1;
	if( bit_test( ACL_ACK_SEND_QUEUED ) ) return 1;
	else return 0;
}

int ACLSendPacketAcked( unsigned int add1, add2, add3, add4, add5, add6, add7, add8, timeout )
{
	if( ACLSendingBusyAcked()) return 0;

	bit_set( ACL_ACK_SEND_QUEUED );

	if( ACLGetRemainingPayloadSpace() <= ( ACL_ACK_HEADER + ACL_ADD_HEADER + 6 )) return 0;

	ACLAddNewType( ACL_TYPE_CAD_HI, ACL_TYPE_CAD_LO );
	ACLAddData( add1 );
	ACLAddData( add2 );
	ACLAddData( add3 );
	ACLAddData( add4 );
	ACLAddData( add5 );
	ACLAddData( add6 );
	ACLAddData( add7 );
	ACLAddData( add8 );
	ACLAddNewType( ACL_TYPE_CAC_HI, ACL_TYPE_CAC_LO );
	ACLAddData( DATA_PACK );
	ACLAddData( ++ACL_ACK_own_seq_number );
	ACL_ACK_timer = ACL_ACK_TIMEOUT;
	ACL_ACK_timer_tolerance = ACL_ACK_TIMEOUT_TOLERANCE;

	ACL_ACK_secure_packet_length = ACL_payload_length;
	memcpy( ACL_ACK_secure_buffer, ACL_payload_send, ACL_ACK_secure_packet_length ); // save a copy of whole packet in aclack_secure_buffer.

	if( ACLSendPacket( timeout )) return 1;
	return 0;
}

int ACLGetSendSuccessAcked()
{
	if( bit_test( ACL_ACK_SEND_QUEUED )) return 2;
	if( bit_test( ACL_ACK_SEND_SUCCESS ))
	{
		bit_clear( ACL_ACK_SEND_SUCCESS );
		return 1;
	}
	else return 0;
}

int ACLGetSendSuccessAckedNow()
{
	if( bit_test( ACL_ACK_SEND_QUEUED )) return 2;
	return bit_test( ACL_ACK_SEND_SUCCESS_NOW );
}

void ACLACKRun()
{
	if( ACL_ACK_on )
	{
		ACL_ACK_packet_checked_in_this_slot = 0;
		bit_clear( ACL_ACK_SEND_SUCCESS_NOW );
		ACLACKCheckNewPacket();
		ACLACKUpdateTimeouts();
		ACLACKCheckRepeats();
		ACLACKHandleError();
	}
}

void ACLACKCheckNewPacket()
{
	if( ACLDataIsNewNow())
	{
		if( ACLFoundReceivedType( ACL_TYPE_CAC_HI, ACL_TYPE_CAC_LO ))
		{
			if( ACLMatchesMyIP( ACLGetReceivedData( ACL_TYPE_CAD_HI, ACL_TYPE_CAD_LO ), 0 ))
			{
				if( *( ACLGetReceivedData( ACL_TYPE_CAC_HI, ACL_TYPE_CAC_LO )) == DATA_PACK )
				{
					ACLACKSendAck();
				}
				else if( *( ACLGetReceivedData( ACL_TYPE_CAC_HI, ACL_TYPE_CAC_LO )) == DATA_ACK )
				{
					ACLACKHandleAck();
				}
			}
		}
	}
}

void ACLACKUpdateTimeouts()
{
	if( bit_test( ACL_ACK_SEND_QUEUED ) )
	{
		if( ACL_ACK_timer > 0 )
		{
			--ACL_ACK_timer;
			if( ACL_ACK_timer == 0 )
			{
				--ACL_ACK_timer_tolerance;
				if( ACL_ACK_timer_tolerance )
				{
					bit_set( ACL_ACK_TIMEOUT_QUEUED );
					ACL_ACK_timer = ACL_ACK_TIMEOUT;
				}
				else bit_set( ACL_ACK_ERROR );
			}
		}
	}
	if( ACL_ACK_yet_received_list_timeout )
	{
		--ACL_ACK_yet_received_list_timeout;
		if( !ACL_ACK_yet_received_list_timeout )
		{
			ACLACKReceiveReset();
		}
	}
}

void ACLACKCheckRepeats()
{
	if( bit_test( ACL_ACK_TIMEOUT_QUEUED ) )
	{
		if( !ACLSendingBusy())
		{
			bit_clear( ACL_ACK_TIMEOUT_QUEUED );
			ACLACKSendPacketAgain();
		}
	}
}

void ACLACKHandleError()
{
	if( bit_test( ACL_ACK_ERROR ) )
	{
		ACLACKSendReset();
		bit_clear( ACL_ACK_SEND_SUCCESS );
	}
}

#inline
unsigned int ACLACKCheckInPacket()
{
	unsigned int i, j;

		ACL_ACK_packet_checked_in_this_slot = 1;
		j = ACL_ACK_yet_received_list_position;
		for( i = 0; i < ACL_ACK_YET_RECEIVED_LIST_SIZE; ++i )
		{
			if(( ACL_ACK_yet_received_list[ j ].seq_nr == *( ACLGetReceivedData( ACL_TYPE_CAC_HI, ACL_TYPE_CAC_LO ) + 1 )) &&
				( ACL_ACK_yet_received_list[ j ].add8 == LL_header_received[ 10 ] ) &&
				( ACL_ACK_yet_received_list[ j ].add7 == LL_header_received[ 9 ] ) &&
				( ACL_ACK_yet_received_list[ j ].add6 == LL_header_received[ 8 ] ) &&
				( ACL_ACK_yet_received_list[ j ].add5 == LL_header_received[ 7 ] ) &&
				( ACL_ACK_yet_received_list[ j ].add4 == LL_header_received[ 6 ] ) &&
				( ACL_ACK_yet_received_list[ j ].add3 == LL_header_received[ 5 ] ) &&
				( ACL_ACK_yet_received_list[ j ].add2 == LL_header_received[ 4 ] ) &&
				( ACL_ACK_yet_received_list[ j ].add1 == LL_header_received[ 3 ] ))
			{
				return 0;
			}

			++j;
			if( j == ACL_ACK_YET_RECEIVED_LIST_SIZE ) j = 0;
		}

		ACL_ACK_yet_received_list[ ACL_ACK_yet_received_list_position ].seq_nr = *( ACLGetReceivedData( ACL_TYPE_CAC_HI, ACL_TYPE_CAC_LO ) + 1 );
		ACL_ACK_yet_received_list[ ACL_ACK_yet_received_list_position ].add8 = LL_header_received[ 10 ];
		ACL_ACK_yet_received_list[ ACL_ACK_yet_received_list_position ].add7 = LL_header_received[ 9 ];
		ACL_ACK_yet_received_list[ ACL_ACK_yet_received_list_position ].add6 = LL_header_received[ 8 ];
		ACL_ACK_yet_received_list[ ACL_ACK_yet_received_list_position ].add5 = LL_header_received[ 7 ];
		ACL_ACK_yet_received_list[ ACL_ACK_yet_received_list_position ].add4 = LL_header_received[ 6 ];
		ACL_ACK_yet_received_list[ ACL_ACK_yet_received_list_position ].add3 = LL_header_received[ 5 ];
		ACL_ACK_yet_received_list[ ACL_ACK_yet_received_list_position ].add2 = LL_header_received[ 4 ];
		ACL_ACK_yet_received_list[ ACL_ACK_yet_received_list_position ].add1 = LL_header_received[ 3 ];
		if( ++ACL_ACK_yet_received_list_position == ACL_ACK_YET_RECEIVED_LIST_SIZE ) ACL_ACK_yet_received_list_position = 0;

		ACL_ACK_yet_received_list_timeout = ACL_ACK_RECEIVE_LIST_TIMEOUT;

		return 1;
}

void ACLACKSendAck()
{
	unsigned int i;

		if( ACLSendingBusy()) ACLAbortSending();

		ACLAddNewType( ACL_TYPE_ACM_HI, ACL_TYPE_ACM_LO );
		ACLAddNewType( ACL_TYPE_CAD_HI, ACL_TYPE_CAD_LO );
		for( i = 0; i < 8; ++i )
		{
			ACLAddData( LL_header_receivebuffer[ 3 + i ] );
		}
		ACLAddNewType( ACL_TYPE_CAC_HI, ACL_TYPE_CAC_LO );
		ACLAddData( DATA_ACK );
		ACLAddData( *( ACLGetReceivedData( ACL_TYPE_CAC_HI, ACL_TYPE_CAC_LO ) + 1 ));	// remote seqence number.
		ACLSendPacket( 20 );
}

void ACLACKHandleAck()
{
	unsigned int i;
	unsigned int position;
	unsigned int res = 0;

		if( bit_test( ACL_ACK_SEND_QUEUED ))
		{
			position = ACL_ACK_secure_packet_length - ACL_ACK_HEADER - ACL_ADD_HEADER - 3;
			for( i = 0; i < 8; ++i )
			{
				if( ACL_ACK_secure_buffer[ position + i ] == LL_header_receivebuffer[ 3 + i ] ) ++res;	// checking remote address.
			}
			if( res == 8 )
			{
				if(( ACL_ACK_secure_buffer[ position + 12 ] ) == *( ACLGetReceivedData( ACL_TYPE_CAC_HI, ACL_TYPE_CAC_LO ) + 1 ))	// checking remote sequence number.
				{
					bit_set( ACL_ACK_SEND_SUCCESS );
					bit_set( ACL_ACK_SEND_SUCCESS_NOW );
					ACLACKSendReset();
				}
			}
		}
}

void ACLACKSendPacketAgain()
{
	ACLClearSendData();
	memcpy(ACL_payload_send, ACL_ACK_secure_buffer, ACL_ACK_secure_packet_length);
	ACL_payload_length = ACL_ACK_secure_packet_length;

	ACLSendPacket( 10 );
}

void ACLACKSendReset()
{
	bit_clear( ACL_ACK_SEND_QUEUED );
	bit_clear( ACL_ACK_TIMEOUT_QUEUED );
	ACL_ACK_secure_packet_length = 0;
	ACL_ACK_timer = 0;
	ACL_ACK_timer_tolerance = 0;
	bit_clear( ACL_ACK_ERROR );
}

void ACLACKReceiveReset()
{
	unsigned int i;

		ACL_ACK_yet_received_list_position = 0;
		for( i = 0; i < ACL_ACK_YET_RECEIVED_LIST_SIZE; ++i )
		{
			ACL_ACK_yet_received_list[ i ].seq_nr = 0;
		}
		ACL_ACK_yet_received_list_timeout = 0;
}

void ACLACKReset()
{
	ACLACKSendReset();
	ACLACKReceiveReset();
}

/*
#inline
void TLTestMs( unsigned int millisek ) {

	output_high( test_pin );
	delay_ms( millisek );
	output_low( test_pin );
	delay_ms( millisek );
}

#inline
void TLTestUs( unsigned int muesek ) {

	output_high( test_pin );
	delay_us( muesek );
	output_low( test_pin );
	delay_us( muesek );
}
*/