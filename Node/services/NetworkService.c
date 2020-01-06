#define this(F) CAT(Network,F)

#include <cobis/util.h>

//#define Network_debug 3


#ifndef Network_debug
#define Network_debug 0
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

char this(ACL)[]={ACL_TYPE_ALPHA_ARG('S','N','W'),0};

char this(startup)=1;
char this(restart_wdt)=0;

#define DEFAULT_DUTYCYCLE_LENGTH 	8192	// about 2 seconds (DEBUG)
#define DEFAULT_WAKE_LENGTH 1600


uint16_t this(dutycycle_length)=DEFAULT_DUTYCYCLE_LENGTH;
uint16_t this(sleep_length)=DEFAULT_DUTYCYCLE_LENGTH-DEFAULT_WAKE_LENGTH;

int this(majorSync)=0;



#define MAX_NO_SYNC 10
#define CLOCK_RATE CLOCK_32kHz

#define WDT_RATE /*18ms<<*/3

#fuses hs,noprotect,nobrownout,nolvp,put,NOWDT

char this(synced)=0;

#define reset_synced() (this(synced)=MAX_NO_SYNC)
#define just_synced() (this(synced)==MAX_NO_SYNC)

void this(OnSend()) {}

void this(DelaySlots(uint8_t slots))
{
#ifdef WATCHDOG
  this(restart_wdt)+=slots;
#endif
  RFDelaySlots(slots);
}

void this(SendPacket(uint8_t slots))
{
  for(slots;slots;slots--)
        if(ACLSendingBusy()) this(DelaySlots)(1); //TODO: check for interrupts
        else break;

  if(slots)
  {
    ServicesSend();
    ACLSendPacket(slots);
  }
  else
  {
    ACLClearSendData();
  }
}


// used for normal sleep and hibernation mode
#define HIBERNATE_SLEEP		        12 //TODO: Make absolute in time

#define CCL_ON				3
#define CCL_OFF				0

#define LED_PATTERN_ODD 0b10101010;
#define LED_PATTERN_EVEN 0b01010101;
#define LED_PATTERN_OFF 0b00000000;

#define LED_PATTERN_FLASH1 0b10101010;
#define LED_PATTERN_FLASH2 0b01010101;

uint8_t this(Led1Pattern)=0;
uint8_t this(Led2Pattern)=0;
uint8_t this(alarm_ticker)=0;



void this(doAlarm())
{
  uint8_t LEDs=0;
  if (bit_test(this(Led1Pattern), this(alarm_ticker)%8))
    LEDs|=1;
  if (bit_test(this(Led2Pattern), this(alarm_ticker)%8))
    LEDs|=2;
  CateyeClipLEDs(LEDs);

  this(alarm_ticker)++;
}


/*
uint8_t this(hibernate)=0;


void this(OffState())
{
      // short LED flash
      CateyeClipLEDs(CCL_ON);
      PCLedRedOff();
      PCLedBlueOff();
      DelayMs(1);
      CateyeClipLEDs(CCL_OFF);

      this(hibernate)=0;
      // stay here until button pressed for HIBERNATE_SLEEP times
      while (1)
      {
        if (CateyeClipButtonPressed())
          this(hibernate++);
        else
          this(hibernate)=0;

        if (this(hibernate) > HIBERNATE_SLEEP)
        {
          this(hibernate)=0;
          break;
        }
        restart_wdt();
        sleep();
#asm
        nop
        nop
        nop
#endasm
      }
      // short LED flash
      CateyeClipLEDs(CCL_ON);
      PCLedRedOff();
      PCLedBlueOff();
      DelayMs(1);
      CateyeClipLEDs(CCL_OFF);
}
*/
#undef NOSLEEP

void this(Server())
{
  uint8_t t3h, t3l;
  uint16_t overflow_duty = 0;
  uint16_t overflow_sleep;

  // stay here till sleep
  overflow_sleep=0-this(sleep_length);

  if (get_timer3() > overflow_sleep)
  {
    /* preparing sleep */
    ServicesSleep();

	while (bit_test(PIC_INTCON, 7)) bit_clear(PIC_INTCON, 7); //disable global w/o peripheral & lo_prio?

#if this(debug) == 1
	/* going to sleep, blue led off (DEBUG) */
		PCLedBlueOff();
#endif

#if this(debug) == 3
	/* going to sleep, both leds off (DEBUG) */
		PCLedBlueOff();
		PCLedRedOff();
#endif

  /* MD: TODO: blink alarm in sleep.. */

  // clear interrupt flags before sleep
  PIC_PIR1=0;
  PIC_PIR2=0;

  enable_interrupts(INT_TIMER3);			// enable timer3 overflow interrupt (TMR3IE) w/ disabled global

  //#define NOSLEEP 		1


  #ifdef NOSLEEP
  // DEBUG SLEEP
  while (!bit_test(PIC_PIR2, 1));
  // END DEBUG SLEEP
  #else
  {
	  // set LED PINs to GND (must be done because of leaky tris)
	  bit_clear(TRIS_LED_RED_POWER);
	  output_low(PIN_LED_RED_POWER);
	  bit_clear(TRIS_LED_GREEN_POWER);
	  output_low(PIN_LED_GREEN_POWER);

  	  sleep();
  	  #asm
          nop
          nop
          nop
	  #endasm
  }
  #endif

  disable_interrupts(INT_TIMER3);

  // clear interrupt flags
  PIC_PIR1=0;
  PIC_PIR2=0;

  rf_sync_state=SYNC_STATE_ALONE;	// make sure to sync immediately
  rf_status=RF_STATUS_LAYER_ON;
  enable_interrupts(global);		// ~~ same as ACLStart()

#if this(debug) == 1
   PCLedBlueOn();
#endif

	/* calculate wakup time (-> timer3), set timer3 */
  	overflow_duty=get_timer3();
  	overflow_duty-=this(dutycycle_length);
  	set_timer3(overflow_duty);

    ServicesWakeup();
  }
}


void this(OnSleep())
{
  while(ACLSendingBusy()) //wait here until all sending is done
    this(DelaySlots(1));
  this(doAlarm());
}


void this(OnWakeup())
{


  this(majorSync)=0;

  this(doAlarm());

  if(this(synced))
  {
#if this(debug) == 1
    PCLedRedOn();
#endif
    this(synced)--;
  }
  else
  {
#if this(debug) == 1
    PCLedRedOff();
#endif
    //TODO: Do something sensible here
    /*
    this(dutycycle_length)=DEFAULT_DUTYCYCLE_LENGTH;
    this(sleep_length)=DEFAULT_DUTYCYCLE_LENGTH-DEFAULT_WAKE_LENGTH;
    this(next_sync_counter)=DEFAULT_DUTYCYCLE_LENGTH;
    */
  }
}

uint8_t tmp_counter;


void this(OnReceive())
{
	#if this(debug) == 2
  		if (rf_sync_state == SYNC_STATE_SYNCED)
  			PCLedRedOn();
  		else PCLedRedOff();
  	#endif

  if ( !just_synced() && ACLDataIsNewNow())
  {
    uint8_t* data;
    data=(uint8_t *) ACLGetReceivedData(ACL_TYPE_ALPHA_ARG('M','S','P'));
    if (data && ACLGetDataLength(data)==6)
    {
      uint16_t t3;
      int i=0;
      this(dutycycle_length)=data[i]<<8;
       this(dutycycle_length)+=data[i+1];
      i+=2;
      this(sleep_length)=data[i]<<8;
      this(sleep_length)+=data[i+1];
      i+=2;
      t3 = data[i]<<8;
      t3 += data[i+1];

     {
        int16_t diff;
        uint16_t max_diff;
        diff=get_timer3();
        diff-=t3;

        max_diff=(this(dutycycle_length)-this(sleep_length))/2;

        if(abs(diff)>max_diff)
         this(majorSync)=1;
	  }

        set_timer3(t3);//assert that this happens at all nodes at the same time

      reset_synced();
    }


  }
  ACLACKRun();

  //TODO: Alarm
}

void this(Init())
{
  WDTConfig(WDT_RATE);
  ClockTimerInit(CLOCK_RATE);
  ACLACKInit();
  this(startup)=0;

  // setup and start timer 3 (RTC)
  bit_set( PIC_T1CON, 3 );		//TIMER1 oszillator enable (is for timer3 as well)

  PIC_T3CON = 0b01110111;		//timer3: 2*8bit acces,1:8 prescale,no external sync, input from timer 1 oszi, enable timer
  								//(one step = 244,1us = 1/4096s)
  // configure interrupts
  bit_clear(PIC_RCON, 7);			// interrupt priorities (IPEN)
  bit_clear(PIC_IPR2, 1);			// timer3 priority bit -> high priority (TMR3P)
  bit_set(PIC_INTCON, 6);			// enable peripheral interrupts (PEIE/GIEL)

  {
    uint16_t t3;
    t3=0-this(dutycycle_length);
    set_timer3(t3);
  }

#if this(debug) == 1
   PCLedBlueOn();
#endif

#ifdef WATCHDOG
  restart_wdt();
  this(restart_wdt)=16;
  WDTEnable();
#endif

  ACLStart();

  /* wakup, preparing sensors etc... */
#if this(debug) == 3
  AppSetLEDBehaviour(LEDS_ON_RECEIVE);
#endif
}


/*
#int_timer3
void inttimer3()
{
	PCLedRedOn();
	PCLedBlueOn();
	delay_ms(100);
}
*/
