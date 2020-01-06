// APPLICATION FILE
// uses ACK, RTC & PRS.

#define this(F) F

#case

#opt 0

#define byte	BYTE
#define boolean BOOLEAN
#define true	TRUE
#define false	FALSE
#define global	GLOBAL


#define RF_ZERO_PRIORITY	1

#define INC(X) #X

#include "system/acltypes.h"			// the acl types

#include "boards/18f6720.h"
#include "boards/18f6720_R.h"

#include INC(boards/BOARD.h)

#include "boards/wdt18fxx20.h"
#include "boards/18f6720.c"

// We use V9 to be more robust !?

#include "boards/wdt18fxx20.c"

#if BOARD_ID_LOW > 229
#include "boards/ballswitch.c"
#include "boards/ds2431.c"	//id chip
#include "boards/AT45DB041.c" //flash
#include "boards/owmb.c" 	//speaker
#else
#include "boards/pci2c.c"
#include "boards/pceeprom.c" //flasE
#endif

#include INC(boards/BOARD.c)

#include "system/awarecon#092.c"			// stack

#ifndef BOARD_ID_LOW
#error NO BOARD DEFINED
#endif

#if BOARD_ID_LOW > 229
#include "system/flash_otap.c"			//We don't use this
#else
#include "system/Otap.c"			//We don't use this
#endif
#fuses hs,noprotect,nobrownout,nolvp,put,nowdt

#include "../Node/cobis/util.h"
#include "../Node/cobis/Method.h"

#include "IRBoard10.h"			// beacon board pin definitions
#include "IRBoard10.c"			// beacon board functions


char this(ACL)[]={ACL_TYPE_ALPHA_ARG('I','R','B'),0};

// new gob vars
unsigned int slotcounter=0;
unsigned int sync_count=0;
unsigned int ir_count=0;

#ifndef DEFAULT_DUTYCYCLE_LENGTH
#ifdef SYNC_BEACON
#define DEFAULT_DUTYCYCLE_LENGTH 	8192	// about 2 seconds (DEBUG)
#else
#define DEFAULT_DUTYCYCLE_LENGTH 	0
#endif
#endif

#ifndef DEFAULT_WAKE_LENGTH
#define DEFAULT_WAKE_LENGTH 1600
#endif

uint16_t this(dutycycle_length)=DEFAULT_DUTYCYCLE_LENGTH;
Setter(dutycycle_length)

uint16_t this(sleep_length)=DEFAULT_DUTYCYCLE_LENGTH-DEFAULT_WAKE_LENGTH;
Setter(sleep_length)

// use ID from makefile (newID = "IDID")
unsigned int this(id)=ID;
Setter(id)

uint8_t this(internalIRStrength)=INITIAL_IR_STRENGTH;
Setter(internalIRStrength)

uint8_t this(externalIRStrength)=INITIAL_IR_STRENGTH;
Setter(externalIRStrength)

// global defs

#define NO_OF_SLOTS_BETWEEN_SYNC	3

#define NO_OF_SLOTS_BETWEEN_IR	0


// function definitions
void SendIRBeacon();

// send beacon time: (standard) 25ms delay, locationID=25
void SendIRBeacon()
{
   uint8_t sbyte;
   sbyte=this(id)&0xF;
   sbyte=sbyte|((~sbyte)<<4);
   PCLedRedOn();
   IRsend38khz(sbyte);		// send byte
   PCLedRedOff();
}

// pack and send a sync packet
int sendSync()
{
	unsigned long t3;

	if(!this(dutycycle_length)) return 1;

	ACLAddNewType(ACL_TYPE_ALPHA_ARG('S', 'N', 'W'));
	ACLAddNewType(ACL_TYPE_ALPHA_ARG('M','S','P'));

	// just add dutycycle and timer3 sync value @predefined offset
	ACLAddData(this(dutycycle_length)>>8);
	ACLAddData(this(dutycycle_length));
	ACLAddData(this(sleep_length)>>8);
	ACLAddData(this(sleep_length));
	while(ACLSendingBusy());
{
	while(RF200usLeft());

	t3=get_timer3();

	ACLAddData(t3>>8);
	ACLAddData(t3);

	ACLSendPacket(1);
} //assert that this happens in same slot

	while(ACLSendingBusy());

	return ACLGetSendSuccess();

}
//-------------------------------------------------------------------
// this function is called from the fsm at the end of an rf slot
// make sure that it terminates in time
#separate
void SlotEndCallBack()
{
	// increase slotcounter
	slotcounter++;


	if(sync_count)
		sync_count--;
	if(ir_count)
		ir_count--;


   if(ACLAdressedDataIsNew())
   {
		    char *data;
		    char send=0;

			ACLSetDataToOld();
		    ACLClearSendData();

		    ACLAddNewType(this(ACL)[0],this(ACL)[1]);

			data=ACLGetReceivedData(ACL_TYPE_ALPHA_ARG('C','I','I'));
			if(data)	send|=SetVar(internalIRStrength)(data);

		    data=ACLGetReceivedData(ACL_TYPE_ALPHA_ARG('C','I','E'));
			if(data)	send|=SetVar(externalIRStrength)(data);


			if(send)
			{
				IRSetFieldStrength(this(internalIRStrength)-1, INTERNAL_IR_LED);
				IRSetFieldStrength(this(externalIRStrength), EXTERNAL_IR_LED);
			}

			data=ACLGetReceivedData(ACL_TYPE_ALPHA_ARG('C','D','L'));
			if(data)	send|=SetVar(dutycycle_length)(data);

		    data=ACLGetReceivedData(ACL_TYPE_ALPHA_ARG('C','D','S'));
			if(data)	send|=SetVar(sleep_length)(data);


		    if(send)
		    {
		      ACLSendPacket(20);
		    }
		    else
		    {
		      ACLClearSendData();
		    }
  }
}

//-------------------------------------------------------------------
void main()
{
	// global init
	PCInit();											// is not dangerous, because all pins are set correct . bport is input, i2c and eeprom are initianlized as well

	// ir board init
	IRBoardInit();

	ACLInit();											// init the stack and start it

	enable_interrupts(global);							// must be done before lifesign and

	RFSetFieldStrength(32);
	IRSetFieldStrength(this(internalIRStrength)-1, INTERNAL_IR_LED);
	IRSetFieldStrength(this(externalIRStrength), EXTERNAL_IR_LED);

    AppSetLEDBehaviour(LEDS_OFF);

	ACLStart();





	// init timer 3 syncronization, setup and start timer 3 (RTC)
	bit_set( PIC_T1CON, 3 );		//TIMER1 oszillator enable (is for timer3 as well)
	PIC_T3CON = 0b01110111;		//timer3: 2*8bit acces,1:8 prescale,no external sync, input from timer 1 oszi, enable timer

        {
          uint16_t t3;
          t3=0-this(dutycycle_length);
          set_timer3(t3);
        }
        enable_interrupts(INT_TIMER3);

        enable_interrupts(global);


        ACLSubscribe(this(ACL)[0],this(ACL)[1]);

        RFDelaySlots(1);
        // send sync and location information
        while(1)
        {


          uint16_t overflow_sleep;
          overflow_sleep=0-this(sleep_length);
          if (get_timer3() > overflow_sleep)
            PCLedBlueOff();


          if (ir_count==0 && (this(internalIRStrength)!=0) )
          {
            SendIRBeacon();
            ir_count=NO_OF_SLOTS_BETWEEN_IR;
          }



          // Send sync?
          if (sync_count==0)
          {
            if(sendSync())
              sync_count=NO_OF_SLOTS_BETWEEN_SYNC;
          }
          else
            RFDelaySlots(1);




        }
}


#int_timer3
void setDutycycle()
{
if(this(dutycycle_length))
{
  disable_interrupts(global);
  PIC_PIR1=0;		// clear interrupt flags
  PIC_PIR2=0;
  PCLedBlueOn();

  set_timer3(get_timer3()-this(dutycycle_length));

  enable_interrupts(global);
}
}
