#ifndef _LOGGING_H
#define _LOGGING_H

#include "My_type.h"

extern void print_data(char* data, u8 len_transmit);
extern void timer_measure_start(void);
extern u32 timer_measure_stop(void);

typedef struct
{
  u8 Data[20];
}TxPacket;

typedef struct
{
  u8 current_ptr;
}LoraPtr;


typedef enum
{
  TXSTARTDONE = 0,
  TXDONE,
  RXSTARTDONE,
  GETACK
}LoraTest;


typedef struct
{
  u8 status;
  u16 timeTxconf;
  u16 timeTxdone;
  u16 timeCycle;
  u16 timeRxconf;
  u16 timeRxdone;  
}LoraTransmit;

extern TxPacket myTxPacket;
extern LoraPtr myLoraPtr;
extern LoraTransmit LoraTime;
#endif


