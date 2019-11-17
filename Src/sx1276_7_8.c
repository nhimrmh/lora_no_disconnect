#include "sx1276_7_8.h"
#include "stdio.h"
#include "stdlib.h"
extern uint32_t t2;

const u8 sx1276_7_8FreqTbl[1][3] =
{
  {0x6C, 0x80, 0x00}, //434MHz
};

const u8 sx1276_7_8PowerTbl[4] =
{
  0xFF,                   //20dbm
  0xFC,                   //17dbm
  0xF9,                   //14dbm
  0xF6,                   //11dbm
};

const u8 sx1276_7_8SpreadFactorTbl[7] =
{
  6,7,8,9,10,11,12
};

const u8 sx1276_7_8LoRaBwTbl[10] =
{
//7.8KHz,10.4KHz,15.6KHz,20.8KHz,31.2KHz,41.7KHz,62.5KHz,125KHz,250KHz,500KHz
  0,1,2,3,4,5,6,7,8,9
};
u8 RxData[64];
u8 Txdata[64] = {"01234567890123456789"};
u8 RxAddr;

uint32_t t2;
/**********************************************************
**Variable define
**********************************************************/

void sx1276_7_8_Config(void);

/**********************************************************
**Name:     sx1276_7_8_Standby
**Function: Entry standby mode
**Input:    None
**Output:   None
**********************************************************/
void sx1276_7_8_Standby(void)
{
  SPIWrite(LR_RegOpMode,0x09);                              		//Standby//Low Frequency Mode
	//SPIWrite(LR_RegOpMode,0x01);                              	 //Standby//High Frequency Mode
}

/**********************************************************
**Name:     sx1276_7_8_Sleep
**Function: Entry sleep mode
**Input:    None
**Output:   None
**********************************************************/
void sx1276_7_8_Sleep(void)
{
  SPIWrite(LR_RegOpMode,0x08);                              		//Sleep//Low Frequency Mode
	//SPIWrite(LR_RegOpMode,0x00);                            		 //Sleep//High Frequency Mode
}

/*********************************************************/
//LoRa mode
/*********************************************************/
/**********************************************************
**Name:     sx1276_7_8_EntryLoRa
**Function: Entry LoRa(LongRange) mode
**Input:    None
**Output:   None
**********************************************************/
void sx1276_7_8_EntryLoRa(void)
{
  SPIWrite(LR_RegOpMode,0x88);//Low Frequency Mode
	//SPIWrite(LR_RegOpMode,0x80);//High Frequency Mode
}


void LR_invertIQ(u8 invert)
{
  SPIWrite(LR_RegInvertIQ , invert == FALSE ? 0x27 : 0x67);  
}

/**********************************************************
**Name:     sx1276_7_8_LoRaClearIrq
**Function: Clear all irq
**Input:    None
**Output:   None
**********************************************************/
void sx1276_7_8_LoRaClearIrq(void)
{
  SPIWrite(LR_RegIrqFlags,0xFF);
}

/**********************************************************
**Name:     sx1276_7_8_LoRaEntryRx
**Function: Entry Rx mode
**Input:    None
**Output:   None
**********************************************************/
u8 sx1276_7_8_LoRaEntryRx(void)
{
  SPIWrite(REG_LR_PADAC,0x84);     //highpower setting for PA 0x87      //Normal and Rx
  SPIWrite(LR_RegHopPeriod,0xFF);                          //RegHopPeriod NO FHSS
  SPIWrite(REG_LR_DIOMAPPING1,0x01);                       //DIO0=00, DIO1=00, DIO2=00, DIO3=01

	sx1276_7_8_LoRaClearIrq();
  SPIWrite(LR_RegIrqFlagsMask,0x3F);                       //Open RxDone interrupt & Timeout
  //SPIWrite(0x30,1<<7);
  SPIWrite(LR_RegPayloadLength,20);                       //RegPayloadLength  21byte(this register must difine when the data long of one byte in SF is 6)
  SPIWrite(LR_RegOpMode,0x8d);                        		//Continuous Rx Mode//Low Frequency Mode
	//SPIWrite(LR_RegOpMode,0x05);                        		//Continuous Rx Mode//High Frequency Mode
	SysTime = 0;
	while(1)
	{
		if((SPIRead(LR_RegModemStat)&0x04)==0x04)   //Rx-on going RegModemStat
			break;
		if(SysTime>=3)
			return 0;                                              //over time for error
	}
	return 0;
}

u8 sx1276_7_8_LoRaEntryRx_single(void)
{
	sx1276_7_8_Config();  
	SPIWrite(REG_LR_PADAC,0x84);     //highpower setting for PA 0x87      //Normal and Rx
  SPIWrite(LR_RegHopPeriod,0xFF);                          //RegHopPeriod NO FHSS
  SPIWrite(REG_LR_DIOMAPPING1,0x01);                       //DIO0=00, DIO1=00, DIO2=00, DIO3=01

	sx1276_7_8_LoRaClearIrq();
  SPIWrite(LR_RegIrqFlagsMask,0x3F); 
	SPIWrite(LR_RegOpMode, 0x8e); // RX single LF
	
	return 0;
}
/**********************************************************
**Name:     sx1276_7_8_LoRaReadRSSI
**Function: Read the RSSI value
**Input:    none
**Output:   temp, RSSI value
**********************************************************/
u8 sx1276_7_8_LoRaReadRSSI(void)
{
  u16 temp=10;
  temp=SPIRead(LR_RegRssiValue);                  //Read RegRssiValue£¬Rssi value
  temp=temp+127-137;                                       //127:Max RSSI, 137:RSSI offset
  return (u8)temp;
}

s8 sx1276_7_8_LoRaReadRSSI_test(void)
{
  s8 temp=10;
  temp=SPIRead(LR_RegRssiValue);                  
  temp=temp+-164;                                       
  return (s8)temp;
}

s8 sx1276_7_8_LoRaReadRSSI_Rx(void)
{
  s8 temp=10;
	s8 snr = 0;
  temp=SPIRead(LR_RegPktRssiValue); 
	snr =SPIRead(LR_RegPktSnrValue);
	snr = snr>>2;
	if(snr > 0)
	{
		temp=temp*16/15+-164;    
	}else if(snr < 0)
	{
		temp=temp +-164+ snr*0.25;
	}else
		temp=temp-164;
  return (s8)temp;
}

s8 sx1276_7_8_LoRaReadSNR(void)
{
  s8 temp=10;
  temp=SPIRead(LR_RegPktSnrValue);   
	temp=temp>>2;                                       
  return (s8)temp;
}
/**********************************************************
**Name:     sx1276_7_8_LoRaRxPacket
**Function: Receive data in LoRa mode
**Input:    None
**Output:   1- Success
            0- Fail
**********************************************************/
u8 sx1276_7_8_LoRaRxPacket(void)
{
  u8 i;
  u8 addr;
  u8 packet_size;

  if(Get_NIRQ())
  {
    for(i=0;i<32;i++)
      RxData[i] = 0x00;

    addr = SPIRead(LR_RegFifoRxCurrentaddr);      //last packet addr
    SPIWrite(LR_RegFifoAddrPtr,addr);                      //RxBaseAddr -> FiFoAddrPtr
    if(sx1276_7_8SpreadFactorTbl[Lora_Rate_Sel]==6)           //When SpreadFactor is six£¬will used Implicit Header mode(Excluding internal packet length)
      packet_size=20;
    else
      packet_size = SPIRead(LR_RegRxNbBytes);     //Number for received bytes
    SPIBurstRead(0x00, RxData, packet_size);
		RxAddr = SPIRead(LR_RegFifoRxByteAddr);
    sx1276_7_8_LoRaClearIrq();

  }
  else
    return(0);
  return(1);
}


u8 sx1276_7_8_LoRaRxPacket_single(void)
{
	u8 i;
  u8 addr;
  u8 packet_size;
	if(Get_RxTimeOut())
	{
		sx1276_7_8_LoRaClearIrq();
		sx1276_7_8_Standby();
		return 0;
	}
  else if(Get_NIRQ())
	{
		sx1276_7_8_Standby();
		
		for(i=0;i<32;i++)
			RxData[i] = 0x00;

    addr = SPIRead(LR_RegFifoRxCurrentaddr);      //last packet addr
    SPIWrite(LR_RegFifoAddrPtr,addr);                      //RxBaseAddr -> FiFoAddrPtr
    if(sx1276_7_8SpreadFactorTbl[Lora_Rate_Sel]==6)           //When SpreadFactor is six£¬will used Implicit Header mode(Excluding internal packet length)
      packet_size=23;
    else
      packet_size = SPIRead(LR_RegRxNbBytes);     //Number for received bytes
    SPIBurstRead(0x00, RxData, packet_size);
		
    sx1276_7_8_LoRaClearIrq();
		return(1);
	}
	else{
		return(0);
	}
}
/**********************************************************
**Name:     sx1276_7_8_LoRaEntryTx
**Function: Entry Tx mode
**Input:    None
**Output:   None
**********************************************************/
u8 sx1276_7_8_LoRaEntryTx(void)
{
  u8 addr,temp;

	//sx1276_7_8_Config();                                         //setting base parameter

  SPIWrite(REG_LR_PADAC,0x87);                                   //Tx for 20dBm
  SPIWrite(LR_RegHopPeriod,0x00);                               //RegHopPeriod NO FHSS
  SPIWrite(REG_LR_DIOMAPPING1,0x41);                       //DIO0=01, DIO1=00, DIO2=00, DIO3=01

  sx1276_7_8_LoRaClearIrq();
  SPIWrite(LR_RegIrqFlagsMask,0xF7);                       //Open TxDone interrupt
  SPIWrite(LR_RegPayloadLength,20);                       //RegPayloadLength  21byte

  addr = SPIRead(LR_RegFifoTxBaseAddr);           //RegFiFoTxBaseAddr
  SPIWrite(LR_RegFifoAddrPtr,addr);                        //RegFifoAddrPtr
	SysTime = 0;
	while(1)
	{
		temp=SPIRead(LR_RegPayloadLength);
		if(temp==20)
		{
			break;
		}
		if(SysTime>=3)
			return 0;
	}
	return 0;
}
/**********************************************************
**Name:     sx1276_7_8_LoRaTxPacket
**Function: Send data in LoRa mode
**Input:    None
**Output:   1- Send over
**********************************************************/
u8 sx1276_7_8_LoRaTxPacket(void)
{
	BurstWrite(0x00, (u8 *)Txdata, strlen((char*)Txdata));
	SPIWrite(LR_RegOpMode,0x8b);                    //Tx Mode
	while(1)
	{
		if(Get_NIRQ())                      //Packet send over
		{
			t2 = HAL_GetTick();
			SPIRead(LR_RegIrqFlags);
			sx1276_7_8_LoRaClearIrq();                                //Clear irq

			sx1276_7_8_Standby();                                     //Entry Standby mode

			break;
		}
	}
	return 0;
}

/**********************************************************
**Name:     sx1276_7_8_Config
**Function: sx1276_7_8 base config
**Input:    mode
**Output:   None
**********************************************************/
void sx1276_7_8_Config(void)
{
  sx1276_7_8_Sleep();                                      //Change modem mode Must in Sleep mode
    //NOP();
	HAL_Delay(10);

  //lora mode
	sx1276_7_8_EntryLoRa();
	//SPIWrite(0x5904);   //?? Change digital regulator form 1.6V to 1.47V: see errata note

	BurstWrite(LR_RegFrMsb,sx1276_7_8FreqTbl[Freq_Sel],3);  //setting frequency parameter

	//setting base parameter
	SPIWrite(LR_RegPaConfig,sx1276_7_8PowerTbl[Power_Sel]);             //Setting output power parameter

	SPIWrite(LR_RegOcp,0x0B);                              //RegOcp,Close Ocp
	SPIWrite(LR_RegLna,0x23);                              //RegLNA,High & LNA Enable

	if(sx1276_7_8SpreadFactorTbl[Lora_Rate_Sel]==6)           //SFactor=6
	{
		u8 tmp;
		SPIWrite(LR_RegModemConfig1,((sx1276_7_8LoRaBwTbl[BandWide_Sel]<<4)+(CR<<1)+0x01));//Implicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)
		SPIWrite(LR_RegModemConfig2,((sx1276_7_8SpreadFactorTbl[Lora_Rate_Sel]<<4)+(SPI_CRC<<2)+0x03));

		tmp = SPIRead(0x31);
		tmp &= 0xF8;
		tmp |= 0x05;
		SPIWrite(0x31,tmp);
		SPIWrite(0x37,0x0C);
	}
	else
	{
		SPIWrite(LR_RegModemConfig1,((sx1276_7_8LoRaBwTbl[BandWide_Sel]<<4)+(CR<<1)+0x00));//Explicit Enable CRC Enable(0x02) & Error Coding rate 4/5(0x01), 4/6(0x02), 4/7(0x03), 4/8(0x04)
		SPIWrite(LR_RegModemConfig2,((sx1276_7_8SpreadFactorTbl[Lora_Rate_Sel]<<4)+(SPI_CRC<<2)+0x03));  //SFactor &  LNA gain set by the internal AGC loop
	}
	SPIWrite(LR_RegSymbTimeoutLsb,0xFF);                   //RegSymbTimeoutLsb Timeout = 0x3FF(Max)

	SPIWrite(LR_RegPreambleMsb,0x00);                       //RegPreambleMsb
	SPIWrite(LR_RegPreambleLsb,12);                      //RegPreambleLsb 8+4=12byte Preamble

	SPIWrite(REG_LR_DIOMAPPING2,0x01);                     //RegDioMapping2 DIO5=00, DIO4=01

  sx1276_7_8_Standby();                                         //Entry standby mode
}
