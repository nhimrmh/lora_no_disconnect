#include "Lora_transmission_HAL.h"
#include "stdio.h"
#include "stdlib.h"
#include "logging.h"
#include "string.h"

u8 RxData[64];
u8 Txdata[64] = {"01234567890123456789"};
/**
  * @brief  	Standby mode
  * @param  	None
  * @retval 	None
  */
void sx1276_7_8_Standby(void)
{
  SPIWrite(Lora_1278_RegOpMode,LORA_STDBY);//Standby//Low Frequency Mode
}
/**
  * @brief  	Sleep mode
  * @param  	None
  * @retval 	None
  */
void sx1276_7_8_Sleep(void)
{
  SPIWrite(Lora_1278_RegOpMode,LORA_SLEEP); //Sleep//Low Frequency Mode
}
/**
  * @brief  	Entry Lora mode
  * @param  	None
  * @retval 	None
  */
void sx1276_7_8_EntryLoRa(void)
{
  SPIWrite(Lora_1278_RegOpMode,LORA_ON);//Low Frequency Mode
	
}
/**
  * @brief  	Reset Lora_SX1278
  * @param  	None
  * @retval 	None
  */
void Reset_LoraModule(void)
{
    HAL_GPIO_WritePin(Reset_GPIO_Port,Reset_Pin,GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(Reset_GPIO_Port,Reset_Pin,GPIO_PIN_SET);
}
/**
  * @brief  	Invert IQ signal
  * @param  	True or false
  * @retval 	None
  */
void LR_invertIQ(bool invert)
{
  SPIWrite(Lora_1278_RegInvertIQ , invert == FALSE ? INVERT_NORMAL : INVERT_IQ);   //invertIQ signal
}
/**
  * @brief  	Clear IrqFlags
  * @param  	None
  * @retval 	None
  */
void sx1276_7_8_LoRaClearIrq(void)
{
  SPIWrite(Lora_1278_RegIrqFlags,IRQFLAG_CLEAR);
}
/**
  * @brief  	Read Rx Packet RSSI
  * @param  	None
  * @retval 	Rx packet RSSI
  */
s8 sx1276_7_8_LoRaReadRSSI_Rx(void)
{
  s8 temp=10;
  s8 snr = 0;
  temp=SPIRead(Lora_1278_RegPktRssiValue); 
  snr =SPIRead(Lora_1278_RegPktSnrValue);
  snr = snr>>2;
  if(snr > 0)
  {
          temp=temp*16/15+-164;    
  }else if(snr < 0)
  {
          temp=temp +-164+ snr*1/4;
  }else
          temp=temp-164;
  return (s8)temp;
}
/**
  * @brief  	Read SNR
  * @param  	None
  * @retval 	SNR
  */
s8 sx1276_7_8_LoRaReadSNR(void)
{
  s8 temp=10;
  temp=SPIRead(Lora_1278_RegPktSnrValue);   
  temp=temp>>2;                                       
  return (s8)temp;
}
/**
  * @brief  	Config and Entry Rx continous mode
  * @param  	None
  * @retval 	OK or not
  */
u8 sx1276_7_8_LoRaEntryRx(void)
{
  u8 addr;  
  Lora_1278_BaseParameter();
  SPIWrite(Lora_1278_RegPaDac,PADAC_NORMAL);                                    //Normal and Rx
  SPIWrite(Lora_1278_RegHopPeriod,0xFF);                                //RegHopPeriod NO FHSS
  SPIWrite(Lora_1278_RegDioMapping1,DIOMAPPING1_RX);                            //DIO0=00, DIO1=00, DIO2=00, DIO3=01
  SPIWrite(Lora_1278_RegIrqFlagsMask,0x3F);                                     //Open RxDone interrupt & Timeout
  sx1276_7_8_LoRaClearIrq();
  SPIWrite(Lora_1278_RegPayloadLength,NUM_PAYLOAD);                             //RegPayloadLength  20byte (this register must difine when the data long of one byte in SF is 6)
  addr = SPIRead(Lora_1278_RegFifoRxBaseAddr);           	                //Read RxBaseAddr
  SPIWrite(Lora_1278_RegFifoAddrPtr,addr);                                      //RxBaseAddr -> FiFoAddrPtr¡¡
  SPIWrite(Lora_1278_RegOpMode,LORA_RXCONTINUOUS);                             	//Continuous Rx Mode//Low Frequency Mode
  while(1)
  {
      if((SPIRead(Lora_1278_RegModemStat)&0x04)==0x04)                          //Rx-on going RegModemStat
        {
            Lora.status = RX;
            break;	
        }
  }
  return 0;
}
/**
  * @brief  	Indicate Rx done
  * @param  	None
  * @retval 	Yes or No
  */
u8 Indicate_Rx_Comming(void)
{
	return HAL_GPIO_ReadPin(nIrq_GPIO_Port,nIrq_Pin);
}
/**
  * @brief  	Read Data recieved
  * @param  	None
  * @retval 	packet size
  */
u8 Read_packet(void)
{
    u8 i;
    u8 addr;
    u8 packet_size;
    for(i=0;i<32;i++)
      RxData[i] = 0x00;
    addr = SPIRead(Lora_1278_RegFifoRxCurrentAddr);                             //last packet addr
    SPIWrite(Lora_1278_RegFifoAddrPtr,addr);                                    //RxBaseAddr -> FiFoAddrPtr
    if(Lora.SpreadFactor == SF_6)           
        packet_size=20;
    else
        packet_size = SPIRead(Lora_1278_RegRxNbBytes);     
    SPIBurstRead(Lora_1278_RegFifo, RxData, packet_size);
    sx1276_7_8_LoRaClearIrq();
    
    return packet_size;
}
/**
  * @brief  	Entry Tx mode
  * @param  	None
  * @retval 	Yes or No
  */
u8 sx1276_7_8_LoRaEntryTx(void)
{
  u8 addr;
  Lora_1278_BaseParameter();
  SPIWrite(Lora_1278_RegPaDac,PADAC_HIGH);                                      //Tx for 20dBm
  SPIWrite(Lora_1278_RegHopPeriod,0x00);	                                //RegHopPeriod NO FHSS
  SPIWrite(Lora_1278_RegDioMapping1,DIOMAPPING1_TX);                            //DIO0=01, DIO1=00, DIO2=00, DIO3=01
  SPIWrite(Lora_1278_RegIrqFlagsMask,0xF7);                                     //Open TxDone interrupt
  sx1276_7_8_LoRaClearIrq();
  SPIWrite(Lora_1278_RegPayloadLength,NUM_PAYLOAD);                             //RegPayloadLength  21byte
  addr = SPIRead(Lora_1278_RegFifoTxBaseAddr);                                  //RegFiFoTxBaseAddr
  SPIWrite(Lora_1278_RegFifoAddrPtr,addr);                                      //RegFifoAddrPtr
  return 0;
}
/**
  * @brief  	Send data over
  * @param  	None
  * @retval 	True or false
  */
bool Send_Tx_Packet(u8* buf, u8 length)
{       
        bool TxDone = FALSE; 
	if(strlen((char*)buf) <= length){
          BurstWrite(Lora_1278_RegFifo, buf, length);
          SPIWrite(Lora_1278_RegOpMode,LORA_TX);                    //Tx Mode
          Wait_Tx_Done();
          TxDone = TRUE;
	}
	else{
          TxDone = FALSE;
        }
        return TxDone;
}
/**
  * @brief  	Wait Tx done transmit
  * @param  	None
  * @retval 	None
  */
void Wait_Tx_Done(void)
{       
	while(!Get_NIRQ())
	{
	}	
	sx1276_7_8_LoRaClearIrq();
}

/**
  * @brief  	Init Lora base parameter
  * @param  	None
  * @retval 	None
  */

void sx1276_7_8_Config_Init(void)
{
	sx1276_7_8_Sleep();                                                     //Change modem mode Must in Sleep mode
        HAL_Delay(10);
	sx1276_7_8_EntryLoRa();
        BurstWrite(Lora_1278_RegFrMsb,(u8*)SX1278_Frequency[Lora.Frequency],3);
        SPIWrite(Lora_1278_RegPaConfig,Lora.OutputPower);             
	SPIWrite(Lora_1278_RegOcp,OCP_DISABLE);                              
	SPIWrite(Lora_1278_RegLna,LNA_G1);                                      //RegLNA,High & LNA Enable
	if(Lora.SpreadFactor == SF_6)                                              //SFactor=6
	{
            SPIWrite(Lora_1278_RegModemConfig1,((Lora.BandWidth<<4)+(Lora.CodingRate<<1)+IMPLICIT_MODE));
            SPIWrite(Lora_1278_RegModemConfig2,((Lora.SpreadFactor<<4)+(Lora.CRCheck<<2)+Lora.RxTimeOut.TimeOutMsb));
            SPIWrite(Lora_1278_RegDetectOptimize,DETECTION_OPTIMIZE_SF6);
            SPIWrite(Lora_1278_RegDetectionThreshold,DETECTION_THRESHOLD_SF6);
	}
	else
	{
            SPIWrite(Lora_1278_RegModemConfig1,((Lora.BandWidth<<4)+(Lora.CodingRate<<1)+EXPLICIT_MODE));
            SPIWrite(Lora_1278_RegModemConfig2,((Lora.SpreadFactor<<4)+(Lora.CRCheck<<2)+Lora.RxTimeOut.TimeOutMsb));  
	}
	SPIWrite(Lora_1278_RegSymbTimeoutLsb,Lora.RxTimeOut.TimeOutLsb);        //RegSymbTimeoutLsb Timeout = 0x3FF(Max)
	SPIWrite(Lora_1278_RegPreambleMsb,PREAMBLE_MSB);                        //RegPreambleMsb
	SPIWrite(Lora_1278_RegPreambleLsb,PREAMBLE_LSB);                        //RegPreambleLsb 8+4=12byte Preamble
	SPIWrite(Lora_1278_RegDioMapping2,DIOMAPPING2_MODE);                    //RegDioMapping2 DIO5=00, DIO4=01
        sx1276_7_8_Standby();                                                   //Entry standby mode
}

Lora_HandleTypeDef Lora;
/**
  * @brief  	Input parameter for Lora
  * @param  	None
  * @retval 	None
  */
void Lora_1278_Init(void)
{
  Lora.Frequency = LORA_433MHZ;
  Lora.OutputPower = OP_20_dBm;
  Lora.SpreadFactor = SF_6;
  Lora.BandWidth = BW_500_kHz;
  Lora.CodingRate = CR_4_5;
  Lora.CRCheck = CRC_ENABLE; 
  Lora.RxTimeOut.TimeOutMsb = RX_TIMEOUT_MSB;
  Lora.RxTimeOut.TimeOutLsb = RX_TIMEOUT_LSB;
}

void Lora_1278_BaseParameter(void)
{
	sx1276_7_8_Sleep();                                                     //Change modem mode Must in Sleep mode
	sx1276_7_8_EntryLoRa();
        BurstWrite(Lora_1278_RegFrMsb,(u8*)SX1278_Frequency[Lora.Frequency],3);
        SPIWrite(Lora_1278_RegPaConfig,Lora.OutputPower);             
	SPIWrite(Lora_1278_RegOcp,OCP_DISABLE);                              
	SPIWrite(Lora_1278_RegLna,LNA_G1);                                      //RegLNA,High & LNA Enable
	if(Lora.SpreadFactor == SF_6)                                              //SFactor=6
	{
            SPIWrite(Lora_1278_RegModemConfig1,((Lora.BandWidth<<4)+(Lora.CodingRate<<1)+IMPLICIT_MODE));
            SPIWrite(Lora_1278_RegModemConfig2,((Lora.SpreadFactor<<4)+(Lora.CRCheck<<2)+Lora.RxTimeOut.TimeOutMsb));
            SPIWrite(Lora_1278_RegDetectOptimize,DETECTION_OPTIMIZE_SF6);
            SPIWrite(Lora_1278_RegDetectionThreshold,DETECTION_THRESHOLD_SF6);
	}
	else
	{
            SPIWrite(Lora_1278_RegModemConfig1,((Lora.BandWidth<<4)+(Lora.CodingRate<<1)+EXPLICIT_MODE));
            SPIWrite(Lora_1278_RegModemConfig2,((Lora.SpreadFactor<<4)+(Lora.CRCheck<<2)+Lora.RxTimeOut.TimeOutMsb));  
	}
	SPIWrite(Lora_1278_RegSymbTimeoutLsb,Lora.RxTimeOut.TimeOutLsb);        //RegSymbTimeoutLsb Timeout = 0x3FF(Max)
	SPIWrite(Lora_1278_RegPreambleMsb,PREAMBLE_MSB);                        //RegPreambleMsb
	SPIWrite(Lora_1278_RegPreambleLsb,PREAMBLE_LSB);                        //RegPreambleLsb 8+4=12byte Preamble
	SPIWrite(Lora_1278_RegDioMapping2,DIOMAPPING2_MODE);                    //RegDioMapping2 DIO5=00, DIO4=01
        sx1276_7_8_Standby();          
}

