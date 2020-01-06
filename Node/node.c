/*	COBIS Node (was DC Device)
*
* Author: Christian Decker (cdecker@teco.edu)
* Created: 2004-01-30
* version: 001
* Author: Till Riedel(riedel@teco.edu)
*/

// headerfile for application

#case


// to reduce compiler optimization. necessary for ccs 202. don't have this comment in the same line as #opt!
#opt 0

#define byte	BYTE
#define boolean BOOLEAN
#define true	TRUE
#define false	FALSE
#define global	GLOBAL

//#define CATEYE_DEBUG	1


#include "system/acltypes.h"			// the acl types

#include "boards/18f6720.h"
#include "boards/18f6720_R.h"

#include "boards/pc232.h"
#include "boards/cateyeclip10.h"

#include "boards/wdt18fxx20.h"

#include "boards/18f6720.c"
#include "boards/sensori2c.c"


//#include "boards/pceeprom.c"
#include "boards/AT45DB041.c" //flash

#include "boards/owmb.c" 	//speaker
#include "boards/ds2431.c"	//id chip
#include "boards/ballswitch.c"

#include "boards/pc232.c"

// We use V9 to be more robust !?
#include "system/awarecon#092.c"			// stack

#include "boards/wdt18fxx20.c"
#include "system/flash_otap.c"			//We don't use this




//+++++ include the sensors your want to use ++++++++++++++++++++++++++++++++++
//

#include "sensors/SP0101NC1.c"

//XXXXXXXXXXX
//#ifndef NO_ADXL
//#include "sensors/adxl210.c"
//#endif

#include "sensors/mcp9800.c"
//#include "sensors/tsl2550.c"
#include "sensors/voltage.c"
#include "sensors/TSOP36236TR.c"


#include "boards/cateyeclip10.c"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//#include "senscore.c"			// this is essential for all sensorboard stuff


#include "system/rtc.c"				// RTC

// other app stuff
#include "system/ack.c"			// acknowledged transmit/receive


#include "cobis/Service.h"			// DigiClip Library



#if 0
#define IR_SAMPLES_IN_A_ROW		3	// number of equals IR samples to serve as locationID
#define IR_OUT_OF_SIGHT			250 // threshold time in slots for no IR receiving
#endif


/**
 *******************************************************************
 *
 * DigiClip Runtime
 *
 *******************************************************************
 */

// this function is called from the fsm at the end of an rf slot
// make sure that it terminates in time
int lock_send=0;
int reset_watchdog=1;
int startup=1;

#pragma separate
void SlotEndCallBack()
{
  ServicesRun();
}

void main()
{
  PCInit();
  ACLInit();
  AppSetLEDBehaviour(LEDS_OFF);

  enable_interrupts(global);

  CateyeClipInit();		// board init

  CateyeClipSensorsInit();
  CateyeClipSensorsOff();


  CateyeClipActuatorsInit();
  CateyeClipActuatorsOff();


  ServicesInit();
  ServicesStart();

  for(;;)
  {
    ServicesServe();
  }
}
