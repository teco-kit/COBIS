/* RemoteProcedureCall for SPart
* allows to address particular functions on the coreboard from the SPart
*
* 1st version by Albert Krohn (krohn@teco.edu)
* call this in the sensorboard for remote procedure call
* wait_time gives the timeout for the whole operation, one digit equal 5us
* returns a long with
*		LOW=return value from remote;
*		HIGH:result value of communication
*								10 remote timeout (not enough time in slot left), (this is never possible!!)
*		  						20	local timeout (wait for remote rts timeout),
*		  						30  connection timeout (transmitted, but no response),
*		  						0	okay
*
*
* Adapted by sabin (wendhack@teco.edu)
* 2004-01-22
*
* small changes in comment by Klaus Schmidt (schmidtk@teco.edu)
* 2004-04-05
*
*/

long RPC(long wait_time,int acl_function, int acl_param0,int acl_param1,int acl_param2)
{
	long result;
	long over;
	int result_int;
	int buff;

	result=255;


	while (wait_time)
	{
		wait_time--;			//decrease the wait_time
		DelayUs(150);
		if (input(PIN_RPC_HANDSHAKE_CLIENT))
		{
			//serial was disabled from remote
			RPCClientSerialDisable();				//disable my Uart
			//output_high(PIN_RPC_CL_TO_DO);			//I'm idle
		}
		else
		{
			//connection can start
			if (!bit_test(PIC_RCSTA,7))				// enable serial port (power on)//if (input(PIN_RPC_CL_TO_DO))
			{
				RPCClientSerialEnable();				// SENSOR state was idle before, now start Uart
				DelayUs(100);
				RPCClientSerialCheckError();
				RPCClientSerialFlushReceive();
				RPCClientSerialCheckError();
			}

			// now send the remote procedure call command
			RPCClientSerialSendByte(acl_function);
			RPCClientSerialSendByte(acl_param0);
			RPCClientSerialSendByte(acl_param1);
			RPCClientSerialSendByte(acl_param2);
			RPCClientSerialWaitForTxEnd();


			if (RPCClientSerialWaitForRx(50))					//wait for ack
			{
				buff=PIC_RCREG;
				if (buff==0b10101010)
				{
					if (RPCClientSerialWaitForRx(200))		//wait for answer
					{
						buff=PIC_RCREG;
						result=buff+(256*0);	//okay!
						RPCClientSerialDisable();
						return result;
					}
					else
					{
						result=(256*30);		//remote timeout
						RPCClientSerialDisable();
						return result;
					}

				}
			}
		}
	} //while
	RPCClientSerialDisable();
	return 256*20;		//local timeout
}

// this function is different for RF or Sensor
void RPCInitClient()
{
	bit_set(TRIS_RPC_HANDSHAKE_CLIENT);		//is input
	RPCClientSerialInit(SERIAL_625000,SERIAL_ASYNCHRONOUS,8);
	RPCClientSerialDisable();
}



/**
* driver to use ACL via RPC.
*
* 2004-02-05 by sabin (wendhack@teco.edu)
*
*
* ACL functions not available in RPC exist as empty ones.
* They are not included at the moment.
*
* changed 2004-04-05 by Klaus Schmidt (schmidtk@teco.edu)
* ver: 002
* - Added ACLFoundReceivedType
*/

#define RPC_ACL_DRIVER 002


//ACL
int ACLSubscribe(byte LL_type_h,LL_type_l);				//adds a type to the subscription list
int ACLUnsubscribe(byte LL_type_h,LL_type_l);			//deletes a type out of the subscription list
int ACLFlushSubscriptions();							//deletes all subscription (not the default ones)
int ACLSubscribeDefault();								//subscribes to the default types (control msgs..)
int ACLVerifySubscription(int type_h,int type_l);		//checks if a subscription is there
//void ACLInit();											//start ACL and lower layers, resets the whole stack
//#inline
//int ACLProcessControlMessages();						//internal: is called if control msg is there
void ACLSetFieldStrength(int power);					//sets the field strength of transmitter signals
int ACLSendingBusy();									//returns true if LL has Packet in send queue
int ACLGetSendSuccess();								//returns the result of last Packet transmission
int ACLSendPacket(int slot_timeout);					//starts packet transmission, return values!
int ACLAddNewType(int type_h, int type_l);				//adds a type into the ACL send buffer, return values!
int ACLAddData(int data);								//adds one byte data into the ACL send buffer, return values!
void ACLAbortSending();									//stops a running transmission
void ACLSubscribeAll();									//subscribes to any possible type (all packets are received)
//int ACLMatchesMyIP(char *buffer,int start);				//checks, if buffer holds my IP
//char* ACLGetReceivedData(int type_h, int type_l);		//returns the ACL payload data of the given type in the last received packet
int ACLGetReceivedPayloadLength();						//returns the number of bytes received
void ACLSetControlMessagesBehaviour(boolean ignore, boolean pass);	//if ignore==true: don't react on any control msg; if pass==true: give control msgs to ACL as received payload packets
signed int ACLGetReceivedDataLength(int type_h, int type_l);	//returns number of bytes received of given type in last msg
int ACLSentPacketInThisSlot();							//is true until next slot if msg was send in this slot
void ACLAnswerOnACM();									//if a control msgs comes in that requires an answer, it is answered
void ACLNoAnswerOnACM();									//if a control msgs comes in that requires an answer, it will not be answered
int ACLClearSendData();									//deletes the send buffer of ACL
//void ACLStart();										//restart ACL after ACLstop()
//void ACLStop();											//stops the RF stack. Everything is hold, continues after ACLstart. msg stay in queue
int ACLGetRemainingPayloadSpace();						//returns the number of free bytes in the transmit buffer
void ACLLockReceiveBuffer();							//locks the receive buffer: no new msgs are received
//int ACLReceiveBufferLocked();							//returns true if receivebuffer is locked
void ACLReleaseReceiveBuffer();							//un-lock the receivebuffer;
void ACLSetDataToOld();									//set received data to "old": means ACLDataIsNew will not return true unless a new packet was received
void ACLSetDataToNew();									//set received data to "new": means ACLDataIsNew will return true every slot
int ACLDataIsNew();										//returns true if data in receive buffer is "new"
char ACLGetReceivedByte(int type_h, int type_l, int position);		//returns a single received byte on the position
//void ACLStartup();										// is the first function in any case,  runs selftest and aclinit
int ACLDataReceivedInThisSlot();						//returns true if data was received in this slot
//int ACLDataIsNewNow();									//==ACLDataReceivedin this slot
int ACLFoundReceivedType(int type_h, int type_l);		//returns true if type was found
//int ACLAdressedDataIsNew();								// return1 if adressed data came
//int ACLAdressedDataIsNewNow();							// return 1 if adressed data came in this slot
//int ACLSendPacketAdressed(unsigned int add1, add2, add3, add4, add5, add6, add7, add8, timeout );//sends out the current packet with a target ID


int ACLSubscribe(byte LL_type_h,LL_type_l){

	unsigned int result;

		result = RPC( 20000, RPC_ACLSubscribe, LL_type_h, LL_type_l, 0 );
		return result;
}

int ACLUnsubscribe(byte LL_type_h,LL_type_l){

	unsigned int result;

		result = RPC( 20000, RPC_ACLUnsubscribe, LL_type_h, LL_type_l, 0 );
		return result;
}

int ACLFlushSubscriptions(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLFlushSubscriptions, 0, 0, 0 );
		return result;
}

int ACLSubscribeDefault(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLSubscribeDefault, 0, 0, 0 );
		return result;

}

int ACLVerifySubscription(int type_h,int type_l){

	unsigned int result;

		result = RPC( 20000, RPC_ACLVerifySubscription, type_h, type_l, 0 );
		return result;
}

//void ACLInit(){}

//int ACLProcessControlMessages(){}

void ACLSetFieldStrength(int power){

	unsigned int result;

		result = RPC( 20000, RPC_ACLSetFieldStrength, power, 0, 0 );
		return;
}

int ACLSendingBusy(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLSendingBusy, 139, 0, 0 );
		return result;
}

int ACLGetSendSuccess(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLGetSendSuccess, 0, 0, 0 );
		return result;
}

int ACLSendPacket(int slot_timeout){

	unsigned int result1;
	unsigned int result2;
	unsigned int ready;

		result1 = RPC( 20000, RPC_ACLSendPacket, slot_timeout, 0, 0 );

		ready=false;
 		while (!ready)
		{
			result2=RPC(20000,RPC_ACLSendingBusy,139,0,0);	//wait 100 ms for answer
			if (result2==0) ready=true;
		}

		return result1;
}

int ACLAddNewType(int type_h, int type_l){

	unsigned int result;

		result = RPC( 20000, RPC_ACLAddNewType, type_h, type_l, 0 );
		return result;
}

int ACLAddData(int data){

	unsigned int result;

		result = RPC( 20000, RPC_ACLAddData, data, 0, 0 );
		return result;
}

void ACLAbortSending(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLAbortSending, 0, 0, 0 );
		return;
}

void ACLSubscribeAll(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLSubscribeAll, 0, 0, 0 );
		return;
}

//int ACLMatchesMyIP(char *buffer,int start){}

//char* ACLGetReceivedData(int type_h, int type_l){}

int ACLGetReceivedPayloadLength(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLGetReceivedPayloadLength, 0, 0, 0 );
		return result;
}

void ACLSetControlMessagesBehaviour(boolean ignore, boolean pass){

	unsigned int result;

		result = RPC( 20000, RPC_ACLSetControlMessagesBehaviour, 0, 0, 0 );
		return;
}

signed int ACLGetReceivedDataLength(int type_h, int type_l){

	unsigned int result;

		result = RPC( 20000, RPC_ACLGetReceivedDataLength, type_h, type_l, 0 );
		return result;
}

int ACLSentPacketInThisSlot(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLSentPacketInThisSlot, 0, 0, 0 );
		return result;
}

void ACLAnswerOnACM(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLAnswerOnACM, 0, 0, 0 );
		return;
}

void ACLNoAnswerOnACM(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLNoAnswerOnACM, 0, 0, 0 );
		return;
}

int ACLClearSendData(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLClearSendData, 0, 0, 0 );
		return result;
}

//void ACLStart(){}

//void ACLStop(){}

int ACLGetRemainingPayloadSpace(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLGetRemainingPayloadSpace, 0, 0, 0 );
		return result;
}

void ACLLockReceiveBuffer(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLLockReceiveBuffer, 0, 0, 0 );
		return;
}

//int ACLReceiveBufferLocked(){}

void ACLReleaseReceiveBuffer(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLReleaseReceiveBuffer, 0, 0, 0 );
		return;
}

void ACLSetDataToOld(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLSetDataToOld, 0, 0, 0 );
		return;
}

void ACLSetDataToNew(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLSetDataToNew, 0, 0, 0 );
		return;
}

int ACLDataIsNew(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLDataIsNew, 0, 0, 0 );
		return result;
}

char ACLGetReceivedByte(int type_h, int type_l, int position){

	unsigned int result;

		result = RPC( 20000, RPC_ACLGetReceivedByte, type_h, type_l, position );
		return result;
}

void ACLStartup(){}

/*
int ACLDataReceivedInThisSlot(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLDataReceivedInThisSlot, 0, 0, 0 );
		return result;
}
*/

/*
int ACLDataIsNewNow(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLDataIsNewNow, 0, 0, 0 );
		return result;
}
*/


int ACLFoundReceivedType(int type_h, int type_l){

	unsigned int result;

		result = RPC( 20000, RPC_ACLFoundReceivedType, type_h, type_l, 0 );
		return result;
}




/*
int ACLAdressedDataIsNew(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLAdressedDataIsNew, 0, 0, 0 );
		return result;
}

int ACLAdressedDataIsNewNow(){

	unsigned int result;

		result = RPC( 20000, RPC_ACLAdressedDataIsNewNow, 0, 0, 0 );
		return result;
}

int ACLSendPacketAdressed(unsigned int add1, add2, add3, add4, add5, add6, add7, add8, timeout ){

	unsigned int result1;
	unsigned int result2;
	unsigned int ready;

		result = RPC( 20000, RPC_ACLSendPacketAdressed, 0, 0, 0 );

		ready=false;
 		while (!ready)
		{
			result2=RPC(20000,RPC_ACLSendingBusy,139,0,0);	//wait 100 ms for answer
			if (result2==0) ready=true;
		}

		return result1;
}
*/