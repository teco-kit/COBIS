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

#include "boards/cateyeclip10.h"			// beacon board pin definitions
#include "sensors/TSOP36236TR.c"			// beacon board pin definitions
#include "boards/cateyeclip10.c"			


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


// function definitions
#separate
void SlotEndCallBack()
{
}

//-------------------------------------------------------------------
void main()
{
	PCInit();											// is not dangerous, because all pins are set correct . bport is input, i2c and eeprom are initianlized as well
   CateyeClipInit();
   for(;;)
   {
      CateyeClipLEDs(0);
      delay_ms(1000);
      while(CateyeClipButtonPressed());
      CateyeClipLEDs(3);
      delay_ms(1000);
   }
}
