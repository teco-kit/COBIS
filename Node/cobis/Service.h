/* Static Service Dispatching for CoBIs 
 * configurable by ../Services.tab
 * Author: Till Riedel
 */
#ifndef __SERVICE_H__
#define __SERVICE_H__ 1
#define Y(X)X
#define CAT(A,B) Y(A)/**/_/**/B

#define INC(X) #X

#ifdef OWN_GOOD_TYPE
#include INC(config/OWN_GOOD_TYPE)
#endif

enum
{
#define SVC(X)  Y(X)/**/ServiceID,
#include "../Services.tab"
#undef SVC
	MAX_SERVICES
};

signed char Services[MAX_SERVICES]=
{
#define SVC(X)  -1,
#include "../Services.tab"
#undef SVC
};

unsigned char ServiceACLs[MAX_SERVICES];

unsigned char ServiceACL_buffer[64];
unsigned char ServiceACL_max=0;


#define ServiceACL(X) (&ServiceACL_buffer[ServiceACLs[X]])

void ServiceListInit();
void ServicesWakeup();
void ServicesSleep();
void ServicesSend();

#define ServiceGetServiceLength(S) (3+(S[2]))

unsigned char ServiceGetService(char* service)
{
	int i,j;
	for(i=0;i<MAX_SERVICES;i++)
	{
		char *s;
		s=ServiceACL(i);

		if(s[0]==service[0]&&s[1]==service[1]&&s[2]==service[2])
			for(j=3;j<ServiceGetServiceLength(service);j++)
				if(s[j]!=service[j])break;
		if(j==3+service[2]) break;
	}
	return i;
}

boolean  _ServiceDataIsNew(unsigned char id)
{
  if(ACLAdressedDataIsNew())
  {
    char *s;
    s=ServiceACL(id);

    if(
        s[0]==LL_payload_received[0]&&
        s[1]==LL_payload_received[1]&&
        s[2]==LL_payload_received[2]
      )
    {
      int j;
      for(j=0;j<LL_payload_received[2];j++)
      {
        if( s[3+j]!=LL_payload_received[3+j]) return 0;
      }

      return 1;
    }
    else
      return 0;

  }
  else
  { return 0;}
}

#define ServiceGetRate(X) Services[X]
char ServiceSetRate(unsigned char id,signed char rate)
{
  if((rate>=0) && Services[id]<0)
  {
    int i;
    ACLStop();
    CateyeClipLEDs(1);
    for ( i = 0; i < 5; i++)
    {
      CateyeClipLEDs(3);
      DelayMs(1000);
      CateyeClipLEDs(2);
      DelayMs(1000);
      CateyeClipLEDs(3);
      DelayMs(1000);
      CateyeClipLEDs(1);
      DelayMs(1000);
    }
    CateyeClipLEDs(0);
    ACLStart();
  }

  if((rate>0) && Services[id]<=0)
  {
    char *s;
    s=ServiceACL(id);

    ACLSubscribe(s[0],s[1]);
  }

  if((rate<=0) && (Services[id]>0))
  {
    char *s;
    s=ServiceACL(id);
    ACLUnsubscribe(s[0],s[1]);
  }

  Services[id]=rate;
}

char _ServiceRegister(unsigned char id)
{
  char *s;
  Services[id]=(signed char)0;
}

#define ServiceSet(X,RATE) ServiceSetRate(Y(X)/**/ServiceID,RATE)

#define ServiceStop(X) ServiceSet(X,0)

#define ServiceRegister(X) do{_ServiceRegister(Y(X)/**/ServiceID);Y(X)/**/_Init();}while(0)

#define ServiceStart(X) ServiceSet(X,Y(X)/**/_DefaultRate)

#define ServiceUnRegister(X) ServiceSet(X,-1)

#define ServiceIsRunning(X) (Services[Y(X)/**/ServiceID]>0)

#define ServiceServe(X) if(Services[Y(X)/**/ServiceID]>0) Y(X)/**/_Server()

#define ServiceRun(X) if(Services[Y(X)/**/ServiceID])\
  Y(X)/**/_OnReceive()

#define ServiceWakeup(X) if(Services[Y(X)/**/ServiceID])\
  Y(X)/**/_OnWakeup()

#define ServiceSleep(X) if(Services[Y(X)/**/ServiceID])\
  Y(X)/**/_OnSleep()

#define ServiceSend(X) if(Services[Y(X)/**/ServiceID])\
  Y(X)/**/_OnSend()

//#define ServiceRun(X) if(Services[Y(X)/**/ServiceID]>0) Y(X)/**/ServiceRun()

#define ServiceAddSubject(X)\
	ACLAddNewType((ServiceACL(Y(X)/**/ServiceID))[0],(ServiceACL(Y(X)/**/ServiceID))[1])

#define ServiceDataIsNew(X) _ServiceDataIsNew(Y(X)/**/ServiceID)

#define ServiceDispatch(A,B,C,F) (data=ACLGetReceivedData(ACL_TYPE_ALPHA_ARG(A,B,C)))&&(F(data))

#define SVC(X) #include INC(../Services/Y(X)/**/Service.c)
#include "services.tab"
#undef SVC

#define ServiceSetACL(X) \
  do{ \
    int i,length;\
    ServiceACLs[Y(X)/**/ServiceID]=ServiceACL_max; \
    length=Y(X)/**/_ACL[2]+3; \
    for(i=0;i<length;i++)\
	  ServiceACL_buffer[ServiceACL_max++]=Y(X)/**/_ACL[i];\
  }while(0)

void ServiceListInit() 
{
#define SVC(X)  ServiceSetACL(X);
#include "Services.tab"
#undef SVC
}

void ServicesRun()
{
#define SVC(X)  ServiceRun(X);
#include "../Services.tab"
#undef SVC
}

void ServicesInit()
{
	ServiceListInit();
#define SVC(X)  ServiceRegister(X);
#include "../Services.tab"
#undef SVC
}

void ServicesSleep()
{
#define SVC(X)  ServiceSleep(X);
#include "../Services.tab"
#undef SVC
}

void ServicesWakeup()
{
#define SVC(X)  ServiceWakeup(X);
#include "../Services.tab"
#undef SVC
}

void ServicesSend()
{
#define SVC(X)  ServiceSend(X);
#include "../Services.tab"
#undef SVC
}

void ServicesStart()
{
#define SVC(X)  ServiceStart(X);
#include "../Services.tab"
#undef SVC
}

void ServicesServe()
{
#define SVC(X)  ServiceServe(X);
#include "../Services.tab"
#undef SVC
}

#endif
