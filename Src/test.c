#include "stdio.h"
#include "stdlib.h"
#include "test.h"
#include "Lora_transmission_HAL.h"

u8 Txd[64];
void EntryAndSend(void)
{
  u8 addr;
  Lora_1278_BaseParameter();
  SPIWrite(REG_LR_PADAC,0x87);                                   
  SPIWrite(LR_RegHopPeriod,0x00);	
  SPIWrite(REG_LR_DIOMAPPING1,0x41);                       
  SPIWrite(LR_RegIrqFlags,0xFF);
  SPIWrite(LR_RegIrqFlagsMask,0xF7);                       
  SPIWrite(LR_RegPayloadLength,20);                      
  addr = SPIRead(LR_RegFifoTxBaseAddr);           
  SPIWrite(LR_RegFifoAddrPtr,addr);                        
  BurstWrite(Lora_1278_RegFifo, (u8 *)Txd, strlen((char*)Txd));
  SPIWrite(Lora_1278_RegOpMode,0x8b);                    //Tx Mode
  while(!Get_NIRQ())
  {
  }	
  SPIWrite(LR_RegIrqFlags,0xFF);
  SPIWrite(Lora_1278_RegOpMode,0x89);
}

