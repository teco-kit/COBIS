#ifndef __COBIS_UTIL_H
#define __COBIS_UTIL_H 1


typedef enum _sendstates
{
  SENSE_WAIT,
  SENSE_START,
  SENSE_SEND,
  SENSE_SENT
} sendstates;


static void rmemcpy(char* dst, char* src,char len )
{
  char i=0;
  while(len--)
    dst[i++]=src[len];
}

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned long uint16_t;
typedef signed long int16_t;
#endif
