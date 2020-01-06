#undef this
#define this(F) CAT(HazardousGoods,F)

#include <cobis/util.h>
#include <cobis/Method.h>
#include <string.h>


#define array_cpy(A,a,B,b,n) do{uint8_t _x; for(_x=0;_x<(n);_x++)A[(a)+_x]=B[(b)+_x];}while(0)

const unsigned int this(DefaultRate)=1;

char this(ACL)[]={ACL_TYPE_ALPHA_ARG('S','H','G'),0};

enum this(goodtype) {NOTHING,COLA,PEPSI};

uint16_t this(storageLimit)=this(STORAGE_LIMIT);
Setter(storageLimit)

int8_t this(maxTemp)=this(MAX_TEMP);
Setter(maxTemp)

int8_t this(minTemp)=this(MIN_TEMP);
Setter(minTemp)

uint8_t this(upperPrecission)=1;
Setter(upperPrecission)

uint8_t this(lowerPrecission)=1;
Setter(lowerPrecission)

uint16_t this(unknownStorageLocationTimeLimit)=this(MAX_UNKNOWN_LOCATION);
Setter(unknownStorageLocationTimeLimit)

uint16_t this(alertRepeatInterval)=4;
Setter(alertRepeatInterval)

uint16_t this(notificationRepeatInterval)=20;
Setter(notificationRepeatInterval)

uint16_t this(drumVolume)=20;
Setter(drumVolume)

uint8_t this(chemical)=this(CHEMICAL);
Setter(chemical)

uint8_t this(chemicalClass)=this(CHEMICAL_CLASS);
Setter(chemicalClass)

uint8_t this(proximityFieldStrength)=4;
Setter(proximityFieldStrength)

char this(alarm)=0;
char this(alarm_local)=0;
char this(oldalarm)=0;
uint16_t this(lastLocationChange)=0;

uint16_t this(storedVol);

uint8_t this(dbg[16]);
uint8_t this(dbg_len)=0;
#define HazardousGoods_BURST_NUM 20
uint8_t this(burst)=0;

#define SET_INCOMPATIBILITY_COUNTER 5

typedef enum
{
 storageLimit,
 incompatibility,
 maxTemperature,
 minTemperature,
 unknownLocation,
 numAlarmType
}alarmType;

uint8_t this(hasAlarm(alarmType a))
{
  if( bit_test(this(alarm),a))
    return 1;

  return 0;
}
uint8_t this(hasLocalAlarm(alarmType a))
{
  if( this(hasAlarm(a)) && bit_test(this(alarm_local),a))
    return 1;

  return 0;
}

void this(setAlarm(alarmType a))
{
  bit_set(this(alarm),a);
}

void this(setLocalAlarm(alarmType a))
{
  this(setAlarm(a));
  bit_set(this(alarm_local),a);
}

void this(clearAlarm(alarmType a))
{
  bit_clear(this(alarm),a);
  bit_clear(this(alarm_local),a);
}


long this(alarmACL[])=
{
  ACL_TYPE_ALPHA('A','S','L'),
  ACL_TYPE_ALPHA('A','I','G'),
  ACL_TYPE_ALPHA('A','T','H'),
  ACL_TYPE_ALPHA('A','T','L'),
  ACL_TYPE_ALPHA('A','U','L'),
};


int this(sendDbg(void))
{
  if(this(dbg_len))
  {
    int i;
    ACLAddNewType(ACL_TYPE_ALPHA_ARG('D','B','G'));
    for(i=0;i<this(dbg_len);i++)
      ACLAddData(this(dbg)[i]);
    this(dbg_len)=0;
  }
}

int this(sendContent(void))
{
  ACLAddNewType(ACL_TYPE_ALPHA_ARG('P','C','H'));
  ACLAddData(this(chemical));

  ACLAddNewType(ACL_TYPE_ALPHA_ARG('P','C','C'));
  ACLAddData(this(chemicalClass));

  ACLAddNewType(ACL_TYPE_ALPHA_ARG('P','D','V'));
  ACLAddData(this(drumVolume)>>8);
  ACLAddData(this(drumVolume));

  if(this(storageLimit))
  {
	  ACLAddNewType(ACL_TYPE_ALPHA_ARG('S','D','V'));
	  ACLAddData(this(storedVol)>>8);
	  ACLAddData(this(storedVol));
  }
}

int this(sendAlarm(void))
{
  int i;

  this(oldalarm)=this(alarm);
  for(i=0;i<numAlarmType;i++)
    if(this(hasLocalAlarm(i)))
      ACLAddNewType(hi(this(alarmACL[i])),lo(this(alarmACL[i])));
  return 0!=this(alarm);
}

int this(checkRemoteAlarm(void))
{
  int i;
    char *data;
    if ( ACLDataIsNewNow()
        &&(data=ACLGetReceivedData(ACL_TYPE_ALPHA_ARG('S','L','C')))
        &&ACLGetDataLength(data)==1
        &&data[0]==Location_location) //in same location
      for(i=0;i<numAlarmType;i++)
       if(ACLGetReceivedData(hi(this(alarmACL[i])),lo(this(alarmACL[i])))) this(setAlarm(i));
}

uint16_t this(alarmSuppress)=0;

void this(OnSend())
{
      this(sendAlarm());
      this(sendContent());
}


void this(Server())
{
  int send=0;



  ACLLockReceiveBuffer();
  if(ServiceDataIsNew(HazardousGoods))
  {
    char *data;
    char send_cc=0;

    ServiceAddSubject(HazardousGoods);

    send|=ServiceDispatch('C','S','L',SetVar(storageLimit));

    send|=ServiceDispatch('C','T','H',SetVar(maxTemp));
    send|=ServiceDispatch('C','T','L',SetVar(minTemp));

    send|=ServiceDispatch('C','P','H',SetVar(upperPrecission));
    send|=ServiceDispatch('C','P','L',SetVar(lowerPrecission));

    send|=ServiceDispatch('C','U','L',SetVar(unknownStorageLocationTimeLimit));
    send|=ServiceDispatch('C','A','R',SetVar(alertRepeatInterval));
    send|=ServiceDispatch('C','N','R',SetVar(notificationRepeatInterval));
    send|=ServiceDispatch('C','D','V',SetVar(drumVolume));
    send|=ServiceDispatch('C','C','H',SetVar(chemical));
    send|=ServiceDispatch('C','F','S',SetVar(proximityFieldStrength));

    send|=ServiceDispatch('C','C','C',SetVar(chemicalClass));

    ACLSetDataToOld();
    ACLReleaseReceiveBuffer();

    if(send)
    {
      Network_SendPacket(20);
    }
    else
      ACLClearSendData();
  }
  else
    ACLReleaseReceiveBuffer();

  if(!send)
  {
    if(this(burst))
    {
      this(burst)--;
      ServiceAddSubject(HazardousGoods);
      Network_SendPacket(20);
    }
    else if( !ACLSendingBusy() && (
             (this(alarm) != this(oldalarm)) ||  
             (this(alarm) && !(rf_random_h%this(alertRepeatInterval))) || 
             (!this(alarm) && !(rf_random_h%this(notificationRepeatInterval)))
              )) 
    {
      ServiceAddSubject(HazardousGoods);
      Network_SendPacket(1);
    }
  }

}

void this(checkTemperature(void))
{
  if (this(hasAlarm(maxTemperature)))
  {
    if(Sensor_temperature+this(upperPrecission) < (this(maxTemp)))
      this(clearAlarm(maxTemperature));
  }
  else
  {
    if(Sensor_temperature > (this(maxTemp)))
      this(setLocalAlarm(maxTemperature));
  }

  if(this(hasAlarm(minTemperature)))
  {
    if(Sensor_temperature > (this(minTemp)+this(lowerPrecission)))
      this(clearAlarm(minTemperature));
  }
  else
  {
    if(Sensor_temperature < (this(minTemp)))
      this(setLocalAlarm(minTemperature));
  }
}


uint8_t this(incompatibilityCounter)=0;



void this(checkIncompatibility())
{
  //if(1||Location_location)
  {
    char *data;
    if ((data=ACLGetReceivedData(ACL_TYPE_ALPHA_ARG('S','L','C')))
        &&ACLGetDataLength(data)==1
        &&data[0]==Location_location) //in same location
    {

      if(data=ACLGetReceivedData(ACL_TYPE_ALPHA_ARG('P','C','C'))
          &&ACLGetDataLength(data)==1)
      {
        uint8_t r,rdc,l,ldc;


//COMPILER BUG!!!
        data=ACLGetReceivedData(ACL_TYPE_ALPHA_ARG('P','C','C'));

      //ACLAddNewType(ACL_TYPE_ALPHA_ARG('D','B','G'));
      //  ACLAddData(*ACLGetReceivedData(ACL_TYPE_ALPHA_ARG('P','C','C')));
      //  ACLAddData(*data);

        r=data[0]&0x0F;
       // ACLAddData(r);
        rdc=data[0]>>4;
        //ACLAddData(rdc);


        l=this(chemicalClass)&0x0F;
        //ACLAddData(l);
        ldc=this(chemicalClass)>>4;
        //ACLAddData(ldc);

        //or ther other's values
        r|=l&rdc;
       // ACLAddData(r);

        l|=r&ldc;
        //ACLAddData(l);


        //ACLSendPacket(20);

        if(l != r)
        {
          this(setLocalAlarm(incompatibility));
          this(incompatibilityCounter)=SET_INCOMPATIBILITY_COUNTER;
        }

      }
    }

  }
}

void this(timeoutIncompatibility())
{
  if(!this(incompatibilityCounter))
  {
    this(clearAlarm(incompatibility));
  }
  else
    this(incompatibilityCounter)--;
}

long this(unknownStorageLocationCounter)=0xffff;

void  this(checkUnknownstorageLocation())
{
  if(Location_location || 0==this(unknownStorageLocationTimeLimit))
  {
    if (0==this(unknownStorageLocationTimeLimit))
	    	this(unknownStorageLocationCounter)=0xFFFF;
    else this(unknownStorageLocationCounter)=this(unknownStorageLocationTimeLimit);
    this(clearAlarm(unknownLocation));
  }

  if(0==this(unknownStorageLocationCounter))
    this(setLocalAlarm(unknownLocation));
  else
    this(unknownStorageLocationCounter)--;
}

#define  MAX_VOL_TIMEOUT  60
#define MAX_DRUMS_IN_LOCATION 16

uint8_t this(storage)[MAX_DRUMS_IN_LOCATION][11];

void  this(resetStorage())
{
  uint8_t i;
  for(i=0;i<MAX_DRUMS_IN_LOCATION;i++)
    this(storage)[i][0]=0;
}

void  this(checkinStorage())
{

  uint8_t *data;

  data=ACLGetReceivedData(ACL_TYPE_ALPHA_ARG('S','L','C'));


  if (
      data
      && ACLGetDataLength(data)==sizeof(Location_location)
      && data[0]==Location_location //in same storage
     )
  {

    data=ACLGetReceivedData(ACL_TYPE_ALPHA_ARG('P','C','H'));

    if (
        data
        && ACLGetDataLength(data)==sizeof(this(chemical))
        && data[0]==this(chemical) // same chemical
       )

    {

      data=ACLGetReceivedData(ACL_TYPE_ALPHA_ARG('P','D','V'));
      if(
          data
          && ACLGetDataLength(data)==sizeof(this(drumVolume))
        )
      {
        uint8_t firstfree=MAX_DRUMS_IN_LOCATION;
        uint8_t hash,i,j;

        hash=ACLGetSrcId()[7]%MAX_DRUMS_IN_LOCATION;

        i=hash;
        do
        {
          if(0==this(storage)[i][0]) //if free
          {
            if(firstfree=MAX_DRUMS_IN_LOCATION) //mark first free
              firstfree=i;
          }
          else
          {
            int8_t j;
            for(j=7;j>=0;j--)
            {
              if(ACLGetSrcId()[j]!=this(storage)[i][3+j]) break;
            }

            if(j<0)
            {

              this(storage)[i][0]=MAX_VOL_TIMEOUT;
              this(storage)[i][1]=data[0];
              this(storage)[i][2]=data[1];
              array_cpy(this(storage)[i],3,ACLGetSrcId(),0,8);

              return;
            }
          }

          i=(i+1)%MAX_DRUMS_IN_LOCATION;
        }while(i!=hash);

        if(firstfree!=MAX_DRUMS_IN_LOCATION) //if it was not there and there is space
        {
          i=firstfree;


          this(storage)[i][0]=MAX_VOL_TIMEOUT;
          this(storage)[i][1]=data[0];
          this(storage)[i][2]=data[1];
          array_cpy(this(storage)[i],3,ACLGetSrcId(),0,8);

        }
      }
    }
  }
}

uint8_t this(oldLocation);

void  this(checkLocationChange())
{

  this(lastLocationChange)++;

  // if(Location_location) //don't care for (x->0)
  {
    if(this(oldLocation)!=Location_location)
    {
      this(resetStorage()); //was valid for old Location
      this(alarm)=0; //@@@@TR: is this OK?????

      this(lastLocationChange)=0;

      if(Location_location)
        ACLSetFieldStrength(32);
      else
        ACLSetFieldStrength(this(proximityFieldStrength)); //hack: use simple Proximity rule
    }
  }

  this(oldLocation)=Location_location;
}


void  this(checkStorageLimit())
{
  uint8_t i;

  if(this(storageLimit))
  {
    this(storedVol)=this(drumVolume);
    if(this(storedVol)>this(storageLimit))
    {
      this(setLocalAlarm(storageLimit));
      return;
    }

    for(i=0;i<MAX_DRUMS_IN_LOCATION;i++)
    {
      if(0!=this(storage)[i][0])
      {
        this(storage)[i][0]--;
        this(storedVol)+=this(storage)[i][1]<<8|this(storage)[i][2];
        if(this(storedVol)>this(storageLimit))
        {
          this(setLocalAlarm(storageLimit));
          return;
        }
      }
    }
  }


  this(clearAlarm(storageLimit));
  return;
}

uint8_t this(d)=0;

void this(OnWakeup())
{

  this(checkLocationChange());
  this(checkTemperature());
  this(checkStorageLimit());
  this(checkUnknownstorageLocation());
  this(timeoutIncompatibility());
  if(this(lastLocationChange<3) || Network_majorSync)
    this(burst)=this(BURST_NUM);
  else
    this(burst)=0;
}

void this(OnSleep())
{
}

void this(OnReceive())
{
  this(checkTemperature());

  if(ACLDataIsNewNow())
  {
    this(checkIncompatibility());
    this(checkinStorage());
    this(checkRemoteAlarm());
  }


  if(this(alarm)!=0){ Network_Led1Pattern=LED_PATTERN_FLASH1;}
  else {Network_Led1Pattern=LED_PATTERN_OFF;}

}

void this(Init())
{
  this(resetStorage)();
}
