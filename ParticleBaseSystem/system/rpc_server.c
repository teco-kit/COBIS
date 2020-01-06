/* RemoteProcedureCall-Server for Particle Coreboard
* allows to address particular functions on the coreboard from the SPart
*
* 1st version by Albert Krohn (krohn@teco.edu)
*
* changed by Klaus Schmidt 2004-04-01
* ver: 002
* - added the four last functions (133-136):
*   RPC_ACLFoundReceivedType
*	to use voltage-sensor on coreboard:
*	RPC_VoltageSensorPrepare; RPC_VoltageSensorGet; RPC_ReturnPointerResult
*
*/

#define RPC_SERVER 002


// the var following (rpc_remote_pointer_result) is used to tranfer the data of results from RPC-functions,
// which hand over a pointer as a parameter. The Function which gives back the values of this array is called
// RPC_ReturnPointerResult(command[1]) which is defined in the switch-case-block.
#define ARRAYLENGTH 2
int rpc_remote_pointer_result[ARRAYLENGTH];

void RPCServerInit()
{
	bit_clear(TRIS_RPC_HANDSHAKE_SERVER);	// set communication control pin to output
	output_high(PIN_RPC_HANDSHAKE_SERVER);	//set communication off
}

int ACLTestFunction(int arg)
{

	if (ACLSendingBusy()) return 12;
	ACLAddNewType(80,80);
	ACLAddData(arg);
	return (ACLSendPacket(20));
}


int ProcessRPCCall(int* command)
{
	int result;
	long tempvar;
	result=0;
	tempvar=0;

	switch(command[0])
	{
		case RPC_ACLAddNewType:						result=ACLAddNewType(command[1],command[2]);break;
		case RPC_ACLAddData: 						result=ACLAddData(command[1]);break;
		case RPC_ACLSendPacket:						result=ACLSendPacket(command[1]);break;
		case RPC_ACLTestFunction:   				result=ACLTestFunction(command[1]);break;
		case RPC_ACLSendingBusy:					result=ACLSendingBusy();break;
		case RPC_ACLSubscribe:						result=ACLSubscribe(command[1],command[2]);break;
		case RPC_ACLUnsubscribe:					result=ACLUnsubscribe(command[1],command[2]);break;
		case RPC_ACLSubscribeDefault:					   ACLSubscribeDefault();break;
		case RPC_ACLFlushSubscriptions:					   ACLFlushSubscriptions();break;
		case RPC_ACLVerifySubscription:				result=ACLVerifySubscription(command[1],command[2]);break;
		//case RPC_ACLInit:								   ACLInit();break;
		case RPC_ACLSetFieldStrength:					   ACLSetFieldStrength(command[1]);break;
		case RPC_ACLGetSendSuccess:					result=ACLGetSendSuccess();break;
		case RPC_ACLAbortSending:						   ACLAbortSending();break;
		case RPC_ACLSubscribeAll:						   ACLSubscribeAll();break;
		case RPC_ACLGetReceivedPayloadLength:		result=ACLGetReceivedPayloadLength();break;
		case RPC_ACLSetControlMessagesBehaviour:	result=ACLSetControlMessagesBehaviour(command[1],command[2]);break;
		case RPC_ACLGetReceivedDataLength:			result=ACLGetReceivedDataLength(command[1],command[2]);break;
		case RPC_ACLSentPacketInThisSlot:			result=ACLSentPacketInThisSlot();break;
		case RPC_ACLAnswerOnACM:						   ACLAnswerOnACM();break;
		case RPC_ACLNoAnswerOnACM:						   ACLNoAnswerOnACM();break;
		case RPC_ACLClearSendData:					result=ACLClearSendData();break;
		//case RPC_ACLStart:							   ACLStart();break;
		//case RPC_ACLStop:								   ACLStop();break;
		case RPC_ACLGetRemainingPayloadSpace:		result=ACLGetRemainingPayloadSpace();break;
		case RPC_ACLLockReceiveBuffer:					   ACLLockReceiveBuffer();break;
		case RPC_ACLReleaseReceiveBuffer:				   ACLReleaseReceiveBuffer();break;
		case RPC_ACLSetDataToOld:						   ACLSetDataToOld();break;
		case RPC_ACLSetDataToNew:						   ACLSetDataToNew();break;
		case RPC_ACLDataIsNew:						result=ACLDataIsNew();break;
		case RPC_ACLGetReceivedByte:				result=ACLGetReceivedByte(command[1],command[2],command[3]);break;
		case RPC_ACLTestReturn:						result=command[1];break;

		case RPC_ACLFoundReceivedType:				result=ACLFoundReceivedType(command[1],command[2]);break;

		case RPC_VoltageSensorPrepare:				result=VoltageSensorPrepare(); break;
		case RPC_VoltageSensorGet:					result=VoltageSensorGet(&tempvar); rpc_remote_pointer_result[0] = hi(tempvar); rpc_remote_pointer_result[1] = lo(tempvar); break;
		case RPC_ReturnPointerResult:				result=rpc_remote_pointer_result[command[1]]; break;
}



	RPCServerSerialSendByte(result);

	return;
}

//this is the main function for RF boards, should be called in appcallback

void RPCServerRun()
{
	int buff;
	int rpc_command[5];
	int rpc_pointer;
	boolean over;
	int max_time;

	output_low(PIN_RPC_HANDSHAKE_SERVER);											// start serial comm (remote turns now the uart on)
	RPCServerSerialInit(SERIAL_625000,SERIAL_ASYNCHRONOUS,8);
	//DelayUs(30);//!!
	RPCServerSerialCheckError();
	RPCServerSerialFlushReceive();
	RPCServerSerialCheckError();



	while ((PIC_TMR1H<RPC_SER_COMM_TIME_OUT))
	{


		RPCServerSerialCheckError();
		if (RPCServerSerialDataIsThere())
		{
			//output_low(PIN_TEST);delay_cycles(10);output_high(PIN_TEST);

			max_time=PIC_TMR1H+3;	// this is 100-150us
			over=false;
			rpc_pointer=0;
 			while ((PIC_TMR1H<=max_time) && (!over) )
			{
				if(RPCServerSerialCheckError()) over=true;
				if (RPCServerSerialDataIsThere())
				{
					rpc_command[rpc_pointer++]=RPCServerSerialGetByte();		//get one byte
					//output_low(PIN_TEST);output_high(PIN_TEST);
				}

				if (rpc_pointer>=4)
				{
					//output_low(PIN_TEST);delay_cycles(4);output_high(PIN_TEST);
					RPCServerSerialSendByte(0b10101010);			//ack
					ProcessRPCCall(rpc_command);
					rpc_pointer=0;
					over=true;
				}
			}
			//output_low(PIN_TEST);delay_cycles(10);output_high(PIN_TEST);
		}
	}
	output_high(PIN_RPC_HANDSHAKE_SERVER);	// serial comm off
}



