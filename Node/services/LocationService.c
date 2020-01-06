#undef this
#define this(F) CAT(Location,F)

#include <cobis/util.h>



#ifndef Location_debug
#define Location_debug 0
#endif



#if this(debug) > 0

#ifdef USE_BLUE_LED
#error led used twice
#endif

#define USE_BLUE_LED 1

#ifdef USE_RED_LED
#error led used twice
#endif

#define USE_RED_LED 1
#endif



const unsigned int this(DefaultRate)=1;
char this(ACL)[]={ACL_TYPE_ALPHA_ARG('S','L','C'),0};

#define IR_SAMPLES_IN_A_ROW 3
#define IR_OUT_OF_SIGHT 20

uint8_t this(location)=0;
uint8_t this(poll)=0;

void this(Run())
{
  return;
}


void this(Server())
{
}


void this(OnWakeup())
{
  CateyeClipIROn();
  this(poll)=1;
}

void this(OnSend())
{
    ACLAddNewType(ACL_TYPE_ALPHA_ARG('S','L','C'));
    ACLAddData(this(location));
}

void this(OnSleep())
{
  CateyeClipIROff();
}

uint8_t this(lastIRByte)=0;
uint8_t this(continuousCounter)=0;
uint8_t this(noIRCounter)=0;

void this(OnReceive())
{
    uint8_t IRByte;
    IRSensorPrepare();


 	if(!this(poll)) return;

    if ( !(IRSensorGet(&IRByte)) ) // successfully received a sensor value
    {
		#if this(debug) == 1
		    PCLedRedOn();
		#endif

		if( (((~IRByte)>>4)&0xF) == (IRByte&0xF) ) //Checksum
		{


		  if (this(lastIRByte) == IRByte)
		  {

			if (this(continuousCounter)==0) // received IR_SAMPLES_IN_A_ROW with the same value (can safely overflow)
			{
		      #if this(debug) == 1
				 		PCLedBlueOn();
			  #endif

			  this(location) = IRByte & 0xF;

		      CateyeClipIROff();		// try to save energy
			  this(poll)=0;
			}
			else
			  this(continuousCounter)--;

		  }
		  else
		  {
			this(lastIRByte) = IRByte;
			this(continuousCounter) = IR_SAMPLES_IN_A_ROW ;
		  }

		  #if this(debug) == 1
		    delay_ms(1);
			PCLedRedOff();
			PCLedBlueOff();
		  #endif

		  // reset the no-IR-counter
		  this(noIRCounter) = IR_OUT_OF_SIGHT;
		}
	}



	if ( this(noIRCounter) == 0)
    {
      this(location) = 0; // no location after IR_OUT_OF_SIGHT*13ms nothing (can safely overflow)
    }
    else
	  this(noIRCounter)--;


	#if this(debug) == 1
      if(this(location)==0)
      	PCLedRedOn();
    #endif
}


void this(Init())
{
}
