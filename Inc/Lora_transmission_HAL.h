#ifndef SX1276_7_8_H_
#define SX1276_7_8_H_

#include "main.h"
#include "stm32f1xx_hal.h"
#include "My_type.h"
#include "spi.h"

/*****************IO define********************/
#define RED_LED_H()					HAL_GPIO_WritePin(RED_LED_GPIO_Port,RED_LED_Pin,GPIO_PIN_SET)//PD_ODR |= 0x40//PD6
#define RED_LED_L()					HAL_GPIO_WritePin(RED_LED_GPIO_Port,RED_LED_Pin,GPIO_PIN_RESET)//PD_ODR &= 0xbf

#define Get_KEY2()					(PD_IDR & 0x10) == 0x10//PD4
#define Get_KEY1()					(PD_IDR & 0x08) == 0x08//PD3

///////////////////////////////////////////////// LoRa mode //////////////////////////////////////////////////
//Error Coding rate (CR)setting
#define CR_4_5
#ifdef   CR_4_5
  #define CR    0x01                                       // 4/5
#else
  #ifdef   CR_4_6
    #define CR    0x02                                     // 4/6
  #else
    #ifdef   CR_4_7
      #define CR    0x03                                   // 4/7
    #else
      #ifdef   CR_4_8
        #define CR    0x04                                 // 4/8
      #endif
    #endif
  #endif
#endif

//CRC Enable
#define SPI_CRC_EN

#ifdef  SPI_CRC_EN
  #define SPI_CRC   0x01                                       //CRC Enable
#else
  #define SPI_CRC   0x00
#endif

//RFM98 Internal registers Address
/********************Lroa mode***************************/
#define LR_RegFifo                                  0x00
// Common settings
#define LR_RegOpMode                                0x01
#define LR_RegFrMsb                                 0x06
#define LR_RegFrMid                                 0x07
#define LR_RegFrLsb                                 0x08
// Tx settings
#define LR_RegPaConfig                              0x09
#define LR_RegPaRamp                                0x0A
#define LR_RegOcp                                   0x0B
// Rx settings
#define LR_RegLna                                   0x0C
// LoRa registers
#define LR_RegFifoAddrPtr                           0x0D
#define LR_RegFifoTxBaseAddr                        0x0E
#define LR_RegFifoRxBaseAddr                        0x0F
#define LR_RegFifoRxCurrentaddr                     0x10
#define LR_RegIrqFlagsMask                          0x11
#define LR_RegIrqFlags                              0x12
#define LR_RegRxNbBytes                             0x13
#define LR_RegRxHeaderCntValueMsb                   0x14
#define LR_RegRxHeaderCntValueLsb                   0x15
#define LR_RegRxPacketCntValueMsb                   0x16
#define LR_RegRxPacketCntValueLsb                   0x17
#define LR_RegModemStat                             0x18
#define LR_RegPktSnrValue                           0x19
#define LR_RegPktRssiValue                          0x1A
#define LR_RegRssiValue                             0x1B
#define LR_RegHopChannel                            0x1C
#define LR_RegModemConfig1                          0x1D
#define LR_RegModemConfig2                          0x1E
#define LR_RegSymbTimeoutLsb                        0x1F
#define LR_RegPreambleMsb                           0x20
#define LR_RegPreambleLsb                           0x21
#define LR_RegPayloadLength                         0x22
#define LR_RegMaxPayloadLength                      0x23
#define LR_RegHopPeriod                             0x24
#define LR_RegFifoRxByteAddr                        0x25
#define LR_RegInvertIQ															0x33
// I/O settings
#define REG_LR_DIOMAPPING1                          0x40
#define REG_LR_DIOMAPPING2                          0x41
// Version
#define REG_LR_VERSION                              0x42
// Additional settings
#define REG_LR_PLLHOP                               0x44
#define REG_LR_TCXO                                 0x4B
#define REG_LR_PADAC                                0x4D
#define REG_LR_FORMERTEMP                           0x5B

#define REG_LR_AGCREF                               0x61
#define REG_LR_AGCTHRESH1                           0x62
#define REG_LR_AGCTHRESH2                           0x63
#define REG_LR_AGCTHRESH3                           0x64

/*********************************************************/
//command
/*********************************************************/
extern u8 key_flag;
extern u8 mode;
extern u8 Freq_Sel;
extern u8 Power_Sel;
extern u8 Lora_Rate_Sel;
extern u8 BandWide_Sel;
extern u8 Fsk_Rate_Sel;
extern u16 SysTime;
extern u8 Txdata[64];
extern u8 RxAddr;
extern u8 status;
extern void delay_ms(unsigned int ms);
extern void delay_us(unsigned int us);


extern u8 RxData[64];
extern uint32_t t1,t2;
extern void sx1276_7_8_Config_Init(void);
extern void sx1276_7_8_Config_Init_2(void);
extern void Reset_LoraModule(void);
extern void LR_invertIQ(u8 invert);
extern u8 Indicate_Rx_Comming(void);
extern u8 Send_Tx_Packet(void);
extern void Wait_Tx_Done(void);
extern u8 sx1276_7_8_LoRaEntryRx_single(void);
extern u8 sx1276_7_8_LoRaEntryRx(void);
extern u8 Read_packet(void);
extern s8 sx1276_7_8_LoRaReadRSSI_test(void);
extern s8 sx1276_7_8_LoRaReadRSSI_Rx(void);
extern s8 sx1276_7_8_LoRaReadSNR(void);
extern u8 sx1276_7_8_LoRaRxPacket_single(void);
extern u8 sx1276_7_8_LoRaRxPacket(void);
extern u8 sx1276_7_8_LoRaEntryTx(void);
extern u8 sx1276_7_8_LoRaTxPacket(void);
extern void sx1276_7_8_Standby(void);    
#endif /* SX1276_7_8_H_ */
