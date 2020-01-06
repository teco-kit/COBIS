/**
* Over The Air Programming functions
*
*
* @author: Albert Krohn (based on the old code,... oh my god!!! :)
* @date: 2004-08-07 (yyyy-mm-dd)
* designed for the pc220 , with pic18f6720
*
*
* 	version 002: 2004-11-17 sabin (wendhack@teco.edu)
*		- left-sided hi() and lo() replaced.
*
*/





//Area in Program Flash Memory that has to be reserved for Otap_loader


//Area in external eeprom that can be used for saving OTAP data
#define OTAP_EXT_EEPROM_FIRST_PAGE	0
#define OTAP_EXT_EEPROM_LAST_PAGE	1100		// 64 "pages" is 64k, old pages names
//#define OTAP_EXT_EEPROM_LAST_PAGE	2000		// 64 "pages" is 64k, old pages names

//Area in Program Flash Memory that can be used for saving OTAP data
#define OTAP_PROGRAM_FLASH_START	0x14700
#define OTAP_PROGRAM_FLASH_END		0x1FFFF

//#define OTAP_MAX_NO_LINES			1940		//maximum number of lines that can be stored equals 64k
//#define OTAP_MAX_NO_LINES			4900		//maximum number of lines that can be stored equals 64k
#define OTAP_MAX_NO_LINES			4000		//maximum number of lines that can be stored equals 64k

#define OTAP_THIS_PROCESSOR			6720
#define OTAP_THIS_PROCESSOR_HI		67
#define OTAP_THIS_PROCESSOR_LO		20
#define OTAP_THIS_VERSION			1






//global variables for OTAP
long otap_TotalNumberOfLines;			//the number of coming Otap Packets times 2
long otap_ProgramLines;					//the number of Lines of the new Program
long otap_ExpectingLineNumber;			//next expected Otap Sequence Number
long otap_LastReceivedLineNumber;		//last received Otap Sequence Number
long otap_EepromWriteAddr;				//current Eeprom Write Address
int otap_DelaySlotCounter;				//counts the slots until the Acknowledge is repeated
int otap_BreakCounter;					//counts the slots until Particle gives up Otap
int otap_EndCounter;					//counts how many times the last Acknowledge has been sent
int otap_ProgramCRCh;					//store the Program CRC for later check
int otap_ProgramCRCl;
boolean otap_CRCok;						//stores the check result
boolean otap_packet_just_received;		//Otap packet was received in this slot?
#ifdef OTAP_PROGRAM_FLASH_START
int32 otap_FlashWriteAddr;				//current Program Flash Write Address
#endif

//function prototypes
int PCEepromBytesInThisSlot();
void OtapReset();
void OtapInit();
void OtapSetID();
int OtapIsRunning();
void  OtapCallLoader();
void OtapCRC16(unsigned int ser_data, unsigned int *crc_hb, unsigned int *crc_lb);
boolean OtapCheckProgramCRC();
void OtapReceive();
void OtapAnswerRepeat();





//*******************************************************************************************************
//this is the converterlayer that fakes the external eeprom and uses the external flash instead, for easy port of old otap code

//returns 0 if successful
//sequential write modus, will only write,if data fits into current page.
int PCEepromWriteSequence(long addr, int* buff, long count)
{
	if ( FlashWriteSequence((int32) addr, buff, count) ) return 0;
	return 1;
}
//returns 0 if successful
int PCEepromReadSequence(long addr,int* buff,long count)
{
	if (FlashReadSequence( (int32) addr,buff,count ) ) return 0;
	return 1;
}
/**
* Switches the PCEeprom on
*/
void PCEepromInit()
{
	//nothing to do here, the Flash should be initialized anyway
}

//*******************************************************************************************************


//-------------------------------------------------------
//---------OTAP    functions-----------------------------
//-------------------------------------------------------
//-------------------------------------------------------


// include the precompiled otap loader

#include "system/flash_loader.rom"




/**
* Calculates the number of bytes that can be read or written
* into Eeprom in the current slot
*
* @return number of bytes that can be read or written in current slot
*/
int PCEepromBytesInThisSlot()
{
	long timerest;
	timerest=RFMicroSecondsUntilNextSlot();
	return (int)((timerest-200)/50);			//each byte takes 50 microseconds plus ID/write/read and address
}


//The OTAP loader




/**
* Initializes the Otap global variables.
*
*/
void OtapInit()
{
	OtapReset();
}

/**
* Checks if Otap is running
*
* @return 1: Otap is running. 0: Otap is not running.
*/
int OtapIsRunning()
{
	if(otap_TotalNumberOfLines>0) return 1;
	else return 0;
}

/**
* Breaks Otap by resetting its global variables.
*
*/
void OtapReset()
{
	otap_TotalNumberOfLines=0;
	otap_ProgramLines=0;
	otap_ExpectingLineNumber=0;
	otap_LastReceivedLineNumber=0;
	otap_EepromWriteAddr=OTAP_EXT_EEPROM_FIRST_PAGE*64;
#ifdef OTAP_PROGRAM_FLASH_START
	otap_FlashWriteAddr=OTAP_PROGRAM_FLASH_START;
#endif
	otap_DelaySlotCounter=0;
	otap_BreakCounter=0;
	otap_EndCounter=0;
	otap_CRCok=FALSE;
	otap_ProgramCRCh=0;
	otap_ProgramCRCl=0;
	otap_packet_just_received=false;
}


/**
* Writes the Particle ID from a CID paket into internal Eeprom.
* Sends a CID paket back for testing the new ID.
*
*/
void OtapSetID()
{
	int i;

	for(i=0;i<8;i++)	//write the ID into internal Eeprom
	{
		WriteIntEeprom(EEPROM_ADDRESS_ID+i,LL_payload_receivebuffer[i+6]);
		LL_header_send[3+i]=ReadIntEeprom(EEPROM_ADDRESS_ID+i);
	}
	if (LLSendingBusy()) ACLAbortSending();			// interrupt any waiting stuff
	// now set the whole packet by hand:
	LL_payload_send[0]=ACL_TYPE_ACM_HI;
	LL_payload_send[1]=ACL_TYPE_ACM_LO;
	LL_payload_send[2]=0;
	LL_payload_send[3]=ACL_TYPE_CID_HI;
	LL_payload_send[4]=ACL_TYPE_CID_LO;
	LL_payload_send[5]=0;
	LL_payload_length=6;								// give the correct len
	if (LLSendPacket(ACL_CONTROL_MESSAGES_TIMEOUT))     // NEVER USE ACL FUNCTION here!!!
	{
		rf_status|=RF_STATUS_INSERTED_CTRLMSG;			// this means that this msg if special concerning the update of the states
	}
}


/**
* Calls the Otap Loader.
* The running code will be stopped and new received code will be burned by the Otap Loader.
*
*/
void  OtapCallLoader()
{
	ACLStop();						// halt
	disable_interrupts(global); 	// !!
	// adresse zerlegen

	//loader();

		#asm						//GOTO OTAP_LOADER_MAIN does not work for OTAP_LOADER_MAIN>0xFFFF, so set the Program Counter by hand
		MOVLW	OTAP_LOADER_MAIN_U
		MOVWF	PIC_PCLATU
		MOVLW	OTAP_LOADER_MAIN_H
	 	MOVWF  	PIC_PCLATH
		MOVLW	OTAP_LOADER_MAIN_L
		MOVWF	PIC_PCL
		#endasm



	goto_address(0x13000);
	#asm
		goto OTAP_LOADER_START
		goto 0x1F000
		goto 0xF000

	#endasm


}

/**
* Calculates 16bit CRC from one byte
*
* @param ser_data byte to be CRCed
* @param crc_hb high byte of 16bit CRC output
* @param crc_lb low byte of 16bit CRC output
*/
void OtapCRC16(unsigned int ser_data, unsigned int *crc_hb, unsigned int *crc_lb)
{
	unsigned int hb, lb;
	unsigned int tmp, tmp2;
	hb = *crc_hb;
	lb = *crc_lb;

	tmp = lb;
	lb = hb;
	hb = tmp;

	lb ^= ser_data;
	lb ^= lb >> 4;
	hb ^= lb << 4;

	hb ^= lb >> 3;
	lb ^= (lb << 4) << 1;

	*crc_hb = hb;
	*crc_lb = lb;

}

/**
* Checks the CRC of the whole received and saved program data
*
* @return TRUE: CRC is ok. FALSE: CRC is wrong.
*/
boolean OtapCheckProgramCRC()
{

	int i,crch,crcl;
	long k,lines;
	int otap_load[16];

	ACLStop();		//the check takes several hundred milliseconds
	FlashFlush();		//finish the last packet
	PCLedRedOn();
	PCLedBlueOn();

	crch=0;crcl=0;
	otap_EepromWriteAddr=OTAP_EXT_EEPROM_FIRST_PAGE*64+64;	//skip the Otap tables page

	#ifdef OTAP_PROGRAM_FLASH_START
	otap_FlashWriteAddr=OTAP_PROGRAM_FLASH_START;
	#endif

	PCEepromInit();						//initialize and power on eeprom module
	PCEepromReadSequence(0,otap_load,16);		//get number of ProgramLines
	lines=((long)otap_load[2]) << 8;
	lines+=(long)otap_load[3];

	for(k=0;k<lines;k++)
	{
		if(k<OTAP_EXT_EEPROM_LAST_PAGE*4)	//first read from Eeprom
		{
			PCEepromReadSequence(otap_EepromWriteAddr, otap_load, 16);
			otap_EepromWriteAddr+=16;
		}

		#ifdef OTAP_PROGRAM_FLASH_START
		else								//then read from Program Flash
		{
			read_program_memory(otap_FlashWriteAddr,otap_load,16);
			otap_FlashWriteAddr+=16;
		}
		#endif

		for(i=0;i<16;i++)
		{
			OtapCRC16(otap_load[i],&crch,&crcl);	//calculate the CRC
		}
	}

	OtapCRC16(otap_ProgramCRCh,&crch,&crcl);
	OtapCRC16(otap_ProgramCRCl,&crch,&crcl);		//check the CRC
	ACLStart();										//restart stack
	if(crch==0 && crcl==0) return TRUE;
	else return FALSE;

}


/**
* Checks the received Otap data.
* If it was the right paket (next Otap sequence number), the data will be stored
* and an acknowledge will be send back
*
*/
void OtapReceive()
{

	int otap_load[58];			//64-3-3
	int enoughspace=FALSE;
	{
		memcpy(otap_load,&(LL_payload_receivebuffer[6]),58);
		if(ACLMatchesMyIP(otap_load,0))			//I must be reprogrammed
		{

			if(otap_load[8]==0 && otap_load[9]==0 && otap_ExpectingLineNumber==0)		//Otap init received
			{
				otap_ProgramLines=((long)otap_load[17]) << 8;
				otap_ProgramLines+=(long)otap_load[18];

				if(otap_ProgramLines<=OTAP_MAX_NO_LINES) enoughspace=TRUE;

				if(otap_load[10]==OTAP_THIS_VERSION && otap_load[11]==OTAP_THIS_PROCESSOR_HI && otap_load[12]==OTAP_THIS_PROCESSOR_LO && enoughspace)
				{
					OtapReset();
					otap_TotalNumberOfLines=((long)otap_load[13]) << 8;
					otap_TotalNumberOfLines+=(long)otap_load[14];
					otap_ProgramCRCh=otap_load[15];
					otap_ProgramCRCl=otap_load[16];




					PCEepromInit();						//initialize and power on eeprom module
					if(PCEepromBytesInThisSlot()>37 && !PCEepromWriteSequence(otap_EepromWriteAddr,&(otap_load[15]),37))	//2+2+2+1+32
							//store Otap tables into Eeprom
					{
						//writing successful, request next line
						otap_EepromWriteAddr+=64;
						otap_LastReceivedLineNumber=1;		//init ack
						otap_ExpectingLineNumber=2;

						if (LLSendingBusy()) ACLAbortSending();			// interrupt any waiting stuff

						// now set the whole packet by hand:
						LL_payload_send[0]=ACL_TYPE_ACM_H;
						LL_payload_send[1]=ACL_TYPE_ACM_L;
						LL_payload_send[2]=0;
						LL_payload_send[3]=198;
						LL_payload_send[4]=88;
						LL_payload_send[5]=2;
						LL_payload_send[6]=0;
						LL_payload_send[7]=1;
						LL_payload_length=8;								// give the correct len

						if (LLSendPacket(ACL_CONTROL_MESSAGES_TIMEOUT))     // NEVER USE ACL FUNCTION here!!!
						{
							rf_status|=RF_STATUS_INSERTED_CTRLMSG;			// this means that this msg if special concerning the update of the states
						}
					}
				}
				else			//Version or Processortype error or not enough space
				{
					//send an error message
					LL_payload_send[0]=ACL_TYPE_ACM_H;
					LL_payload_send[1]=ACL_TYPE_ACM_L;
					LL_payload_send[2]=0;
					LL_payload_send[3]=198;
					LL_payload_send[4]=88;
					LL_payload_send[5]=6;
					LL_payload_send[6]=0xFF;
					LL_payload_send[7]=0xFF;
					LL_payload_send[8]=OTAP_THIS_VERSION;
					LL_payload_send[9]=OTAP_THIS_PROCESSOR_HI;
					LL_payload_send[10]=OTAP_THIS_PROCESSOR_LO;
					LL_payload_send[11]=enoughspace;
					LL_payload_send[13]=lo(otap_ProgramLines);
					LL_payload_send[12]=hi(otap_ProgramLines);
					LL_payload_length=14;								// give the correct len

					if (LLSendPacket(ACL_CONTROL_MESSAGES_TIMEOUT))     // NEVER USE ACL FUNCTION here!!!
					{
						rf_status|=RF_STATUS_INSERTED_CTRLMSG;			// this means that this msg if special concerning the update of the states
					}
				}

			}


			else if(otap_load[8]==hi(otap_ExpectingLineNumber) && otap_load[9]==lo(otap_ExpectingLineNumber))	//next line received
			{
				if(otap_EepromWriteAddr<=OTAP_EXT_EEPROM_LAST_PAGE*64+32)	//still space in Eeprom
				{
					PCEepromInit();
					if(PCEepromBytesInThisSlot()>32 && !PCEepromWriteSequence(otap_EepromWriteAddr,&(otap_load[10]),32))	//this takes 2ms
					{

						otap_EepromWriteAddr+=32;
						otap_LastReceivedLineNumber=otap_ExpectingLineNumber;
					}
				}

				if(otap_LastReceivedLineNumber<otap_TotalNumberOfLines)		//acknowledge all packets but the last one here
				{
					if (LLSendingBusy()) ACLAbortSending();			// interrupt any waiting stuff
					// now set the whole packet by hand:
					LL_payload_send[0]=ACL_TYPE_ACM_H;
					LL_payload_send[1]=ACL_TYPE_ACM_L;
					LL_payload_send[2]=0;
					LL_payload_send[3]=198;
					LL_payload_send[4]=88;
					LL_payload_send[5]=2;

					LL_payload_send[6]=hi(otap_ExpectingLineNumber);
					LL_payload_send[7]=lo(otap_ExpectingLineNumber);
					LL_payload_length=8;								// give the correct len

					if (LLSendPacket(ACL_CONTROL_MESSAGES_TIMEOUT))     // NEVER USE ACL FUNCTION here!!!
					{
						rf_status|=RF_STATUS_INSERTED_CTRLMSG;			// this means that this msg if special concerning the update of the states
					}

				}
				otap_ExpectingLineNumber+=2;
				otap_BreakCounter=0;
				otap_DelaySlotCounter=0;
			}
		}
	}
	otap_packet_just_received=true;
}//otap receive



/**
* Repeats sending the last Otap acknowledge packet,
* if the received Otap packet has the wrong Otap sequence number.
*
*/
void OtapAnswerRepeat()
{

	if (otap_packet_just_received==false)
	{
		if((otap_ExpectingLineNumber>otap_TotalNumberOfLines) && !ACLSendingBusy())
		{
			if(otap_EndCounter<15)
			{

				if (LLSendingBusy()) ACLAbortSending();			// interrupt any waiting stuff

				if(otap_EndCounter==0) otap_CRCok=OtapCheckProgramCRC();	//check the CRC

				//start new control packet
				LL_payload_send[0]=ACL_TYPE_ACM_H;
				LL_payload_send[1]=ACL_TYPE_ACM_L;
				LL_payload_send[2]=0;
				LL_payload_send[3]=198;
				LL_payload_send[4]=88;
				LL_payload_send[5]=3;

				if(otap_CRCok)		//send a success message, the acknowledge of the last packet
				{
					LL_payload_send[6]=hi(otap_LastReceivedLineNumber);
					LL_payload_send[7]=lo(otap_LastReceivedLineNumber);
				}
				else				//send an CRC error message
				{
					LL_payload_send[6]=0xCC;
					LL_payload_send[7]=0xCC;
				}

				LL_payload_length=8;								// give the correct len
				if (LLSendPacket(ACL_CONTROL_MESSAGES_TIMEOUT))     // NEVER USE ACL FUNCTION here!!!
				{
					rf_status|=RF_STATUS_INSERTED_CTRLMSG;			// this means that this msg if special concerning the update of the states
				}

				otap_EndCounter++;
			}
			else if(otap_CRCok) OtapCallLoader();
			else OtapReset();
		}

		else if(otap_ExpectingLineNumber!=0)
		{
			if(otap_BreakCounter>100) OtapReset();					//break ota session
			else if(otap_DelaySlotCounter>5 && !ACLSendingBusy())	//repeat the last acknowledge
			{
				if (LLSendingBusy()) ACLAbortSending();			// interrupt any waiting stuff
				// now set the whole packet by hand:
				LL_payload_send[0]=ACL_TYPE_ACM_H;
				LL_payload_send[1]=ACL_TYPE_ACM_L;
				LL_payload_send[2]=0;
				LL_payload_send[3]=198;
				LL_payload_send[4]=88;
				LL_payload_send[5]=2;
				LL_payload_send[6]=hi(otap_LastReceivedLineNumber);
				LL_payload_send[7]=lo(otap_LastReceivedLineNumber);
				LL_payload_length=8;								// give the correct len
				if (LLSendPacket(ACL_CONTROL_MESSAGES_TIMEOUT))     // NEVER USE ACL FUNCTION here!!!
				{
					rf_status|=RF_STATUS_INSERTED_CTRLMSG;			// this means that this msg if special concerning the update of the states
				}

				otap_DelaySlotCounter=0;
				otap_BreakCounter++;
			}
			else if(!ACLSendingBusy()) otap_DelaySlotCounter++;

		}
		otap_packet_just_received=false;
	}
	else
	{
		otap_packet_just_received=false;
	}

}






