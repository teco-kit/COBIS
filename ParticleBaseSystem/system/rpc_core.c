/* RemoteProcedureCall
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
*/

#define RPC_CORE 002

#define RPC_SER_COMM_TIME_OUT		205


#define RPC_ACLAddData				101
#define RPC_ACLAddNewType			102
#define RPC_ACLSendPacket			103
#define RPC_ACLTestFunction			104
#define RPC_ACLSubscribe			105
#define RPC_ACLUnsubscribe			106
#define RPC_ACLSubscribeDefault		107
#define RPC_ACLFlushSubscriptions	108
#define RPC_ACLVerifySubscription	109
#define RPC_ACLInit					110
#define RPC_ACLSetFieldStrength		111
#define RPC_ACLSendingBusy			112
#define RPC_ACLGetSendSuccess		113
#define RPC_ACLAbortSending			114
#define RPC_ACLSubscribeAll			115
#define RPC_ACLGetReceivedPayloadLength		116
#define	RPC_ACLSetControlMessagesBehaviour	117
#define RPC_ACLGetReceivedDataLength		118
#define RPC_ACLSentPacketInThisSlot	119
#define RPC_ACLAnswerOnACM			120
#define RPC_ACLNoAnswerOnACM		121
#define RPC_ACLClearSendData		122
#define RPC_ACLStart				123
#define RPC_ACLStop					124
#define RPC_ACLGetRemainingPayloadSpace		125
#define RPC_ACLLockReceiveBuffer	126
#define RPC_ACLReleaseReceiveBuffer	127
#define RPC_ACLSetDataToOld			128
#define RPC_ACLSetDataToNew			129
#define RPC_ACLDataIsNew			130
#define RPC_ACLGetReceivedByte		131
#define RPC_ACLTestReturn			132

#define RPC_ACLFoundReceivedType	133

#define RPC_VoltageSensorPrepare	134
#define RPC_VoltageSensorGet		135
#define RPC_ReturnPointerResult		136


void EscapeAndPacket(char* in, char* out, int inlen,int* outlen)
{
	int i,pos;

	pos=0;
	out[pos++]=0;
	for (i=0;i<inlen;i++)
	{
		if (in[i]==0)
		{
			out[pos++]=219;
			out[pos++]=1;
		}
		else if (in[i]==192)
		{
			out[pos++]=219;
			out[pos++]=193;
		}
		else if (in[i]==219)
		{
			out[pos++]=219;
			out[pos++]=220;
		}
		else
		{
			out[pos++]=in[i];
		}
	}

	out[pos++]=192;
	*outlen=pos;

}

boolean DeEscapePacket(char* in, char* out, int inlen,int* outlen)
{
	int i,pos;



	if (in[0]!=0) return false;
	if (in[inlen-1]!=192) return false;

	pos=0;
	i=1;
	while (i<inlen-1)
	{
		if (in[i]==0) return false;		//not allowed
		if (in[i]==192) return false;

		if (in[i]==219)
		{
			i++;
			out[pos++]=in[i++]-1;
		}
		else
		{
			out[pos++]=in[i++];
		}
	}
	*outlen=pos;
}

