#undef this
#define this(F) CAT(Sensor,F)

#define Sensor_Service

#include <cobis/util.h>
#include <cobis/Method.h>

const unsigned int this(DefaultRate)=1;

char this(ACL)[]={ACL_TYPE_ALPHA_ARG('S','S','S'),0};


uint16_t this(notificationInterval)=10;
Setter(notificationInterval)

uint16_t  this(notifyCounter)=0;

int8_t this(temperature)=25;

sendstates this(sendState)=SENSE_WAIT;

void this(OnSleep())
{
  if(this(sendState)==SENSE_WAIT && 0==this(notifyCounter))
  {
    this(notifyCounter)=this(notificationInterval);

    TemperatureSensorOn();

    this(sendState)=SENSE_START;
  }
  else
    this(notifyCounter)--;
}

void this(OnWakeup())
{

  if(this(sendState)==SENSE_START)
  {

    TemperatureSensorPrepare();
    TemperatureSensorGet(&this(temperature));
    TemperatureSensorOff();

    this(sendState)=SENSE_SEND;
  }

}


void this(Server())
{
  int send=0;

  ACLLockReceiveBuffer();

  if(ServiceDataIsNew(Sensor))
  {
    char *data;

    ServiceAddSubject(Sensor);
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
    ServiceAddSubject(Sensor);
    ACLAddNewType(ACL_TYPE_ALPHA_ARG('S','T','E'));
    ACLAddData(this(temperature));
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
      this(sendState)=SENSE_WAIT;
    }
    else
      this(sendState)=SENSE_SEND;
  }
}

void this(Init()){}
