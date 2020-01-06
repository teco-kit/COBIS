/**
  * Copyright (c) 2004, Telecooperation Office (TecO),
  * Universitaet Karlsruhe (TH), Germany.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions
  * are met:
  *
  *   * Redistributions of source code must retain the above copyright
  *     notice, this list of conditions and the following disclaimer.
  *   * Redistributions in binary form must reproduce the above
  *     copyright notice, this list of conditions and the following
  *     disclaimer in the documentation and/or other materials provided
  *     with the distribution.
  *   * Neither the name of the Universitaet Karlsruhe (TH) nor the names
  *     of its contributors may be used to endorse or promote products
  *     derived from this software without specific prior written
  *     permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **/


/*
//********************************************************
//	RF stack for TecO Smart-Its/Particle Computer
//  listen before talk
//
//	Albert Krohn				(krohn@teco.edu)
//  sabin wendhack				(wendhack@teco.edu)
//	Telecooperation Office
//  University of Karlsruhe
// 	Germany
//  www.teco.edu
//
#00 : awareconv9#000.c bases on awarecon#90.c
#01 : fixed get by type functions 

//********************************************************
*/



// these are for fixed variables (timing)
#reserve 0x20	// rx1
#reserve 0x21	// rx2
#reserve 0x22	// rx3
#reserve 0x23	// rx4
#reserve 0x24	// drx1
#reserve 0x25	// drx2
#reserve 0x26	// bit_count
#reserve 0x27	// shift
#reserve 0x28	// good
#reserve 0x29	// rf_last_err
#reserve 0x2a
#reserve 0x2b
#reserve 0x2c
#reserve 0x2d
#reserve 0x2e
#reserve 0x2f
#reserve 0x30	// rf_random
#reserve 0x31	// rf_random_sum
#reserve 0x32	// rftx



typedef unsigned byte  	UInt8;
typedef unsigned long  	UInt16;


// this is debug stuff
#define SENDER			0
#define DEBUG			0
#define SYNC_DEBUG		0		// set this to one: on pin 4 is the sync time visible to compare with other devices
//#define LL_PACKET_SENDER 0


#define VERSION			9
#define MY_MAC_ID		141					//141

// State Machine Timings
#define RF_FSM_TIME_SYNC		0			//one count is one bit or TIMER1H = 50,1 us
#define RF_FSM_TIME_SYNC_RECEIVE  88
#define RF_FSM_TIME_SLOT_END		255
#define RF_FSM_TIME_EARLY_SLOT_END (RF_FSM_TIME_SLOT_END-2)

#define LBT_FSM_TIME_SYNC_SEND  	0
#define LBT_FSM_TIME_SYNC_RECEIVE  	88

// State Machine
#define FSM_STATE_START 1
#define FSM_STATE_SYNC	2
#define FSM_STATE_STATISTIC  3
#define FSM_STATE_ARBI  4
#define FSM_STATE_SEND_DATA  5
#define FSM_STATE_REC_DATA  8
#define FSM_STATE_ALONE 6
#define FSM_STATE_LISTEN 7					// LBT
#define FSM_STATE_DEBUG 99
#define FSM_STATE_ERROR 10
#define FSM_STATE_END	20

// RF_TRAILING, SYMBOL
#define RF_INITIAL_LISTEN_SLOTS_DEFAULT 			2				// duration in slot that an unsynced new smartit uses to listen
#define RF_ARBITRATION_MINIMUM_SAMPLES 				7				// no of (uart)samples with which a slot is assumed "busy"

// RF_SYMBOLS            all end with "0" for turning off the tx pin on rf module
#define RF_SYMBOL_SYNC1 		0b11100011
#define RF_SYMBOL_SYNC2			0b10001110
#define RF_SYMBOL_SYNC3			0b00111000
#define RF_SYMBOL_SYNC4			0b11100011
#define RF_SYMBOL_SYNC5			0b10001111
#define RF_SYMBOL_SYNC6			0b11000000
#define RF_SYMBOL_SYNC7			0b11100000

#define RF_SYMBOL_SYNC			0b10110010

#define RF_SYMBOL_SYNCRECEIVED  0b00110010  			// the received Sync symbol (is the "sync"-symbol, but 3 times as fast
#define RF_SYMBOL_FASTTRAILER	0b10101010				// trailer for data packets
#define RF_SYMBOL_FASTTRAILER1	0b11100011				// trailer for data packets
#define RF_SYMBOL_FASTTRAILER2	0b10001110				// trailer for data packets
#define RF_SYMBOL_FASTTRAILER3	0b00111000				// trailer for data packets
#define RF_SYMBOL_SOP			0b11001010
#define RF_SYMBOL_EOP			0b00000000				// to reset the spi


// RF stati
#define RF_STATUS_SENDING			0b00000001
#define	RF_STATUS_SEND_SUCCESS		0b00000010
#define RF_STATUS_NEW_PACKET		0b00000100
#define RF_STATUS_RECEIVE_ERROR		0b00001000
#define RF_STATUS_LAYER_ON			0b10000000		// one if stack is running
#define RF_STATUS_INSERTED_CTRLMSG	0b01000000		// is being set if waiting normal msg was interrupted due t incoming control/management msg
#define RF_STATUS_NEW_DATA			0b00010000		// new data arrived
#define RF_STATUS_JUST_SENT			0b00100000		// data send in this slot

// Sync states
#define SYNC_STATE_SYNCED		1
#define SYNC_STATE_ALONE		2
#define SYNC_MAX_MISSED_SYNCS 	7		// after these empty sync=> state=alone
#define SYNC_RECOVER_SYNCS		4		// after these syncs it's recover state (only sync receive)
#define SYNC_STATE_DEBUG		99

//sync modes
#define RF_SYNC_MODE_ACTIVE		1		// searches for partners and syncs
#define RF_SYNC_MODE_PASSIVE	2		// does not search for partners but syncs

#define RF_SYNC_RATE_DEFAULT	80		// normal = 30% sync rate

// LBT
#define LBT_PREAMBLE_LENGTH			236
#define LBT_PREAMBLE_MIN_RECOGNIZED	5
#define	LBT_NOISELEVEL				9
#define LBT_TOLERANCE				5	// tolerance of wrongly received bits of the preamble, at minimum 3 because sync produces two or three bad bits....
#define LBT_WAITING_TIME_CONSTANT	24000 // = 4,8 ms => arbitrary waiting time ranges between 0 and 8,3ms (13,1-4,8ms)
#define LBT_LISTENING_TIME			500 // 500 tested

#define LBT_TIME_GOOK_LOOP			33	// measured miliseconds (to check timeout)
#define LBT_TIME_CHECK_LOOP			8	// measured miliseconds (to check timeout)
#define LBT_TIME_CHECK_IN_LOOP		24	// measured miliseconds (to check timeout)

#define LBT_SYMBOL_SYNC1			0b10101111
#define LBT_SYMBOL_SYNC2			0b11000000
#define LBT_SYMBOL_SYNC3			0b11100000

// LL
#define LL_HEADER_SIZE			12
#define LL_PAYLOAD_SIZE			64
#define LL_TAIL_SIZE			2
#define LL_FRAME_MAX_SIZE		(LL_HEADER_SIZE+LL_PAYLOAD_SIZE+LL_TAIL_SIZE)

// ACL
#define ACL_SUBSCRIPTIONLIST_LENGTH	16
#define ACL_CONTROL_MESSAGES_TIMEOUT	30
#define ACL_TYPE_ACM_H		ACL_TYPE_ACM_HI
#define ACL_TYPE_ACM_L		ACL_TYPE_ACM_LO

// for LED
#define LEDS_NORMAL			1
#define LEDS_OFF			0	//leds off and stack doesnt touch them
#define LEDS_ON_SEND		2
#define LEDS_ON_RECEIVE		3
#define LEDS_ONLY_ONE		5
#define LEDS_ON_CRC_ERROR	6	// blue led on crc error
#define LEDS_ON_TRAFFIC		7	// blue led on send/receive








// function declarations
//-------------------------------------------------------------------------------------
// pic layer... discontinued

/*
void PICInit();
int PICCombineTris(int sys,mask,user);
*/


// RF Layer
#pragma inline
void RFSetModeSleep();
#pragma inline
void RFSetModeAsk();
#pragma inline
void RFSetModeOok();
#pragma inline
void RFSetModeReceive();
#pragma inline
void RFSetModeSensitive();
#pragma inline
void RFSpiSendByte();
#pragma inline
void RFDeleteTimer1Overflow();
void RFInit();
UInt8 RFGetRandom();
#pragma inline
void RFPowerModuleOn();
#pragma inline
void RFPowerModuleOff();
#pragma separate
int DebugGiveOut(int i);


#pragma inline
void RFInitUart();
#pragma inline
void RFUartStartContinuous();
#pragma inline
void RFUartClearFifo();
#pragma inline
int RFUartGetByte();
#pragma inline
void RFSpiWait();
#pragma inline
void RFUartOff();
#pragma inline
void RFUartOn();
#pragma inline
void RFSpiOff();
#pragma inline
void RFSpiOn();
#pragma inline
void RFWaitForTimer1Exact(int timeout);
#pragma inline
void RFWaitForTimer1ExactNew(int timeout);	// runtime of function ends exactly before TIMER1_H = timeout
#pragma inline
void RFSpiClear();
#pragma inline
void RFWaitForNextBitTimeExact();
int RFSendSync();
int RFSynchronize(int Timeout,int wait_slots);
void RFInitRandom();
#pragma separate
int RFStatistic(boolean receive);
int RFArbitration(long rf_arbi_pos);
int RFSendData();
int RFReceiveData();
#pragma inline
void RFScramble();
int RFCreateAliveSymbol();
void RFNewRandom();
void RFStop();
void RFSetFieldStrength(int power);		// unimplemented
#pragma inline
void RFStart();
void RFReceiveScrambledByte();
void RFReceiveByte();
void RFSetSyncRate(int rate);
void RFSetSyncMode(int mode);
void RFSetInitialListenSlots(int slots);
#pragma separate
int RFAlone();
#pragma separate
void RFCheckSubscriptions();
void RFShowSynchronization();
long RFMicroSecondsUntilNextSlot();
#pragma separate
void RFEvaluateStatistic();
int RF100usLeft();
int RF200usLeft();
int RF500usLeft();


enum lbt_listen{LBT_NOISE, LBT_RECEIVE, LBT_FREE};
lbt_listen LBTListen( unsigned long timeout );	// listens  and returns RECEIVE, if preamble and syncsymbol detected, NOISE, if channel busy and FREE. timeout in ms.
int LBTSend(); //sends a packet (preamble, syncsymbol & packet)
#pragma inline
long LBTGetWaitingTime(); // calculates arbitrary waiting time within a range determined by LBT_WAITING_TIME_CONSTANT


enum ll_data_in_last_slot{LL_CRC_ERROR,LL_ACM,LL_GOOD,LL_NONE};

// LL Layer
void LLStart();
void LLStop();
int LLSendPacket(int slot_limit);
int LLSendingBusy();
int LLGetSendSuccess();
int LLIsActive();
void LLLockReceiveBuffer();
void LLReleaseReceiveBuffer();
int LLReceiveBufferLocked();
void RFDelaySlots(int slots);
void LLSlotEnd();
void LLSetDataToOld();
void LLSetDataToNew();
void LLInit();
int  LLGetFieldStrength();
void LLSetFieldStrength(int value);
long LLCalcCRC16(byte *header_data,byte *payload_data,byte payload_size);//,int payload_size);
void LLAbortSending();
int  LLGetRemainingPayloadSpace();
int LLSetSendingSuccess();
int LLDataIsNew();
int LLSentPacketInThisSlot();
int LLGetIDFromHardware();								// takes id either from internal or external eeprom



//ACL
int ACLSubscribe(byte LL_type_h,LL_type_l);				//adds a type to the subscription list
int ACLUnsubscribe(byte LL_type_h,LL_type_l);			//deletes a type out of the subscription list
int ACLFlushSubscriptions();							//deletes all subscription (not the default ones)
int ACLSubscribeDefault();								//subscribes to the default types (control msgs..)
int ACLVerifySubscription(int type_h,int type_l);		//checks if a subscription is there
void ACLInit();											//start ACL and lower layers, resets the whole stack
//#pragma inline
int ACLProcessControlMessages();						//internal: is called if control msg is there
void ACLSetFieldStrength(int power);					//sets the field strength of transmitter signals
int ACLSendingBusy();									//returns true if LL has Packet in send queue
int ACLGetSendSuccess();								//returns the result of last Packet transmission
int ACLSendPacket(int slot_timeout);					//starts packet transmission, return values!
int ACLAddNewType(int type_h, int type_l);				//adds a type into the ACL send buffer, return values!
int ACLAddData(int data);								//adds one byte data into the ACL send buffer, return values!
void ACLAbortSending();									//stops a running transmission
void ACLSubscribeAll();									//subscribes to any possible type (all packets are received)
int ACLMatchesMyIP(char *buffer,int start);				//checks, if buffer holds my IP

#define ACLGetDataLength(data) *(data-1)

char* ACLGetReceivedData(int type_h, int type_l);		//returns the ACL payload data of the given type in the last received packet
int ACLGetReceivedPayloadLength();						//returns the number of bytes received
void ACLSetControlMessagesBehaviour(boolean ignore, boolean pass);	//if ignore==true: don't react on any control msg; if pass==true: give control msgs to ACL as received payload packets
signed int ACLGetReceivedDataLength(int type_h, int type_l);	//returns number of bytes received of given type in last msg
int ACLSentPacketInThisSlot();							//is true until next slot if msg was send in this slot
void ACLAnswerOnACM();									//if a control msgs comes in that requires an answer, it is answered
void ACLNoAnswerOnACM();									//if a control msgs comes in that requires an answer, it will not be answered
int ACLClearSendData();									//deletes the send buffer of ACL
void ACLStart();										//restart ACL after ACLstop()
void ACLStop();											//stops the RF stack. Everything is hold, continues after ACLstart. msg stay in queue
int ACLGetRemainingPayloadSpace();						//returns the number of free bytes in the transmit buffer
void ACLLockReceiveBuffer();							//locks the receive buffer: no new msgs are received
int ACLReceiveBufferLocked();							//returns true if receivebuffer is locked
void ACLReleaseReceiveBuffer();							//un-lock the receivebuffer;
void ACLSetDataToOld();									//set received data to "old": means ACLDataIsNew will not return true unless a new packet was received
void ACLSetDataToNew();									//set received data to "new": means ACLDataIsNew will return true every slot
int ACLDataIsNew();										//returns true if data in receive buffer is "new"
char ACLGetReceivedByte(int type_h, int type_l, int position);		//returns a single received byte on the position
void ACLStartup();										// is the first function in any case,  runs selftest and aclinit
int ACLDataReceivedInThisSlot();						//returns true if data was received in this slot
int ACLDataIsNewNow();									//==ACLDataReceivedin this slot
int ACLFoundReceivedType(int type_h, int type_l);		//returns true if type was found
int ACLAdressedDataIsNew();								// return1 if adressed data came
int ACLAdressedDataIsNewNow();							// return 1 if adressed data came in this slot
int ACLSendPacketAdressed(unsigned int add1, add2, add3, add4, add5, add6, add7, add8, timeout );//sends out the current packet with a target ID
void ACLWaitForReady();									// waits until ACL has finished all jobs like packet send etc.

//DEBUG, APP, SERVICE, and all the rest
void DebugBuildTestPacketACL();

//#pragma separate
//void AppRfSlotEndCallBack();

void AppSetRealTimeClock(long value);					//sets the real time clock to given value
int AppGetRealTimeClockH();								//returns the HIGh byte of the real time clock
int AppGetSupplyVoltage();								//returns the supply voltage; 255 means 3.0 V, scaled linear

void AppSetLEDBehaviour(int ledstyle);					//sets how LED should behave (blicking, blue on transmit, sync etc, off)
#pragma separate
void AppSetLEDs();										//internal: updates the state of the LEDs
int AppSelfTest(int* result);										//runs the selftest if a selftest connector is on

void SDJSRF();											//runs SDJS if initiated by SDJS start type either on receive or on send side.

#pragma separate
void SlotEndCallBack();



//DECLARE EXTERNAL DEPENDENCIES:		(dirty hack)
void OtapInit();
void OtapSetID();
void OtapReceive();
void OtapAnswerRepeat();
void OtapReset();


//---------------------------------------------------------------------------------------------------------
// constants

const int RF_BIT_POS_HIGH_PERIOD[256]=
{
0,      1,  128,    2,  128,  128,  128,    3,  128,  128,  128,  128,  128,  128,  128,  4,
128   128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  5,
128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  6,
128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  7,
128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,
128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  128,  8
};


// RFScrambler as used in IEEE 802.11
//const int RFScrambler[16]=
//{
//0b00001110 ,0b11110010 ,0b11001001 ,0b00000010 ,0b00100110 ,0b00101110 ,0b10110110 ,0b00001100 ,0b11010100
//0b11100111 ,0b10110100 ,0b00101010 ,0b11111010 ,0b01010001 ,0b10111000 ,0b1111111
//};




const int RF_NUMBER_OF_BITS[256]=
{
 0,   1,   1,   2,   1,   2,   2,   3,   1,   2,   2,   3,   2,   3,   3,   4,
 1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5,
 1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5,
 2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
 1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5,
 2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
 2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
 3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7,
 1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5,
 2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
 2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
 3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7,
 2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6,
 3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7,
 3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7,
 4,   5,   5,   6,   5,   6,   6,   7,   5,   6,   6,   7,   6,   7,   7,   8
};

/*
const int RF_4_to_8[16]=
{
	0b00000000,
	0b00000011,
	0b00001100,
	0b00001111,
	0b00110000,
	0b00110011,
	0b00111100,
	0b00111111,
	0b11000000,
	0b11000011,
	0b11001100,
	0b11001111,
	0b11110000,
	0b11110011,
	0b11111100,
	0b11111111
};

*/
const int RF_4_to_8[16]=
{
	0b00000000,
	0b00000010,
	0b00001000,
	0b00001010,
	0b00100000,
	0b00100010,
	0b00101000,
	0b00101010,
	0b10000000,
	0b10000010,
	0b10001000,
	0b10001010,
	0b10100000,
	0b10100010,
	0b10101000,
	0b10101010
};


// these are the estimation of number of devices, if bit 0..7 is 50% there
const int RF_STATISTIC_TABLE[8]=
{
	1,
	2,
	5,
	11,
	22,
	44,
	88,
	177
};

//give in the found position of 50% max; this here returns the sync rate setting
/*
const int RF_SYNC_RATES[8]=
{
	100,
	70,
	40,
	19,
	10,
	4,
	2,
	2
};
*/const int RF_SYNC_RATES[8]=
{
	128,
	100,
	64,
	23,
	11,
	6,
	2,
	2
};


// global variables
//
//
int rf_fsm_state;
int rf_status;																// holds the state of rf layer like busy, receiving succes , err, etc

int rf_sync_state;
int rf_sync_mode;
int rf_sync_rate;		// 0 means never send a sync; 255 means always send a sync

int rf_missed_syncs;
long rf_slotcounter;
long rf_longbuffer;
int rf_symbol_am_alive;											// always make sure that the lowest bit is zero
int rf_statistic_am_alive;										// hold the last valid situation
int rf_field_strength;
int rf_initial_listen_slots;									// the number of listenslots before a lonely note sends a sync
boolean rf_new_statistic_value;									// is set true if a new statistic value was there
int rf_LED_style;
int rf_random_8;
int rf_number_of_devices;										// gives the number of devices in the environment



int rf_statistic_values[9];									// statistics only on 18f platforms 0..7 holds bits, 8 the counter

boolean	lbt_time_set;
boolean lbt_state_sending;


int LL_slots_left;
int LL_payload_length;											// hold the actual size of LL frame (not includes ll header, lltail)
int LL_sequence_no;
boolean LL_receive_buffer_locked;
boolean LL_received_packet_in_this_slot;	//the reincarnation of the good old mac_msg_recieved_in_last_slot
long LL_longbuffer;

int LL_payload_received[LL_PAYLOAD_SIZE];
int LL_payload_send[LL_PAYLOAD_SIZE];
int LL_payload_receivebuffer[LL_PAYLOAD_SIZE];

int LL_header_received[LL_HEADER_SIZE];
int LL_header_send[LL_HEADER_SIZE];
int LL_header_receivebuffer[LL_HEADER_SIZE];

int LL_tail_received[LL_TAIL_SIZE];
int LL_tail_send[LL_TAIL_SIZE];
int LL_tail_receivebuffer[LL_TAIL_SIZE];

int LL_payload_write_pos;
ll_data_in_last_slot	LL_last_data;






int ACL_payload_send[LL_PAYLOAD_SIZE];
int ACL_payload_length;

int ACL_subscriptions[ACL_SUBSCRIPTIONLIST_LENGTH][2];
boolean ACL_send_buffer_locked;
boolean ACL_subscribe_all;
boolean ACL_ACM_answers;		// answer on ACM messages like helo and so on
int ACL_write_position;


boolean ACL_ignore_control_messages;
boolean ACL_pass_control_messages;


int ACL_i;		// dirty workaround because not enough RAM
int ACL_res;


// SDJS globals.
boolean sdjs_start;					// is set if sdjs start command is found either in received data or in own sendbuffer. if found set SDJSRF() is executed at a later point.
unsigned int sdjs_type;				// determines type of sdjs that is executed. at the moment there's only type 1, type 0 means no sdjs is executed.


/*
//globals for ota packets
#if PROCESSOR_TYPE==1
	long ota_TotalNumberOfLines;
	long ota_ProgramLines;
	long ota_ExpectingLineNumber;
	long ota_LastReceivedLineNumber;
	long ota_EepromWriteAddr;
	int ota_DelaySlotCounter;
	int ota_BreakCounter;
	int ota_EndCounter;
	int ota_ProgramCRCh;
	int ota_ProgramCRCl;
	boolean ota_CRCok;
	boolean ota_packet_just_received;
#endif

*/

//boolean selftest_active;







#pragma byte rf_rx1			=0x20
#pragma byte rf_rx2			=0x21
#pragma byte rf_buff			=0x22
#pragma byte rf_thisbyte		=0x23
#pragma byte rf_pos			=0x24

#pragma byte lbt_noiselevel	=0x25

#pragma byte rf_duty_cycle		=0x26
#pragma byte rf_bit_count		=0x27
#pragma byte rf_shift			=0x28
#pragma byte rf_transition0	=0x29
#pragma byte rf_transition1	=0x2a
#pragma byte rf_transition2	=0x2b
#pragma byte rf_transition3	=0x2c
#pragma byte rf_limit			=0x2d
#pragma byte rf_random_l 		=0x2e		//bits: 0: send request, bits1:arbi won,
#pragma byte rf_random_h		=0x2f

#pragma byte lbt_tolerance		=0x30

#pragma byte rf_timeout    	=0x31	// current random number. used for generating "random" numbers in RFGetRandom()
#pragma byte rf_tx				=0x32






void newlifesign()
{
	int i;
	int old_state;

	old_state=rf_LED_style;
	AppSetLEDBehaviour(LEDS_OFF);

	for (i=0;i<3;i++)
	{
		PCLedRedOn();
		PCLedBlueOff();
		RFDelaySlots(8);
		PCLedRedOff();
		PCLedBlueOn();
		RFDelaySlots(8);
	}
	AppSetLEDBehaviour(old_state);

}




//***********
//
// das ist die neue state machine
//
//**********
#int_timer1
int fsm()
{
lbt_listen result;
//unsigned int i;


goto RfFsmStart;		// start of state machine

//**********************************************************************

RfFsmStart:
	rf_fsm_state=FSM_STATE_START;

//	output_high( PIN_CONN_18 );
//	output_low( PIN_CONN_18 );
//	output_high( PIN_CONN_18 );
//	output_low( PIN_CONN_18 );
//	output_high( PIN_CONN_18 );

	lbt_time_set = FALSE;

	rf_status&= ~RF_STATUS_JUST_SENT;				//delete the just sent flag
	LL_received_packet_in_this_slot=false;			//unset the reception flag
	LL_last_data=LL_NONE;

	rf_symbol_am_alive=RFCreateAliveSymbol();		// prepare symbol

	RFDeleteTimer1Overflow();

	SPIInit(SPI_MODE_TIMER2);
	RFSpiOn();

	RFInitUart();							// set correct baudrate etc
	RFUartOn();

	RFSetModeReceive();								// to wake up from sleep (this takes typ. 20us)

	goto RfFsmListen;


//**********************************************************************

RfFsmListen:
	rf_fsm_state=FSM_STATE_LISTEN;

	RFSetModeReceive();

	result = LBTListen( LBT_LISTENING_TIME );

	if( result == LBT_RECEIVE )
	{
		lbt_state_sending = FALSE;
		goto RfFsmReceiveData;
	}
	else if(( rf_status & RF_STATUS_SENDING ) && ( result == LBT_FREE ))  //something to send and channel free
	{
		lbt_state_sending = ~lbt_state_sending;

		if( !lbt_state_sending ) goto RfFsmSend; 	// send
		else
		{
				set_timer1( LBTGetWaitingTime() );	// listen again after an arbitrary waiting time
				goto RfFsmEnd;
		}
	}
	else // result == LBT_NOISE
	{
		lbt_state_sending = FALSE;
		goto RfFsmEnd;
	}

	goto RfFsmEnd;


//**********************************************************************

RfFsmSend:
	rf_fsm_state=FSM_STATE_SEND_DATA;

	RFSetModeAsk();

	LBTSend();

	LLSetSendingSuccess();

	goto RfFsmEnd;

//**********************************************************************

RfFsmReceiveData:

	rf_fsm_state=FSM_STATE_REC_DATA;

	lbt_state_sending = FALSE;

	if (RFReceiveData() != 0)
	{
//		PCLedBlueOn();					//debug
//		output_high( PIN_CONN_18 );
	}

	goto RfFsmEnd;

//**********************************************************************

RfFsmEnd:

	rf_fsm_state=FSM_STATE_END;

	RFSetModeReceive();
	rf_tx=0;RFSpiSendByte();					// for a secure zero state
	RFUartOff();									// save energy
	RFSpiOff();										// save energy

	RFSetModeSleep();							// switch off the rf module

	rf_slotcounter++;

	AppSetLEDs();						// sets the leds

	RFDeleteTimer1Overflow();

	if (rf_status & RF_STATUS_LAYER_ON)
	{
		enable_interrupts(INT_TIMER1);		// make sure to keep timer 1 running
	}
	else
	{
		RFStop();							// if layer was switched off; make sure the rf stack stops here
	}

	LLSlotEnd();

	RFNewRandom();

	OtapAnswerRepeat();						//repeat answer packets if no answer from return

	if(!selftest_active) SlotEndCallBack();	// call CallBack, but not if still in selftest

/*	if( lbt_time_set )						//debug
	{
		RFWaitForTimer1ExactNew( 200 );
		output_high( PIN_CONN_17 );
		output_low( PIN_CONN_17 );
	}
*/
//	PCLedBlueOff();							//debug
//	output_low( PIN_CONN_18 );

	return 0;										//back to application main - level

//**********************************************************************

RfFsmError:

		rf_fsm_state=FSM_STATE_ERROR;

		PCLedRedOn();
		PCLedRedOff();

	goto RfFsmEnd;

//**********************************************************************

}






// RFMODES of tr1001

#pragma inline
void RFSetModeSleep()
{
	output_low(PIN_RF_CTR0);
	output_low(PIN_RF_CTR1);
}

#pragma inline
void RFSetModeAsk()
{
	output_low(PIN_RF_CTR0);
	output_high(PIN_RF_CTR1);
}

#pragma inline
void RFSetModeOok()
{
	output_high(PIN_RF_CTR0);
	output_low(PIN_RF_CTR1);
}

#pragma inline
void RFSetModeReceive()
{
	output_high(PIN_RF_CTR0);
	output_high(PIN_RF_CTR1);
	output_high(PIN_RF_SENSITIVE);
}

#pragma inline
void RFSetModeSensitive()
{
	output_high(PIN_RF_CTR0);
	output_high(PIN_RF_CTR1);
	output_low(PIN_RF_SENSITIVE);
}




#pragma inline
void RFInitUart()
{
	#pragma asm
		movlw 	0x04
		movwf 	RF_SPBRG		// set correct baudrate   // ehemals PI_SPBRG
		bcf		RF_TXSTA,2		// set low speed
		bsf		RF_TXSTA,4		// set synchronous
		bsf		RF_TXSTA,7		// set synchronus mode MASTER
		bcf		RF_TXSTA,6		// 8 bit transmit

		bsf		RF_UART_SPEN		// enable serial port (power on)
		bcf		RF_UART_SREN		// disable single receive
		bcf		RF_RCSTA,6		// 8 bit reception
		bcf     RF_UART_CREN		// disable continuous receive

	#pragma endasm

	bit_clear(RF_TXSTA,5);		//transmit on pin_c6 disable

}


// one bit of buff is one sample of spi (no oversampling!!)
// reuse of rf_rx1 as trash buffer
#pragma inline
void RFSpiSendByte()
{
	#pragma asm
			MOVF	RF_SSPBUF, W	// read SSBUF for clear -> Akku (every byte is also received)
			MOVWF	rf_rx1			// load to nirvana (must do this, otherwise compiler kills command)
			MOVF	rf_tx, W			// load rf_shift Register
			MOVWF	RF_SSPBUF		// Akku -> SSBUF
	#pragma endasm
}


#pragma inline
void RFSpiWait()
{
	//
	#pragma asm
		LOOP3:
			BTFSS 	RF_SSPSTAT_BF		// sspstat<bf> wait for free
			GOTO 	LOOP3
	#pragma endasm
	//
}


#pragma inline
void RFSpiClear()
{

	int nirvana;
	#pragma asm
			MOVF	RF_SSPBUF, W	// read SSBUF for clear -> Akku (every byte is also received)
			MOVWF	nirvana			// load to nirvana (must do this, otherwise compiler kills command)
	#pragma endasm
}



//sends out sync at time sync
/*int RFSendSync()
{
	int i;
	// first wait for correct time
	if (RF_TIMER1H>= RF_FSM_TIME_SYNC) return 0;
//	RFWaitForTimer1Exact(RF_FSM_TIME_SYNC);
	RFWaitForNextBitTimeExact();


    rf_tx=RF_SYMBOL_SYNC1;RFSpiSendByte();RFSpiWait();
    rf_tx=RF_SYMBOL_SYNC2;RFSpiSendByte();RFSpiWait();
    rf_tx=RF_SYMBOL_SYNC3;RFSpiSendByte();RFSpiWait();
    rf_tx=RF_SYMBOL_SYNC4;RFSpiSendByte();RFSpiWait();
    rf_tx=RF_SYMBOL_SYNC5;RFSpiSendByte();RFSpiWait();
    rf_tx=RF_SYMBOL_SYNC6;RFSpiSendByte();RFSpiWait();
    rf_tx=RF_SYMBOL_SYNC7;RFSpiSendByte();RFSpiWait();


	return 1;
}
*/



#pragma inline
void RFDeleteTimer1Overflow()
{
	bit_clear(RF_TIMER1_IF);		// clear TMR1IF i
}



void RFSetSyncRate(int rate)
{
	rf_sync_rate=rate;
}
void RFSetSyncMode(int mode)
{
	rf_sync_mode=mode;
}

void RFSetInitialListenSlots(int slots)
{
	rf_initial_listen_slots=slots;
}



void RFInit()
{


	//set my own pins to the necessary values


	bit_clear(TRIS_RF_CTR0);		//set to output
	bit_clear(TRIS_RF_CTR1);
	bit_clear(TRIS_RF_TX);
	bit_set(TRIS_RF_RX);
	bit_clear(TRIS_RF_POWER);
	bit_clear(TRIS_RF_SENSITIVE);


	RFSetModeSleep();
	RFPowerModuleOn();											// turn rf module to sleep


	//PICInit();
	// setup timers
	setup_timer_1( T1_INTERNAL | T1_DIV_BY_1);
	bit_clear(RF_TIMER1_CON,7);								// no 16bit read
	setup_timer_2( T2_DIV_BY_1,19,2);					// 8.0 µS
	bit_clear(RF_TIMER1_CON,3);								// turn off external oszi



	//RFSetModeSleep();											// set rf module asleep
	//	RFPowerModuleOff();											// turn off rf module

	// setup serial ports
	RFInitUart();
	RFUartOff();
	//setup_spi( SPI_MASTER | SPI_L_TO_H | SPI_CLK_T2);
	SPIInit(SPI_MODE_TIMER2);
	rf_tx=0;
	RFSpiSendByte();											//  to have bit BF set
	RFSpiWait();rf_tx=0;RFSpiSendByte();						//****new to have output definitive low
	RFSpiOff();

	RFInitRandom();

	rf_status=RF_STATUS_LAYER_ON|0;								// init the status
	rf_sync_mode=RF_SYNC_MODE_ACTIVE;							// normal sync mode with looking for and syncing
	rf_sync_rate=RF_SYNC_RATE_DEFAULT;							// normal 50% sync rate
	rf_initial_listen_slots=RF_INITIAL_LISTEN_SLOTS_DEFAULT;	// see default def

	rf_field_strength=32;										// init the fieldstrength to max
	rf_slotcounter=0;
	rf_statistic_am_alive=0;

	for (rf_pos=0;rf_pos<=8;rf_pos++) rf_statistic_values[rf_pos]=0;	// clear statistics

	sdjs_type = 0;   // switch off sdjs.
}



#pragma inline
void RFStart()
{

	rf_sync_state=SYNC_STATE_ALONE;						// at a start I'm never synced, so state= alone

	if (DEBUG) rf_sync_state=SYNC_STATE_DEBUG;

	RFSetModeSleep();									//****new

	RFPowerModuleOn();									// turn on rf module
	DelayMs(90);										//*** for waiting until tr1001 capacity is charged


	RFSetFieldStrength(rf_field_strength);

	PCLedRedOn();
	DelayMs(200);
	PCLedRedOff();
	DelayMs(200);


	rf_status|=RF_STATUS_LAYER_ON;

	RFDeleteTimer1Overflow();
	set_timer1(65000);										// start rf frame
	set_timer2(0);
	enable_interrupts(INT_TIMER1);						// enable rf interrupt
}




void RFSetFieldStrength(int power)
{
	rf_field_strength=power;								// update layer variable

		// if poti is the MAX5161
		{
			output_high(PIN_POTI_UD);							// set direction up
			for(rf_shift=32;rf_shift>0;rf_shift--)				// count till zero
			{
				output_high(PIN_POTI_INC);						// cycle
				output_low(PIN_POTI_INC);
			}
			// now poti has max resistance
			output_low(PIN_POTI_UD);							// set direction downwards
			for(;power>0;power--)
			{
				output_high(PIN_POTI_INC);						// cycle
				output_low(PIN_POTI_INC);
			}
		}

}


// listens  and returns RECEIVE, if preamble and syncsymbol detected, NOISE, if channel busy and FREE. timeout in ms.
lbt_listen LBTListen( unsigned long timeout )
{

	// use of globals:
	// rf_transition0 	minimum number of preamble-bits recognized
	// rf_transition1	timeout low byte in ms
	// rf_transition2	timeout high byte in ms
	// rf_shift			preamble is shifted in to detect sync symbol
	// rf_thisbyte		byte read from uart (either 8samples respectively one bit, or one byte read by RFUartGetByte()
	// rf_buff			recognized bit
	// rf_pos			recognized bit
	// rf_timeout		timeout occurred if set
	// lbt_tolerance	counting bad bits of preamble
	// lbt_noiselevel	incrementing with found ones



	unsigned int noiselevel;

//	output_high( PIN_CONN_18 );		//debug

	rf_transition1 = lo( timeout );
	rf_transition2 = hi( timeout );

	lbt_tolerance = LBT_TOLERANCE;


	noiselevel = 0;


	RFUartStartContinuous();
	RFUartClearFifo();

	lbt_noiselevel = 0;
	rf_timeout = 0;

	goto gook_new;

	gook_from_check:

		lbt_noiselevel += (( 3 * ( LBT_PREAMBLE_MIN_RECOGNIZED - rf_transition0 )) >> 1 ); // add average detected bytes from last check_loop to noiselevel
		if( lbt_noiselevel > LBT_NOISELEVEL ) goto noise;

		if( rf_timeout ) goto time_out;

	gook_new:
		rf_shift = 0;
		rf_transition0 = LBT_PREAMBLE_MIN_RECOGNIZED;

	gook:
			#pragma asm
                        nop
		BIT_WAIT1:
			btfss	RF_UART_RCIF			// test rcif
			goto 	BIT_WAIT1
                        nop

			movff  RF_RCREG, rf_thisbyte	// read RCREG and put in rf_thisbyte

		adjust:								// switch off uart, wait for 9 mue
			bcf		RF_RCSTA, 4
			movlw	11
			movwf	rf_transition3
		loop:
			decfsz	rf_transition3
			goto loop
			bsf		RF_RCSTA, 4

		BIT_WAIT2:
			btfss	RF_UART_RCIF			// test rcif
			goto 	BIT_WAIT2
                        nop

			movff  RF_RCREG, rf_thisbyte	// read RCREG and put in rf_thisbyte

	//		btfsc	rf_thisbyte, 0			// test three bits of last byte for noiselevel
	//		incf	lbt_noiselevel
	//		btfsc	rf_thisbyte, 1
	/*		incf	lbt_noiselevel
			btfsc	rf_thisbyte, 2
			incf	lbt_noiselevel
	*/		btfsc	rf_thisbyte, 3
			incf	lbt_noiselevel
			btfsc	rf_thisbyte, 4
			incf	lbt_noiselevel
			btfsc	rf_thisbyte, 5
			incf	lbt_noiselevel
	/*		btfsc	rf_thisbyte, 6
			incf	lbt_noiselevel
			btfsc	rf_thisbyte, 7
			incf	lbt_noiselevel
	*/
			movlw	LBT_TIME_GOOK_LOOP						// decrement timeout 33 mue for one gook-loop
			subwf	rf_transition1
			btfss	PIC_STATUS, 0
			decf	rf_transition2
			btfss	PIC_STATUS, 0
			goto	time_out


			clrf	rf_pos					// check middle SIX bits for one, zero or something baaad.
			movf 	rf_thisbyte, w
			iorlw	0b10000001
			movwf	rf_thisbyte
			incfsnz	rf_thisbyte, w
			bsf		rf_pos, 0
			movlw	0b01111110
			andwf	rf_thisbyte
			tstfsz	rf_thisbyte
			bsf		rf_pos, 6
			btfsc	rf_pos, 0
			bcf		rf_pos, 6

		BIT_WAIT3:
			btfss	RF_UART_RCIF			// test rcif
			goto 	BIT_WAIT3
                        nop

			movff  RF_RCREG, rf_thisbyte	// read RCREG and put in rf_thisbyte

	//		btfsc	rf_thisbyte, 0			// test three bits of last byte for noiselevel
	//		incf	lbt_noiselevel
	//		btfsc	rf_thisbyte, 1
	/*		incf	lbt_noiselevel
			btfsc	rf_thisbyte, 2
			incf	lbt_noiselevel
	*/		btfsc	rf_thisbyte, 3
			incf	lbt_noiselevel
			btfsc	rf_thisbyte, 4
			incf	lbt_noiselevel
			btfsc	rf_thisbyte, 5
			incf	lbt_noiselevel
	/*		btfsc	rf_thisbyte, 6
			incf	lbt_noiselevel
			btfsc	rf_thisbyte, 7
			incf	lbt_noiselevel
	*/
			clrf	rf_buff					// check middle SIX bits for one, zero or something baaad.
			movf 	rf_thisbyte, w
			iorlw	0b10000001
			movwf	rf_thisbyte
			incfsnz	rf_thisbyte, w
			bsf		rf_buff, 0
			movlw	0b01111110
			andwf	rf_thisbyte
			tstfsz	rf_thisbyte
			bsf		rf_buff, 6
			btfsc	rf_buff, 0
			bcf		rf_buff, 6

			movf	rf_buff, w				// check if last two bytes make 'one' together
			addwf	rf_pos, 1
			decfsz	rf_pos, w
			goto 	gook

			movff  rf_buff, rf_pos			// save last byte in rf_pos

	check:
                        nop
		BIT_WAIT4:
			btfss	RF_UART_RCIF			// test rcif
			goto 	BIT_WAIT4
                        nop

			movff  RF_RCREG, rf_thisbyte	// read RCREG and put in rf_thisbyte

			clrf	rf_buff					// check middle SIX bits for one, zero or something baaad.
			movf 	rf_thisbyte, w
			iorlw	0b10000001
			movwf	rf_thisbyte
			incfsnz	rf_thisbyte, w
			bsf		rf_buff, 0
			movlw	0b01111110
			andwf	rf_thisbyte
			tstfsz	rf_thisbyte
			bsf		rf_buff, 6
			btfsc	rf_buff, 0
			bcf		rf_buff, 6

			movlw	LBT_TIME_CHECK_LOOP		// decrement timeout 8 mue for one check-loop
			subwf	rf_transition1
			btfss	PIC_STATUS, 0
			decf	rf_transition2
			btfss	PIC_STATUS, 0
			bsf		rf_timeout, 0

			movf	rf_buff, w				// check if last two bytes make 'one' together
			addwf	rf_pos
			decfsz	rf_pos
			goto 	gook_from_check

			movff  rf_buff, rf_pos			// save last byte in rf_pos

			decfsz	rf_transition0			// check if RF_PREAMBLE_MIN_RECOGNIZED times zero or one
			goto	check

	check_in:
                        nop
		BIT_WAIT5:
			btfss	RF_UART_RCIF			// test rcif
			goto 	BIT_WAIT5
                        nop

			movff  RF_RCREG, rf_thisbyte	// read RCREG and put in rf_thisbyte

			clrf	rf_buff					// check middle FOUR bits for one, zero or something baaad.
			movf 	rf_thisbyte, w
			iorlw	0b11000011
			movwf	rf_thisbyte
			incfsnz	rf_thisbyte, w
			bsf		rf_buff, 0
			movlw	0b00111100
			andwf	rf_thisbyte
			tstfsz	rf_thisbyte
			bsf		rf_buff, 6
			btfsc	rf_buff, 0
			bcf		rf_buff, 6
			#pragma endasm

		rf_shift = rf_shift << 1;			// queue bytes in rf_shift to match with syncsymbol

		if( rf_buff == 1 ) bit_set( rf_shift, 0 );
		else if( rf_buff == 0 ) bit_clear( rf_shift, 0 );
		else
		{
			--lbt_tolerance;				// if not, decrement RF_LBT_TOLERANCE
			if( !lbt_tolerance ) goto noise;
		}

		rf_shift = rf_shift & 0b00111111;

		rf_thisbyte = RFUartGetByte();

		#pragma asm
		movlw	LBT_TIME_CHECK_IN_LOOP							// decrement timeout 24 mue for one check_in-loop
		subwf	rf_transition1
		btfss	PIC_STATUS, 0
		decf	rf_transition2
		btfss	PIC_STATUS, 0
		bsf		rf_timeout, 0
		#pragma endasm

		rf_pos += rf_buff;					// check if last two bytes make 'one' together

		if( rf_pos != 1 )
		{
			--lbt_tolerance;				// if not, decrement RF_LBT_TOLERANCE
			if( !lbt_tolerance ) goto noise;
		}

		rf_pos = rf_buff;

		rf_thisbyte = RFUartGetByte();

		if( rf_shift != RF_SYMBOL_SYNCRECEIVED ) goto check_in;		// check for syncsymbol

		while((( rf_thisbyte = RFUartGetByte()) | 0b11000011 ) != 255 );	// wait for first 'one' after syncsymbol, to determine the point at which timer1 is set.

		set_timer1( LBT_FSM_TIME_SYNC_RECEIVE );
		lbt_time_set = TRUE;

		return LBT_RECEIVE;

	noise:
		++noiselevel;

	time_out:

/*		output_low( PIN_CONN_18 );		//debug
		output_high( PIN_CONN_18 );
		output_low( PIN_CONN_18 );
		output_high( PIN_CONN_18 );
		output_low( PIN_CONN_18 );
		output_high( PIN_CONN_18 );
		output_low( PIN_CONN_18 );
		output_high( PIN_CONN_18 );
		output_low( PIN_CONN_18 );
*/
		if( noiselevel || ( lbt_noiselevel > LBT_NOISELEVEL ))
		{
/*			output_high( PIN_CONN_18 );		//debug
			output_low( PIN_CONN_18 );
			output_high( PIN_CONN_18 );
			output_low( PIN_CONN_18 );
			output_high( PIN_CONN_18 );
			output_low( PIN_CONN_18 );
*/			return LBT_NOISE;
		}
		else return LBT_FREE;
}




#pragma inline
void RFUartStartContinuous()
{
	#pragma asm
		bcf		RF_UART_SREN		// clear SREN = disable single receive
		bcf		RF_UART_CREN		// clear CREN = disable continuous receive
		bsf		RF_UART_CREN		// set	CREN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	#pragma endasm

}



#pragma inline
void RFUartClearFifo()
{
	#pragma asm
		btfsc	RF_UART_RCIF			// skip next (readout) if RCIF is clear (=no data there)
		movf	RF_RCREG,W
		btfsc	RF_UART_RCIF			// two times
		movf	RF_RCREG,W
	#pragma endasm
}

#pragma inline
int RFUartGetByte()
{
	int i;
	#pragma asm
		BIT_WAIT:
		btfss	RF_UART_RCIF			// test rcif
		goto 	BIT_WAIT
	#pragma endasm
	return RF_RCREG;


}





// turn uart immideately off (a byte being received is killed!)
#pragma inline
void RFUartOff()
{
	#pragma asm
		bcf	RF_UART_SPEN				//clear SPEN
	#pragma endasm
}
#pragma inline
void RFUartOn()
{
	#pragma asm
		bsf	RF_UART_SPEN				//set SPEN
	#pragma endasm
}


// turns spi immideately off!! (a byte being transmitted is killed!), the pin must then be set
#pragma inline
void RFSpiOff()
{
	output_low(PIN_RF_TX);
	#pragma asm
		bcf	RF_SSPCON_SSPEN			// clear SSPEN
	#pragma endasm

}

#pragma inline
void RFSpiOn()
{

int i;
	#pragma asm

			bsf	RF_SSPCON_SSPEN				// set SSPEN
			//MOVLW	0						// load 0, init the rf_shift reg with zero (otherwise tx line is high for long time)
			//MOVWF	RF_SSPBUF				// Akku -> SSBUF

	#pragma endasm


}


#pragma inline
void RFWaitForTimer1Exact(int timeout)
{
	//use global rf_buff as buffer, rf_timeout
		rf_timeout=timeout;
		RFDeleteTimer1Overflow();
		if (rf_timeout>RF_FSM_TIME_EARLY_SLOT_END) rf_timeout=RF_FSM_TIME_EARLY_SLOT_END;	// for stable reason (otherwise the break condition may not occur)
		while (RF_TIMER1H<rf_timeout);
		while (!bit_test(RF_TIMER1L,3)) ;

			rf_buff=RF_TIMER1L;			// read Timer 2
		#pragma asm

			BTFSS	rf_buff,0			// if buff,0 ==0 then wait 2
			goto    weiter1
			goto    weiter2
		weiter1:					// buff,0 was 0 0> wait
			nop
			nop						// one extra wait
		weiter2:
			BTFSS	rf_buff,1
			goto weiter3
			goto weiter4
		weiter3:
			nop
			nop						//two extra wait
			nop
		weiter4:
			BTFSS  rf_buff,2
			goto weiter5
			goto weiter6
		weiter5:
			nop
			nop						// four extra wait
			nop
			nop
			nop
		weiter6:
			nop

		#pragma endasm
		set_timer2(0);
}



// runtime of function ends exactly before TIMER1_H = timeout
#pragma inline
void RFWaitForTimer1ExactNew(int timeout)
{
	//use global rf_buff as buffer, rf_timeout
		rf_timeout = timeout-1;
		RFDeleteTimer1Overflow();
		if( rf_timeout > RF_FSM_TIME_EARLY_SLOT_END ) rf_timeout = RF_FSM_TIME_EARLY_SLOT_END;	// for stable reason (otherwise the break condition may not occur)
		if( RF_TIMER1H >= rf_timeout ) return;
		while( RF_TIMER1H < rf_timeout );

		#pragma asm
	seven:
		btfss	RF_TIMER1L, 7
		goto	seven
	six:
		btfss	RF_TIMER1L, 6
		goto	six
	five:
		btfss	RF_TIMER1L, 5
		goto	five
	four:
		btfss	RF_TIMER1L, 4	// 1st possibility: 11110000, 2nd possibility: 11110001, 3rd possibility: 11110010
		goto	four			// + 2 cycles

		movf	RF_TIMER1L, w	// 1st poss: 11110011, 2nd poss: 11110100, 3rd poss: 11110101
								// 1st poss: 12 cyc to go, 2nd poss: 11 cyc to go, 3rd poss: 10 cyc to go
		movwf	rf_buff			// + 1 cycle

		btfss	rf_buff, 2		// + 1 cycle for all
		goto 	first			// + 2 cycles for all
								// = 4 cycles gone for all possibilities
		btfss	rf_buff, 0		// + 1 cycle for 2nd and 3rd poss
		goto 	second			// + 2 cycles for 2nd and 3rd poss
								// = 3 cycles gone for 2nd and 3rd poss
		goto	third			// + 2 cycles for 3rd poss
								// = 2 cycles gone for 3rd poss
								// ---------------------------------------
								// = 4 cycles gone for 1st poss -> 12 - 4 = 8 to go
								// = 7 cycles gone for 2nd poss -> 11 - 7 = 4 to go
								// = 9 cycles gone for 3rd poss -> 10 - 9 = 1 to go
	first:
		nop
		nop
		nop
		nop


	second:
		nop
		nop
		nop

	third:
		nop
/*		tstfsz	RF_TIMER1L
		bsf		PIC_PORTD, 4
		bcf		PIC_PORTD, 4
		movf	RF_TIMER1H, w
		subwf	200, w
		btfss	PIC_STATUS, 2
		bsf		PIC_PORTD, 4
		bcf		PIC_PORTD, 4
*/		#pragma endasm
}


#pragma inline
void RFWaitForNextBitTimeExact()
{

	// use global rf_buff



		#pragma asm
			bcf		RF_TIMER2_TMR2IF
		SYNC_L1:
			btfss	RF_TIMER2_TMR2IF		// test tmr21f overflow
			goto 	SYNC_L1

		#pragma endasm
			rf_buff=RF_TIMER2;			// read Timer 2
		#pragma asm

			BTFSS	rf_buff,0			// if rf_buff,0 ==0 then wait 2
			goto    weiter1
			goto    weiter2
		weiter1:					// buff,0 was 0 0> wait
			nop
			nop						// one extra wait
		weiter2:
			BTFSS	rf_buff,1
			goto weiter3
			goto weiter4
		weiter3:
			nop
			nop						//two extra wait
			nop
		weiter4:
			BTFSS  rf_buff,2
			goto weiter5
			goto weiter6
		weiter5:
			nop
			nop						// four extra wait
			nop
			nop
			nop
		weiter6:
			nop

		#pragma endasm


}




void RFInitRandom()
{
	UInt8 i;
	rf_random_h = 0;
	for (i=0;i<8;i++)
	{
		rf_random_h = rf_random_h ^ ReadIntEeprom(i);
	}
	if (rf_random_h==0) rf_random_h = MY_MAC_ID;

	rf_random_8=rf_random_h;						// init the 8bit random function as well

}



//sends a packet (preamble, syncsymbol & packet)
int LBTSend()
{
	unsigned int i;



		i = LBT_PREAMBLE_LENGTH - 1;
		rf_tx = RF_SYMBOL_FASTTRAILER;
		RFSpiSendByte();
		while (--i)
		{
			RFSpiWait();
			RFSpiSendByte();
		}

		rf_tx=LBT_SYMBOL_SYNC1;RFSpiWait();RFSpiSendByte();
		rf_tx=LBT_SYMBOL_SYNC2;RFSpiWait();RFSpiSendByte();
		rf_tx=LBT_SYMBOL_SYNC3;RFSpiWait();RFSpiSendByte();

		rf_tx=RF_SYMBOL_FASTTRAILER;
		RFSpiWait();
		RFSpiSendByte();	// now you have 64us time to start the data transmission

		set_timer1( LBT_FSM_TIME_SYNC_SEND );
		lbt_time_set = TRUE;

		rf_tx=RF_SYMBOL_FASTTRAILER;
		RFSpiWait();
		RFSpiSendByte();	// now you have 64us time to start the data transmission

		// SDJS: if sdjs start type found in sendbuffer and sdjs is switched on sdjs_start is set.
		if (( LL_payload_send[0]==ACL_TYPE_SJS_HI) && (LL_payload_send[1]==ACL_TYPE_SJS_LO) && sdjs_type ) sdjs_start = 1;
		else sdjs_start = 0;

		// send RF Header
		rf_tx=RF_SYMBOL_SOP;RFSpiWait();RFSpiSendByte();					// Send SOP		(PHY Header)
		if (bit_test(rf_random_l,2)) rf_tx=23; else rf_tx=116;				// if swcrambler was 0 ´change it to none zero
		RFSpiWait();RFSpiSendByte();										// Send Scrambler init (PHY Header)
		rf_transition0=rf_tx;												// init RFScrambler with  rf_transition0 is the RFScrambler rf_shift reg
		rf_tx=LL_payload_length+LL_HEADER_SIZE+LL_TAIL_SIZE;RFSpiWait();RFSpiSendByte();					// Send LL payload length



		// this is   for acl subscriptions
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();
			rf_tx=rf_transition0^LL_payload_send[0];RFSpiWait();RFSpiSendByte();	//artefact high
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();
			rf_tx=rf_transition0^LL_payload_send[1];RFSpiWait();RFSpiSendByte();	//artefact low

		// give 4 bytes time for subscription chekc in receiver
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();
			rf_tx=rf_transition0^0b10101010;RFSpiWait();RFSpiSendByte();	//give receiver time
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();
			rf_tx=rf_transition0^0b10101010;RFSpiWait();RFSpiSendByte();	//give receiver time
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();
			rf_tx=rf_transition0^0b10101010;RFSpiWait();RFSpiSendByte();	//give receiver time
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();
			rf_tx=rf_transition0^0b10101010;RFSpiWait();RFSpiSendByte();	//give receiver time


		// SDJS: if sdjs_start is set sdjs is started here.
		if ( sdjs_start )
		{
			SDJSRF();
			rf_status|=RF_STATUS_JUST_SENT;

			return 1;
		}


		// send LL_header
		for (rf_shift=0;rf_shift<=11;rf_shift++)
		{
			for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();					// next step in the RFScrambler

			rf_tx=rf_transition0^LL_header_send[rf_shift];					// scramble header data with scrambler
			RFSpiWait();
			RFSpiSendByte();
		}

		//send ACL_Data;
		rf_limit=LL_payload_length;
		for (rf_shift=0;rf_shift<rf_limit;rf_shift++)
		{
			for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();					// next step in the RFScrambler

			rf_tx=rf_transition0^LL_payload_send[rf_shift];					// scramble header data with scrambler
			RFSpiWait();
			RFSpiSendByte();
		}


		//send LL_tail1
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();					// next step in the RFScrambler
		rf_tx=rf_transition0^LL_tail_send[0];
		RFSpiWait();
		RFSpiSendByte();

		//send LL_tail2
		for (rf_pos=0;rf_pos<=7;rf_pos++) RFScramble();					// next step in the RFScrambler
		rf_tx=rf_transition0^LL_tail_send[1];
		RFSpiWait();
		RFSpiSendByte();

		// send eop
		rf_tx=RF_SYMBOL_EOP;
		RFSpiWait();RFSpiSendByte();

		// that's it
		RFSpiWait();


		rf_status|=RF_STATUS_JUST_SENT;
		// this is debug
		/*
		for (rf_shift=0;rf_shift<LL_HEADER_SIZE;rf_shift++)
			DebugGiveOut(LL_header_send[rf_shift]);
		for (rf_shift=0;rf_shift<rf_limit;rf_shift++)
			DebugGiveOut(LL_payload_send[rf_shift]);
		for (rf_shift=0;rf_shift<LL_TAIL_SIZE;rf_shift++)
			DebugGiveOut(LL_tail_send[rf_shift]);

		*/


		return 1;
}



// returns result in rf_pos
#pragma separate
void RFCheckSubscriptions()
{
	//search through subscriptions for early shutdown
	for (rf_shift=0;rf_shift<ACL_SUBSCRIPTIONLIST_LENGTH;rf_shift++)
	{
		RFUartGetByte();	//trash one bit
		RFScramble();

		if (rf_transition2==ACL_subscriptions[rf_shift][0])
		{
			RFUartGetByte();	//trash 2nd bit
			RFScramble();

			if (rf_transition3==ACL_subscriptions[rf_shift][1]) rf_pos=1;
		}
		else
		{
			RFUartGetByte();	//trash 2nd bit
			RFScramble();
		}

	}
	for (rf_shift=0;rf_shift<(32-(ACL_SUBSCRIPTIONLIST_LENGTH*2));rf_shift++)
	{
		RFUartGetByte();
		RFScramble();	//trash the rest of the time

		// SDJS: if sdjs start type is found in receivebuffer and sdjs is switched on sdjs_start is set.
		if (( rf_transition2==ACL_TYPE_SJS_HI) && (rf_transition3==ACL_TYPE_SJS_LO) && sdjs_type )
		{
				sdjs_start = TRUE;
				rf_pos=1;
		}
	}
	// till here check subscription
}




int RFReceiveData()
{
	int i,ll_receive_len;
	long crc16;


	RFUartClearFifo();
	rf_buff=RFUartGetByte();				//trash one bit

	rf_rx1=0;
	rf_shift=40; 		// 40 bit timeout
	// receive SOP Symbol in rf_rx1
		while ((rf_shift) && (rf_rx1!=RF_SYMBOL_SOP))
	{
		rf_rx1<<=1;
		rf_buff=RFUartGetByte();
		if ((rf_buff|0b11100111)==255) bit_set(rf_rx1,0);else bit_clear(rf_rx1,0);
		rf_shift--;

	}

	if (rf_rx1!=RF_SYMBOL_SOP) return 0;

	// receive Scrambler init byte
	RFReceiveByte();
	rf_transition0=rf_rx1;	// init RFScrambler, rf_transition0 is rf_shiftreg of RFScrambler


	//receive the len of the ll packet
	RFReceiveByte();
	ll_receive_len=rf_rx1;


	//check the subscrisptions, trash 16 bit during process
	RFReceiveScrambledByte();		// get ARtefact high
	rf_transition2=rf_rx1;
	RFReceiveScrambledByte();		// get artefact low
	rf_transition3=rf_rx1;
	rf_pos=0;						// reuse for result of subscriptions

	// SDJS: clear sdjs_start as it is set if SDJS start type is found in RFCheckSubscriptions().
	sdjs_start = 0;
	RFCheckSubscriptions();  		// result is in rf_pos


	if (ACL_subscribe_all) rf_pos=1;		// if all subscribe then set the "found" state





	if (!rf_pos)
	{
		// no subscription matched the types
		//DebugGiveOut(0b11000011);
		return 0;		//early shut off
	}




	// receive the LL header
	for (rf_shift=0;rf_shift<LL_HEADER_SIZE;rf_shift++)
	{
		RFReceiveScrambledByte();										// gets one byte and stores it in rx1
		LL_header_receivebuffer[rf_shift]=rf_rx1;					// copy received byte into receive buffer
	}






	// receive the LL payload
	rf_limit=ll_receive_len-LL_HEADER_SIZE-LL_TAIL_SIZE;
	if (rf_limit>LL_PAYLOAD_SIZE) return 0;					// length is error

	// SDJS: if sdjs_start is set sdjs is started.
	if ( sdjs_start == 1 )
	{
		SDJSRF();
		rf_limit=64;
		return 0;
	}

	for (rf_shift=0;rf_shift<rf_limit;rf_shift++)
	{
		RFReceiveScrambledByte();		// gets one byte and stores it in rx1
		LL_payload_receivebuffer[rf_shift]=rf_rx1;					// copy received byte into receive buffer
	}


	// receive the LL tail
	for (rf_shift=0;rf_shift<LL_TAIL_SIZE;rf_shift++)
	{
		RFReceiveScrambledByte();		// gets one byte and stores it in rx1
		LL_tail_receivebuffer[rf_shift]=rf_rx1;					// copy received byte into receive buffer
	}


	// check the CRC
	crc16=LLCalcCRC16(LL_header_receivebuffer,LL_payload_receivebuffer,rf_limit);


	rf_shift=LL_tail_receivebuffer[0]^(crc16>>8);	//CRCh (implizit typecast; ´takes low byte from long
 	rf_shift|=LL_tail_receivebuffer[1]^crc16;		//CRCl
	if (rf_shift)		// if anything is unlike zero, then crc error was there
	{
		//
		//DebugGiveOut(255);
		//DebugGiveOut(rf_transition2);DebugGiveOut(rf_transition3);	// the artefact type

		//
		//PCLedBlueOn();
		LL_last_data=LL_CRC_ERROR;
		return 0;

	}



	// here the coplete packet is there, crc is ok, subscription is positive now process the packet
	// transition2,transition3 still holds the artefact type, rf_pos holds the status control or not


	if ((LL_payload_receivebuffer[0]==ACL_TYPE_ACM_H) && (LL_payload_receivebuffer[1]==ACL_TYPE_ACM_L) && (ACL_ignore_control_messages==false))
	{
		LL_last_data=LL_ACM;
		ACLProcessControlMessages();	// control message verarbeiten
	}

	if ((LL_payload_receivebuffer[0]!=ACL_TYPE_ACM_H) || (LL_payload_receivebuffer[1]!=ACL_TYPE_ACM_L) || (ACL_pass_control_messages==true))
	{
		LL_last_data=LL_GOOD;
		// if there was not a control msg, but subscription was good anyhow, or pass flag is set, then copy to received
		if (!LLReceiveBufferLocked())
		{
			//copy the header
			for (rf_shift=0;rf_shift<LL_HEADER_SIZE;rf_shift++)
				LL_header_received[rf_shift]=LL_header_receivebuffer[rf_shift];
			//copy the payload
			for (rf_shift=0;rf_shift<rf_limit;rf_shift++)
				LL_payload_received[rf_shift]=LL_payload_receivebuffer[rf_shift];
			//copy the tail
			for (rf_shift=0;rf_shift<LL_TAIL_SIZE;rf_shift++)
				LL_tail_received[rf_shift]=LL_tail_receivebuffer[rf_shift];
			LLSetDataToNew();		//indicate that new data packet has arrived
			LL_received_packet_in_this_slot=true;		//set the trigger that a packet is there
		}
	}

return 1;
}


// receives byte (must be synchronized and descrambles it
//#pragma inline
void RFReceiveScrambledByte()
{
		rf_rx1=0;
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,7);	// RFScramble the bit and set it into
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,6);	// RFScramble the bit and set it into
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,5);	// RFScramble the bit and set it into
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,4);	// RFScramble the bit and set it into
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,3);	// RFScramble the bit and set it into
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,2);	// RFScramble the bit and set it into
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,1);	// RFScramble the bit and set it into
		rf_buff=RFUartGetByte();RFScramble();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		rf_buff=rf_buff^rf_transition0;if (bit_test(rf_buff,0)) bit_set(rf_rx1,0);	// RFScramble the bit and set it into
}


void RFReceiveByte()
{
		rf_rx1=0;
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,7);
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,6);
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,5);
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,4);
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,3);
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,2);
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,1);
		rf_buff=RFUartGetByte();if ((rf_buff|0b11100111)==255) rf_buff=1;else rf_buff=0;
		if (bit_test(rf_buff,0)) bit_set(rf_rx1,0);
}




// reuse of rf_transition1,
#pragma inline
void RFScramble()
{

	//rf_transition0 is the rf_shift register of the RFScrambler
	rf_transition1=0;
	if (bit_test(rf_transition0,3)) rf_transition1++;
	if (bit_test(rf_transition0,6)) rf_transition1++;
	rf_transition0<<=1;
	if (bit_test(rf_transition1,0)) bit_set(rf_transition0,0);

	//debug
	//rf_transition0=0;	// no scrambling


}



// set a one in the rf_pos 1 with prob. 0.5, in the next slot with 0.25 then with 0.125 and so on
int RFCreateAliveSymbol()
{


	/*
	if (!bit_test(rf_random_l,0)) return 0b00000001;
	if (!bit_test(rf_random_l,1)) return 0b00000011;
	if (!bit_test(rf_random_l,2)) return 0b00000111;
	if (!bit_test(rf_random_l,3)) return 0b00001111;
	if (!bit_test(rf_random_l,4)) return 0b00011111;
	if (!bit_test(rf_random_l,5)) return 0b00111111;
	if (!bit_test(rf_random_l,6)) return 0b01111111;
	if (!bit_test(rf_random_l,7)) return 0b11111111;
	return 0b11111111;

	*/

	if (!bit_test(rf_random_8,0)) return 0b10000000;
	if (!bit_test(rf_random_8,1)) return 0b11000000;
	if (!bit_test(rf_random_8,2)) return 0b11100000;
	if (!bit_test(rf_random_8,3)) return 0b11110000;
	if (!bit_test(rf_random_8,4)) return 0b11111000;
	if (!bit_test(rf_random_8,5)) return 0b11111100;
	if (!bit_test(rf_random_8,6)) return 0b11111110;
	if (!bit_test(rf_random_8,7)) return 0b11111111;
	return 0b11111111;


}


//reuse of rf_pos

void RFNewRandom()
{


	// first the 16bit randomfunction
	rf_pos=0;rf_shift=0;
	if (bit_test(rf_random_l,3)) rf_pos=1;
	if (bit_test(rf_random_h,4)) rf_pos^=1;
	if (bit_test(rf_random_h,6)) rf_shift=1;
	if (bit_test(rf_random_h,7)) rf_shift^=1;
	rf_pos=rf_pos^rf_shift;

	// this makes a 16 bit shift left with the random_l& h registers (they might not be next to each other)
	rf_shift=0;
	if (bit_test(rf_random_l,7)) rf_shift=1;	// save carry bit
	rf_random_l<<=1;							//shift one left
	rf_random_h<<=1;							//shift one left
	if (rf_shift==1) bit_set(rf_random_h,0);	// set carry bit
	// this was 16 bit shift
	if(rf_pos) bit_set(rf_random_l,0);



	// now the 8 bit random funciotn
	/*
	#pragma asm
		RLF     rf_random_8,W
		RLF     rf_random_8,W
		BTFSC   rf_random_8,4
		XORLW   1
		BTFSC   rf_random_8,5
		XORLW   1
		BTFSC   rf_random_8,3
		XORLW   1
		MOVWF   rf_random_8
	#pragma endasm
		rf_random_8 += MY_MAC_ID;				// -> no identical sequence random numbers on any two boards
	*/

	//reuse of rf_pos;
	rf_pos=0;
	/*
	if (bit_test(rf_random_8,2)) rf_pos++;
	if (bit_test(rf_random_8,4)) rf_pos++;
	if (bit_test(rf_random_8,5)) rf_pos++;
	if (bit_test(rf_random_8,7)) rf_pos++;

	rf_random_8=rf_random_8<<1;
	if (bit_test(rf_pos,0)) bit_set(rf_random_8,0); else bit_clear(rf_random_8,0);	//
	*/


		// This calculates parity on the selected bits (mask = 0xb4).
		if(rf_random_8 & 0x80)
			rf_pos = 1;

		if(rf_random_8 & 0x20)
			rf_pos ^= 1;

		if(rf_random_8 & 0x10)
			rf_pos ^= 1;

		if(rf_random_8 & 0x04)
			rf_pos ^= 1;

		rf_random_8 <<= 1;

		rf_random_8 |= rf_pos;

}


void RFStop()
{
		disable_interrupts(INT_TIMER1);
		rf_status=rf_status&(~RF_STATUS_LAYER_ON);
		if (rf_status & RF_STATUS_SENDING)
		{
			rf_status&= (~RF_STATUS_SENDING);
			rf_status&= (~RF_STATUS_SEND_SUCCESS);
			rf_status&= (~RF_STATUS_INSERTED_CTRLMSG);
		}
		PCLedRedOff();
		PCLedBlueOff();
		RFSetModeSleep();									// turn off rf module

}



void RFDelaySlots(int slots)
{

	long target;

	target =rf_slotcounter+slots;

	//if (rf_status & RF_STATUS_LAYER_ON)
		while (rf_slotcounter!=target) ;	// block until time is reached
}


//return the remaining time until next rf slot
long RFMicroSecondsUntilNextSlot()
{
	long value;
	value=(long)(255-PIC_TMR1H)*51;
	return value;
}
// returns true, when more than 100 us are left
int RF100usLeft()
{
	if (RF_TIMER1H<253) return 1;
	return 0;
}
// returns true, when more than 200 us are left
int RF200usLeft()
{
	if (RF_TIMER1H<251) return 1;
	return 0;
}

// returns true, when more than 500 us are left
int RF500usLeft()
{
	if (RF_TIMER1H<245) return 1;
	return 0;
}


// Calculates arbitrary waiting time within a range determined by LBT_WAITING_TIME_CONSTANT
#pragma inline
long LBTGetWaitingTime()
{
	unsigned long waiting_time;
	unsigned long waiting_time_sum;

		waiting_time = (long)rf_random_h << 8;
		waiting_time += (long)rf_random_l;

		waiting_time_sum = 0;

		waiting_time >>= 3;

		waiting_time_sum = waiting_time + waiting_time + waiting_time + waiting_time + waiting_time;

		return waiting_time_sum + LBT_WAITING_TIME_CONSTANT;
}




// here comes the Link Layer
//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------
//------------------------------LL-------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------





int LLGetIDFromHardware()
{
	int i,buff,result;

	//return;
	//FlashInit();
	//RFPowerModuleOn();
	//RFSetModeSleep();

	//check if valid ID in Eeprom:

	// the new stuff: get the id from the 1-wire


	buff=PCGetID(&LL_header_send[3]);
	return buff;


	/* storage of id is discontinued at the moment...
	//

	result=0;
	for (i=0;i<=7;i++)
	{
		buff=ReadIntEeprom(EEPROM_ADDRESS_ID+i);
		LL_header_send[3+i]=buff;
		if (buff==255) result++;
	}
	if (result<8)
	{
		//ID in internal EEProm was valid!
		//now write it to external eeprom and exit


		FlashWriteSequence(FLASH_ID_ADDRESS, &LL_header_send[3],8);
		FlashFlush();
		return 1;
	}


	// id in internal EEPROM was invalid; try external eeprom

		FlashReadSequence(FLASH_ID_ADDRESS,&LL_header_send[3],8);
		result=0;
		for(i=0;i<=7;i++)
		{
			buff=LL_header_send[3+i];
			if (buff==255) result++;
		}
		if (result<8)
		{
			//external was ok, write it to internal
			for(i=0;i<=7;i++)
			{
				WriteIntEeprom(EEPROM_ADDRESS_ID+i,LL_header_send[i+3]);
			}
			return 1;
		}


	return 0;		//both were invalid: there's no valid id
	*/
}






void LLInit()
{
	int i;
	LL_sequence_no=0;


	//prepare standard header with Ids



	LL_header_send[0]=VERSION;



	/*
	LL_header_send[3]=ReadIntEeprom(EEPROM_ADDRESS_ID+0);
	LL_header_send[4]=ReadIntEeprom(EEPROM_ADDRESS_ID+1);
	LL_header_send[5]=ReadIntEeprom(EEPROM_ADDRESS_ID+2);
	LL_header_send[6]=ReadIntEeprom(EEPROM_ADDRESS_ID+3);
	LL_header_send[7]=ReadIntEeprom(EEPROM_ADDRESS_ID+4);
	LL_header_send[8]=ReadIntEeprom(EEPROM_ADDRESS_ID+5);
	LL_header_send[9]=ReadIntEeprom(EEPROM_ADDRESS_ID+6);
	LL_header_send[10]=ReadIntEeprom(EEPROM_ADDRESS_ID+7);
	*/


	LL_payload_length=0;	// set used ll payload to zero
	LL_receive_buffer_locked=false;
	LL_received_packet_in_this_slot=false;
	LL_last_data=LL_NONE;

	RFInit();

	LLGetIDFromHardware();	//take id from internal or external eeprom and set ll_header_send
}




void LLStart()
{
	RFStart();
}


void LLStop()
{
	RFStop();	// handles the states
}



void LLSlotEnd()
{
	// check if timeout for waiting packet
	if (LLSendingBusy())
	{
		LL_slots_left--;
		if (LL_slots_left==0)
		{
			rf_status&=~RF_STATUS_SENDING;					// busy is over

			if (rf_status & RF_STATUS_INSERTED_CTRLMSG)
			{
				rf_status&=~RF_STATUS_INSERTED_CTRLMSG;		// in case it was switched on, switch it off and do NOT update the success state
			}
			else
			{
				rf_status&=~RF_STATUS_SEND_SUCCESS;
			}

			LL_payload_length=0;							// set used payload to zero
		}
	}

	// if msg received in last slot set appropr. bit

}



void LLAbortSending()
{
	if (LLSendingBusy())
	{
		if (rf_status & RF_STATUS_INSERTED_CTRLMSG)
		{
			rf_status&=~RF_STATUS_INSERTED_CTRLMSG;
			rf_status&=~RF_STATUS_SENDING;
		}
		else
		{
			rf_status&=~RF_STATUS_SEND_SUCCESS;
			rf_status&=~RF_STATUS_SENDING;
		}
		LL_payload_length=0;
	}
}


int LLSetSendingSuccess()
{
	//update the states
	if (rf_status & RF_STATUS_INSERTED_CTRLMSG)
	{
		// no update of success flag
		rf_status&= ~RF_STATUS_INSERTED_CTRLMSG;	// clear the control management bit
		rf_status&= ~RF_STATUS_SENDING;				// busy is over
		LL_payload_length=0;						// set used payload to zero
	}
	else
	{
		rf_status|= RF_STATUS_SEND_SUCCESS;			// succesfull sent
		rf_status&= ~RF_STATUS_SENDING;				// busy is over
		LL_payload_length=0;						// set used payload to zero
	}
}



int LLSendingBusy()
{
	if (rf_status & RF_STATUS_SENDING) return 1;
	return 0;
}

// !! this returns the last(!) result, also if sending is active at the moment
int LLGetSendSuccess()
{
	if (rf_status & RF_STATUS_SEND_SUCCESS) return 1;
	else return 0;
}

int LLIsActive()
{
	if (rf_status& RF_STATUS_LAYER_ON) return 1;
	else return 0;
}


void LLLockReceiveBuffer()
{
	LL_receive_buffer_locked=true;
	// LLSetDataToOld(); @@TR: why??
}

int LLReceiveBufferLocked()
{
	if (LL_receive_buffer_locked==true) return 1;
	return 0;
}


void LLReleaseReceiveBuffer()
{
	LL_receive_buffer_locked=false;
	//LLSetDataToOld(); @@TR: why??
}


void LLSetDataToOld()
{
	rf_status&=~RF_STATUS_NEW_DATA;
}

void LLSetDataToNew()
{
	rf_status|=RF_STATUS_NEW_DATA;
}


int LLDataIsNew()
{
	if (rf_status & RF_STATUS_NEW_DATA) return 1;
	return 0;
}



void DebugBuildTestPacketACL()
{

	int i;
	// this is only testcode

	ACLAddNewType(ACL_TYPE_ACM_H,ACL_TYPE_ACM_L);
	ACLAddNewType(115,216);	//helo

	ACLAddData(255);
	ACLAddData(255);
	ACLAddData(255);
	ACLAddData(255);

	ACLAddData(255);
	ACLAddData(255);
	ACLAddData(255);
	ACLAddData(255);


	//ACLAddNewType(100,100);
	//for (i=0;i<32;i++)
	// ACLAddData(i);

}




int LLGetFieldStrength()
{
	return rf_field_strength;
}

void LLSetFieldStrength(int value)
{
	RFSetFieldStrength(value);
}


long LLCalcCRC16(byte *header_data,byte *payload_data,byte payload_size)
{
	//calcs a crc on LL_header, ACL_data, LL_tail
	int hb,lb,i,tmp;
	lb=0;
	hb=0;

	for(i=0;i<LL_HEADER_SIZE;i++)
	{
		tmp = lb;
		lb = hb;
		hb = tmp;
		lb ^= header_data[i];
		lb ^= lb >> 4;
		hb ^= lb << 4;
		hb ^= lb >> 3;
		lb ^= (lb << 4) << 1;
	}

	//len=header_data[1]-LL_HEADER_SIZE-LL_TAIL_SIZE;
	for(i=0;i<payload_size;i++)
	{
		tmp = lb;
		lb = hb;
		hb = tmp;
		lb ^= payload_data[i];
		lb ^= lb >> 4;
		hb ^= lb << 4;
		hb ^= lb >> 3;
		lb ^= (lb << 4) << 1;
	}


	return hb*256+lb;
}







// prepares the header and sends the packet out
int LLSendPacket(int slot_limit)
{
	//long crc16; // makes RAM too full if local var is active



	if (slot_limit==0) slot_limit=1;	// zero makes no sense

	if (!(rf_status & RF_STATUS_LAYER_ON)) return 0;
	if (rf_status & RF_STATUS_SENDING) return 0;


	// finalize the LLPacket

	//DebugGiveOut(LL_payload_length);


	LL_header_send[1]=LL_payload_length;
	LL_header_send[2]=rf_field_strength;
	LL_header_send[11]=LL_sequence_no;


	LL_longbuffer=LLCalcCRC16(LL_header_send,LL_payload_send, LL_payload_length);

	LL_tail_send[0]=(LL_longbuffer>>8);	//CRCh (implizit typecast; ´takes low byte from long
	LL_tail_send[1]=LL_longbuffer;		//CRCl

	LL_slots_left=slot_limit;
	LL_sequence_no++;

	rf_status|= RF_STATUS_SENDING;

	return 1;
}


int LLGetRemainingPayloadSpace()
{

	return (LL_PAYLOAD_SIZE-LL_payload_length);

}


int LLSentPacketInThisSlot()
{
	if (rf_status & RF_STATUS_JUST_SENT) return 1;
	return 0;
}


// here comes the "ACL"
//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------
//------------------------------LL-------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------




void ACLStartUp()
{
	int result[10];
	int selftest_result,i;

	selftest_result=AppSelfTest(result);		//runs the selftest


	//!!
	PCLedBlueOff();
	PCLedRedOff();
	DelayMs(100);				// for stabilizing of power etc.

	ACLInit();					// sets the pins in pic init correct (looks at selftest_active

	if(selftest_result==1)
	{
		for(i=0;i<10;i++)			//send result ten times to get it
		{
			//send packet
			ACLAddNewType(ACL_TYPE_CST_HI,ACL_TYPE_CST_LO);		//MST Self test
			ACLAddData(result[0]);
			ACLAddData(result[1]);
			ACLAddData(result[2]);
			ACLAddData(result[3]);
			ACLAddData(result[4]);
			ACLAddData(result[5]);
			ACLAddData(result[9]);

			ACLSendPacket(30);
			while(ACLSendingBusy()) {PCLedRedOn();PCLedRedOff();}

		}

		while(selftest_active)				//wait for selftest board to be removed
		{
			output_high(PIN_D7);
			if (!input(PIN_B7)) selftest_active=false;
			output_low(PIN_D7);
			if (input(PIN_B7)) selftest_active=false;

		}

		ACLInit();					//restart the whole stack once more
	}


}




void ACLInit()
{

	ACLStop();	//hold stack if already running

	PCLedRedOn();

	ACLFlushSubscriptions();
	ACL_send_buffer_locked=false;
 	ACL_write_position=0;
 	ACL_subscribe_all=false;
 	ACL_ACM_answers=true;
 	ACL_ignore_control_messages=false;
 	ACL_pass_control_messages=false;

	ACL_payload_length=0;
	ACL_write_position=0;

	OtapInit();			//reset all global variables for otap

	AppSetLEDBehaviour(LEDS_ON_TRAFFIC);			//setdefault


	PCLedRedOn();
	DelayMs(200);
	PCLedRedOff();
	DelayMs(200);


	LLInit();
	ACLStart();


}






int ACLSubscribe(byte LL_type_h,LL_type_l)
{
	int i;

	for (i=0;i<ACL_SUBSCRIPTIONLIST_LENGTH;i++)		// first test if already there
	{
		if ((ACL_subscriptions[i][0]==LL_type_h) && (ACL_subscriptions[i][1]==LL_type_l)) return 1;
	}

	for (i=0;i<ACL_SUBSCRIPTIONLIST_LENGTH;i++)
	{
		if ((ACL_subscriptions[i][0]==0) && (ACL_subscriptions[i][1]==0))
		{
			ACL_subscriptions[i][0]=LL_type_h;
			ACL_subscriptions[i][1]=LL_type_l;
			return 1;								// successful subscribed
		}
	}
	return 0;										// subsriptions failed
}

int ACLUnsubscribe(byte LL_type_h,LL_type_l)
{
	int i;

	for (i=0;i<ACL_SUBSCRIPTIONLIST_LENGTH;i++)		// if type is there, delete it
	{
		if ((ACL_subscriptions[i][0]==LL_type_h) && (ACL_subscriptions[i][1]==LL_type_l))
		{
			ACL_subscriptions[i][0]=0;
			ACL_subscriptions[i][1]=0;
			return 1;		// successufl unsubscribed
		}
	}
	return 0;		// not found
}

int ACLFlushSubscriptions()
{
	int i;

	for (i=0;i<ACL_SUBSCRIPTIONLIST_LENGTH;i++)
	{
		ACL_subscriptions[i][0]=0;
		ACL_subscriptions[i][1]=0;
	}

	ACL_subscribe_all=false;
	ACLSubscribeDefault();
	return;
}

void ACLSubscribeAll()
{
	ACL_subscribe_all=true;
}

void ACLAnswerOnACM()
{
 	ACL_ACM_answers=true;
}
void ACLNoAnswerOnACM()
{
	ACL_ACM_answers=false;
}






int ACLSubscribeDefault()
{
	int result;

	result=ACLSubscribe(ACL_TYPE_ACM_H,ACL_TYPE_ACM_L);		//165 14 is CMA (Control and Management virtual Artefact)

	return result;
}




int ACLVerifySubscription(int type_h,int type_l)
{
	int i;

	for (i=0;i<ACL_SUBSCRIPTIONLIST_LENGTH;i++)
	{
		if ((ACL_subscriptions[i][0]==type_h) && (ACL_subscriptions[i][1]==type_l)) return 1;
	}


	return 0;		//subscription not found
}





// retun one if there was a control msg // return not needed anymore
//#pragma inline
int ACLProcessControlMessages()
{



	if ((LL_payload_receivebuffer[0]==ACL_TYPE_ACM_H) && (LL_payload_receivebuffer[1]==ACL_TYPE_ACM_L))
	// there was a control msg
	{


 		// remote shutdown: NEEDS THE SOFTWARE-WATCHDOG-DRIVER FOR PIC (only exists for 18fxx20 at this time!!!!)
 		// WDT18FXX20 has to be included before this STACK-File
 		if ((LL_payload_receivebuffer[3]==ACL_TYPE_CRR_HI) && (LL_payload_receivebuffer[4]==ACL_TYPE_CRR_LO))	// check a control msg, e.g. "1,1"
 		{
 			#define LONGESTWDT 7	//this is the longest wait-time which is accepted from the Software-WDT (2.304sec)
 			unsigned int oldwdtvalue = 0;
 			unsigned int oldwdttest = 0;
 			unsigned int shutdownminutes = 0;
 			unsigned int shutdownseconds = 0;
 			unsigned long allseconds = 0;
 			unsigned long i = 0;
 			unsigned long timercycles = 0;

 			if (ACLMatchesMyIP(LL_payload_receivebuffer,6))
 			{
 				shutdownminutes=LL_payload_receivebuffer[14];
 				shutdownseconds=LL_payload_receivebuffer[15];

 				// check shutdown-time - if the given time is 0, the device is reseted
 				if ( (shutdownminutes==0) && (shutdownseconds==0) )
 				{
 					reset_cpu();
 				}

 				// check shutdown-time - if the given time is both 255, ShutDown the whole Device, only wakes up after hardware-reset
 				if ( (shutdownminutes==255) && (shutdownseconds==255) )
 				{
 					ACLStop();
 					sleep();
 				}

 				// Check ShutDown-Time - time in minutes in higher byte and time in seconds in lower byte
 				if ( ( (shutdownminutes!=0) || (shutdownseconds!=0) ) && ( (shutdownminutes!=255) || (shutdownseconds!=255) ) )
 				{
 					oldwdttest = WDTTest();
 					oldwdtvalue = WDTReadValue();

 					allseconds = shutdownseconds + ( 60*shutdownminutes );
 					timercycles = (allseconds>>1);	//Division by 2, because the longest WDT is 2.304s and you need an integral value

 					WDTConfig(LONGESTWDT);
 					WDTEnable();

 					PCLedBlueOff();
 					PCLedRedOff();

 					for (i=0;i<timercycles;i++)
 						{
 							sleep();
 							#pragma asm
 							 nop
 							 nop
 							 nop
 							#pragma endasm
 						}

 					WDTDisable();

 					WDTConfig(oldwdtvalue);

 					if (oldwdttest) {
 						WDTEnable();
 						}
 				}
 			}
 		}

 		// ControlFieldStrength:
 		// give the value to set the fieldstrength
 		if ((LL_payload_receivebuffer[3]==ACL_TYPE_CFS_HI) && (LL_payload_receivebuffer[4]==ACL_TYPE_CFS_LO))
 		{
 			if (ACLMatchesMyIP(LL_payload_receivebuffer,6))
 			{
 				int fsvalue;
 				fsvalue = LL_payload_receivebuffer[14];
 				ACLSetFieldStrength(fsvalue);
 			}

 		}



		// check syncrate setting
		if ((LL_payload_receivebuffer[3]==198) && (LL_payload_receivebuffer[4]==208))
		{
			RFSetSyncRate(LL_payload_receivebuffer[6]);

		}

		// check initial listen slots setting
		if ((LL_payload_receivebuffer[3]==159) && (LL_payload_receivebuffer[4]==192))
		{
			RFSetInitialListenSlots(LL_payload_receivebuffer[6]);
		}


		// check modeswitch
		if ((LL_payload_receivebuffer[3]==1) && (LL_payload_receivebuffer[4]==1))	// check a control msg, e.g. "1,1"
		{
		}


		//check over the air programming
		if ((LL_payload_receivebuffer[3]==198) && (LL_payload_receivebuffer[4]==88))	// check a control msg, e.g. "1,1"
		{
				OtapReceive();		// call over the airprogramming stuff
		}


		//check set ID
		if ((LL_payload_receivebuffer[3]==ACL_TYPE_CID_HI) && (LL_payload_receivebuffer[4]==ACL_TYPE_CID_LO) &&( selftest_active==true))
		{
 			OtapSetID();
  		}


		//check ledstyle
		if ((LL_payload_receivebuffer[3]==ACL_TYPE_CLE_HI) && (LL_payload_receivebuffer[4]==ACL_TYPE_CLE_LO))
		{
			if( ACLMatchesMyIP( LL_payload_receivebuffer, 12 ))
			{
				AppSetLEDBehaviour( LL_payload_receivebuffer[6] );
				if( LL_payload_receivebuffer[7] ) PCLedRedOn();
				else PCLedRedOff();
				if( LL_payload_receivebuffer[8] ) PCLedBlueOn();
				else PCLedBlueOff();
			}
		}


		//set production id
		#ifndef __PC210_C__
		if ((LL_payload_receivebuffer[3]==ACL_TYPE_CPI_HI) && (LL_payload_receivebuffer[4]==ACL_TYPE_CPI_LO))
		{
			if( ACLMatchesMyIP( LL_payload_receivebuffer, 11 ))
			{
				ACLStop();
				DelayMs( 20 );
				IDChipWriteProductionID( LL_payload_receivebuffer[6], LL_payload_receivebuffer[7] );
				ACLStart();
				DelayMs( 20 );
			}
		}
		#endif

		//////////////////////////////////////////
		if (ACL_ACM_answers==false) return 0;	// no answering allowed; all messages below here are not allowed, if acl_acm_answer is false





		//check Helo
		if ((LL_payload_receivebuffer[3]==115) && (LL_payload_receivebuffer[4]==216))
		{
			if (ACLMatchesMyIP(LL_payload_receivebuffer,6))		// does sent IP match my IP??
			{

				if (LLSendingBusy()) ACLAbortSending();			// interrupt any waiting stuff



				// now set the whole packet by hand:
				LL_payload_send[0]=ACL_TYPE_ACM_H;
				LL_payload_send[1]=ACL_TYPE_ACM_L;
				LL_payload_send[2]=0;

				LL_payload_send[3]=134;
				LL_payload_send[4]=32;

				LL_payload_send[5]=8;
				LL_payload_send[6]=LL_header_receivebuffer[3];		// put in orogin adress from HELO
				LL_payload_send[7]=LL_header_receivebuffer[4];
				LL_payload_send[8]=LL_header_receivebuffer[5];
				LL_payload_send[9]=LL_header_receivebuffer[6];
				LL_payload_send[10]=LL_header_receivebuffer[7];
				LL_payload_send[11]=LL_header_receivebuffer[8];
				LL_payload_send[12]=LL_header_receivebuffer[9];
				LL_payload_send[13]=LL_header_receivebuffer[10];
				LL_payload_length=14;								// give the correct len

				if (LLSendPacket(ACL_CONTROL_MESSAGES_TIMEOUT))		// NEVER USE ACL FUNCTION here!!!
				{
					rf_status|=RF_STATUS_INSERTED_CTRLMSG;			// this means that this msg if special concerning the update of the states
				}


			}

		}
		// this was helo



		return 1;											// there was a control management msg
	}
	return 0;												// there was no control management msg
}

void ACLSetFieldStrength(int power)
{
	LLSetFieldStrength(power);
}


int ACLSendingBusy()
{
	if (LLSendingBusy()) return 1;
	else	return 0;
}

int ACLGetSendSuccess()
{
	if (LLGetSendSuccess()) return 1;
	else return 0;
}






int ACLMatchesMyIP(char *buffer,int start)
{
	int res,i;
	res=0;

	// check if was my address		// ll_header_send [3..10] hold the myIP
	for (i=0;i<8;i++)
	 	if (buffer[start+i]==LL_header_send[3+i]) res++;
	if (res==8) return 1;

	res=0;

	// check if  was broadcast adress
	for (i=0;i<8;i++)
	 	if (buffer[start+i]==255) res++;
	if (res==8) return 1;

	return 0;
}





void ACLAbortSending()
{
	ACL_send_buffer_locked=false;
	ACL_write_position=0;
	ACL_payload_length=0;
	LLAbortSending();
}




// returns1 if packet was started
// returns 0 if still sending another or connection is off..
int ACLSendPacket(int slot_timeout)
{

	// check if sending busy or layer off

	if (!(rf_status & RF_STATUS_LAYER_ON)) return 0;
	if (rf_status & RF_STATUS_SENDING) return 0;

	// now copy the ACL to the LL payload
	// # copy
	memcpy(LL_payload_send,ACL_payload_send,LL_PAYLOAD_SIZE);	// copies the acl data to ll
	LL_payload_length=ACL_payload_length;
	if (LLSendPacket(slot_timeout))
	{
		// now ACL_payload_send is free again
		ACL_payload_length=0;
		ACL_write_position=0;
		return 1;
	}

	else return 0;

}

int ACLClearSendData()
{
	int i;
	if (LLSendingBusy()) return 0;					// dont clear if busy sending
	if (ACL_send_buffer_locked==true) return 0;		// dont clear if reservation is active

	//for(i=0;i<LL_PAYLOAD_SIZE;i++)
	//	ACL_payload_send[i]=0;

	memset(ACL_payload_send,0, LL_PAYLOAD_SIZE);	// clear buffer

	ACL_payload_length=0;							// no data in ACL buffer
	ACL_write_position=0;


	return 1;
}






// return 0 if ok
// returns 1 if full
// returns 2 if locked
int ACLAddNewType(int type_h, int type_l)
{
	if (ACL_send_buffer_locked==true) return 2;		// dont set if reservation is active
	if ((LL_PAYLOAD_SIZE-ACL_payload_length)<3)
	{
		return 1;		// not enough space
	}
	else
	{
		//place the new type
		ACL_payload_send[ACL_payload_length]=type_h;
		ACL_payload_send[ACL_payload_length+1]=type_l;
		ACL_payload_send[ACL_payload_length+2]=0;	//data length is zero
		ACL_write_position=ACL_payload_length+2;
		ACL_payload_length+=3;
		return 0;
	}
}

int ACLAddData(int data)
{
	if (ACL_send_buffer_locked==true) return 2;		// dont set if reservation is active
	if ((LL_PAYLOAD_SIZE-ACL_payload_length)<1)
	{
		return 1;		// not enough space
	}
	else
	{
		ACL_payload_send[ACL_write_position]++;			//count the acl tuple len up
		ACL_payload_send[ACL_payload_length]=data;		//place the data
		ACL_payload_length+=1;
		return 0;
	}

}

char *ACLGetPointer()
{
	return &ACL_payload_send[ACL_payload_length];		//place the data
}

int ACLSkip(int length)
{

	if (ACL_send_buffer_locked==true) return 2;		// dont set if reservation is active
	if ((LL_PAYLOAD_SIZE-ACL_payload_length)<length)
	{
		return 1;		// not enough space
	}
	else
	{
		ACL_payload_send[ACL_write_position]+=length;			//count the acl tuple len up
		ACL_payload_length+=length;
		return 0;
	}
}

int ACLGetRemainingPayloadSpace()
{
	//return (LL_payload_length-ACL_payload_length);
	return LL_PAYLOAD_SIZE-ACL_payload_length;
}






int ACLGetReceivedPayloadLength()
{
	return LL_header_received[1];
}




//looks for the length of context data of the given type, return2 -1 if type wasn't found
signed int ACLGetReceivedDataLength(int type_h, int type_l)
{
	int i;

	for (i=0;i<LL_header_received[1]-2;i+=3+LL_payload_received[i+2])
	{
		if ((LL_payload_received[i]==type_h) && (LL_payload_received[i+1]==type_l))
			return (LL_payload_received[i+2]);	//return valid length
	}

	return -1; // type not found 
}


char* ACLGetReceivedData(int type_h, int type_l)
{
	int i;

	for (i=0;i<=LL_header_received[1]-2;i+=(LL_payload_received[i+2]+3))
	{
		if ((LL_payload_received[i]==type_h) && (LL_payload_received[i+1]==type_l))
		{
				return (&(LL_payload_received[i+3]));								    

		}
	}
        return 0;

	//attention!! if type wasn't found: return is undetermined!!
}

char ACLGetReceivedByte(int type_h, int type_l, int position)
{
	int i;

	for (i=0;i<=LL_header_received[1]-2;i+=(LL_payload_received[i+2]+3))
	{
		if ((LL_payload_received[i]==type_h) && (LL_payload_received[i+1]==type_l))
		{
			if (LL_payload_received[i+2]!=0)
			{
				return (LL_payload_received[i+3+position]);									// if len is not zero: return valid pointer to data begin
			}

		}
	}

	//attention!! if type wasn't found: return is undetermined!!


}




int ACLFoundReceivedType(int type_h, int type_l)
{
	return (ACLGetReceivedDataLength(type_h,type_l)!=-1) ;	//found!
}



void ACLSetControlMessagesBehaviour(boolean ignore, boolean pass)
{
	ACL_ignore_control_messages=ignore;		//all control messages are ignored
	ACL_pass_control_messages=pass;			// all control messages are as well copied to receivebuffer for applicaiton
}



int ACLSentPacketInThisSlot()
{
	return (LLSentPacketInThisSlot());
}


void ACLStart()
{
	LLStart();
}

void ACLStop()
{
	LLStop();
}


void ACLLockSendBuffer()
{
  ACL_send_buffer_locked=true;
}

boolean ACLSendBufferLocked()
{
  return ACL_send_buffer_locked;
}


void ACLReleaseSendBuffer()
{
  ACL_send_buffer_locked=false;
}

void ACLLockReceiveBuffer()
{
	LLLockReceiveBuffer();
}

int ACLReceiveBufferLocked()
{
	return LLReceiveBufferLocked();
}


void ACLReleaseReceiveBuffer()
{
	LLReleaseReceiveBuffer();
}


void ACLSetDataToOld()
{
	LLSetDataToOld();
}

void ACLSetDataToNew()
{
	LLSetDataToNew();
}


int ACLDataIsNew()
{
	return LLDataIsNew();
}

int ACLDataReceivedInThisSlot()
{
	if (LL_received_packet_in_this_slot==true) return 1;
	return 0;
}

int ACLDataIsNewNow()
{
	return ACLDataReceivedInThisSlot();
}


//
int ACLAdressedDataIsNew()
{
	if (!ACLDataIsNew()) return 0;

	if (ACLFoundReceivedType(ACL_TYPE_CAD_HI,ACL_TYPE_CAD_LO))
	{
		if (ACLMatchesMyIP( ACLGetReceivedData( ACL_TYPE_CAD_HI, ACL_TYPE_CAD_LO), 0 )) return 1;
	}
	return 0;
}

int ACLAdressedDataIsNewNow()
{
	if (!ACLDataIsNewNow()) return 0;

	if (ACLFoundReceivedType(ACL_TYPE_CAD_HI,ACL_TYPE_CAD_LO))
	{
		if (ACLMatchesMyIP( ACLGetReceivedData( ACL_TYPE_CAD_HI, ACL_TYPE_CAD_LO), 0 )) return 1;
	}
	return 0;
}


//sends out the current packet with a target ID
int ACLSendPacketAdressed(unsigned int add1, add2, add3, add4, add5, add6, add7, add8, timeout )
{
	if (ACLGetRemainingPayloadSpace()<11) return 0;		//not enough space

	ACLAddNewType(ACL_TYPE_CAD_HI,ACL_TYPE_CAD_LO);
	ACLAddData(add1);
	ACLAddData(add2);
	ACLAddData(add3);
	ACLAddData(add4);
	ACLAddData(add5);
	ACLAddData(add6);
	ACLAddData(add7);
	ACLAddData(add8);

	if (ACLSendPacket(timeout)) return 1;
	return 0;
}


//****************************************************************************************************************
//*************              APP, SERVICES, REST
//****************************************************************************************************************







void AppSetLEDBehaviour(int ledstyle)
{
	rf_LED_style=ledstyle;
	if (ledstyle==LEDS_OFF)
	{
		PCLedBlueOff();
		PCLedRedOff();
	}

}


// this function is called at slot end and handles the leds
#pragma separate
void AppSetLEDs()
{
	if (rf_LED_style==LEDS_OFF)
	{
		//PCLedBlueOff();
		//PCLedRedOff();
	}
	if (rf_LED_style)										// if any other mode, then blinking led works at least
	{
		if (bit_test(rf_slotcounter,1)) PCLedRedOn(); else PCLedRedOff();				// this is for alife sign "blinking"
	}
	if (rf_LED_style==LEDS_NORMAL)
	{
		if (rf_sync_state==SYNC_STATE_SYNCED) PCLedBlueOn(); else PCLedBlueOff();		// this is for sync sign
	}
	if (rf_LED_style==LEDS_ON_RECEIVE)
	{
		if (LLDataIsNew()) PCLedBlueOn(); else PCLedBlueOff();		// this is for sync sign
	}
	if (rf_LED_style==LEDS_ON_SEND)
	{
		if (ACLSentPacketInThisSlot()) PCLedBlueOn(); else PCLedBlueOff();		// this is for sync sign
	}
	if (rf_LED_style==LEDS_ON_CRC_ERROR)
	{
		switch (LL_last_data)
		{
			case LL_GOOD:
				PCLedBlueOn();
				PCLedRedOff();
			break;
			case LL_ACM:
				PCLedBlueOn();
				PCLedRedOff();
			break;
			case LL_CRC_ERROR:
				PCLedBlueOff();
				PCLedRedOn();
			break;
			default:
				PCLedBlueOff();
				PCLedRedOff();
			break;
		}
 	}
	if (rf_LED_style==LEDS_ON_TRAFFIC)
	{
		if (( LL_received_packet_in_this_slot==true) || ACLSentPacketInThisSlot()) PCLedBlueOn(); else PCLedBlueOff();		// this is for sync sign
	}
}

void ACLWaitForReady()
{
	while (ACLSendingBusy()) ;
}


// SDJS: Runs sdjs when received a sdjs-starting type or sended one oneself. here only void function, actual function in sdjs.c.
#ifndef __SDJS_H__
void SDJSRF()
{}
#endif





