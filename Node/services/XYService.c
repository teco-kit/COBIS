#define this(F) CAT(XY,F)

#include <cobis/util.h>

const unsigned int this(DefaultRate)=1;

char this(ACL)[]={ACL_TYPE_ALPHA_ARG('S','X','Y'),0};


void this(Server())
{
  int send=0;


  if(ServiceDataIsNew(XY))
  {
    char *data;

    ServiceAddSubject(XY);
    send|=ServiceDispatch('C','V','X',this(method));

    if(send)
    {
      while(ACLSendingBusy());
      ACLSendPacket(20);
    }
    else
      ACLClearSendData();

  }
}

void this(OnSleep()){}

void this(OnWakeup()){}

void this(OnReceive()){}
