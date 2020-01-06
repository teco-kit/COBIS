#undef this
#define this(F) CAT(Monitoring,F)

#define Monitoring_Service


#include <cobis/util.h>
#include <cobis/Method.h>



#ifndef Monitoring_debug
#define Monitoring_debug 0
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

char this(ACL)[]={ACL_TYPE_ALPHA_ARG('S','M','S'),0};


uint16_t this(notificationInterval)=6;
uint16_t this(sampleInterval)=5;

Setter(notificationInterval)

uint16_t  this(notifyCounter)=0;
uint16_t  this(sampleCounter)=0;

int16_t this(voltage);
int16_t this(voltage_sum);

sendstates this(sendState)=SENSE_WAIT;

void this(OnSleep())
{

  if(this(sendState)==SENSE_WAIT && 0==this(sampleCounter))
  {
    this(sampleCounter)=this(notificationInterval);

    ADCEnable();

    this(sendState)=SENSE_START;
  }
  else
    this(sampleCounter)--;
}

uint8_t this(alarm)=0;

uint8_t this(d)=0;

void this(OnWakeup())
{
  
#if this(debug)==1
  this(d)++;
  if(this(d)&1)
    PCLedBlueOn();
  else
    PCLedBlueOff();

  if(this(notifyCounter)&1)
    PCLedRedOn();
  else
    PCLedRedOff();
#endif



  if(this(sendState)==SENSE_START)
  {
    

      VoltageSensorPrepare();
      VoltageSensorGet(&this(voltage));
      this(voltage_sum)+=this(voltage);
      ADCDisable();

      if(this(notifyCounter))
      {
        this(notifyCounter)--;
      }

      if(0==this(notifyCounter))
      {
        this(notifyCounter)=this(notificationInterval);
        this(sendState)=SENSE_SEND;
      }
      else
        this(sendState)=SENSE_WAIT;

  }

  if(this(voltage)<1200)
  {
    this(alarm)=1;
  }
  else
    this(alarm)=0;
}


void this(Server())
{
  int send=0;

  ACLLockReceiveBuffer();

  if(ServiceDataIsNew(Monitoring))
  {
    char *data;

    ServiceAddSubject(Monitoring);
    send|=ServiceDispatch('C','N','I',SetVar(notificationInterval));

    ACLSetDataToOld();
    ACLReleaseReceiveBuffer();

    if(send)
    {
      while(ACLSendingBusy()) Network_DelaySlots(1);
      Network_SendPacket(20);
    }
    else
      ACLClearSendData();
  }
  else
    ACLReleaseReceiveBuffer();


  if(this(sendState)==SENSE_SEND)
  {
    uint16_t avg_volt;
    avg_volt=this(voltage_sum)/this(notificationInterval);

    ServiceAddSubject(Monitoring);
    ACLAddNewType(ACL_TYPE_ALPHA_ARG('S','V','C'));
    ACLAddData(avg_volt>>8);
    ACLAddData(avg_volt&0xff);
    Network_SendPacket(1);
    this(sendState)=SENSE_SENT;
  }

}

void this(OnSend()){}

  void this(OnReceive()){
    if(this(sendState)==SENSE_SENT)
    {
      if(ACLGetSendSuccess())
      {
        this(voltage_sum)=0;
        this(sendState)=SENSE_WAIT;
      }
      else
        this(sendState)=SENSE_SEND;
    }
  }

void this(Init()){}
