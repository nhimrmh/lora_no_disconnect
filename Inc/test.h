#include "spi.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "My_type.h"


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
#define LR_RegInvertIQ		                    0x33
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

//SX1278 Internal registers Address
/********************Lora Registers***************************/
#define Lora_1278_RegFifo                                  0x00
#define Lora_1278_RegOpMode                                0x01
#define Lora_1278_RegFrMsb                                 0x06
#define Lora_1278_RegFrMid                                 0x07
#define Lora_1278_RegFrLsb                                 0x08
#define Lora_1278_RegPaConfig                              0x09
#define Lora_1278_RegPaRamp                                0x0A
#define Lora_1278_RegOcp                                   0x0B
#define Lora_1278_RegLna                                   0x0C
#define Lora_1278_RegFifoAddrPtr                           0x0D
#define Lora_1278_RegFifoTxBaseAddr                        0x0E
#define Lora_1278_RegFifoRxBaseAddr                        0x0F
#define Lora_1278_RegFifoRxCurrentAddr                     0x10
#define Lora_1278_RegIrqFlagsMask                          0x11
#define Lora_1278_RegIrqFlags                              0x12
#define Lora_1278_RegRxNbBytes                             0x13
#define Lora_1278_RegRxHeaderCntValueMsb                   0x14
#define Lora_1278_RegRxHeaderCntValueLsb                   0x15
#define Lora_1278_RegRxPacketCntValueMsb                   0x16
#define Lora_1278_RegRxPacketCntValueLsb                   0x17
#define Lora_1278_RegModemStat                             0x18
#define Lora_1278_RegPktSnrValue                           0x19
#define Lora_1278_RegPktRssiValue                          0x1A
#define Lora_1278_RegRssiValue                             0x1B
#define Lora_1278_RegHopChannel                            0x1C
#define Lora_1278_RegModemConfig1                          0x1D
#define Lora_1278_RegModemConfig2                          0x1E
#define Lora_1278_RegSymbTimeoutLsb                        0x1F
#define Lora_1278_RegPreambleMsb                           0x20
#define Lora_1278_RegPreambleLsb                           0x21
#define Lora_1278_RegPayloadLength                         0x22
#define Lora_1278_RegMaxPayloadLength                      0x23
#define Lora_1278_RegHopPeriod                             0x24
#define Lora_1278_RegFifoRxByteAddr                        0x25
#define Lora_1278_RegModemConfig3                          0x26
#define Lora_1278_RegFeiMsb                                0x28
#define Lora_1278_RegFeiMid                                0x29
#define Lora_1278_RegFeiLsb                                0x2A
#define Lora_1278_RegRssiWideband                          0x2C
#define Lora_1278_RegDetectOptimize                        0x31
#define Lora_1278_RegInvertIQ                              0x33
#define Lora_1278_RegDetectionThreshold                    0x37
#define Lora_1278_RegSyncWord                              0x39
// I/O settings
#define Lora_1278_RegDioMapping1                           0x40
#define Lora_1278_RegDioMapping2                           0x41
// Version
#define Lora_1278_RegVersion                               0x42
// Additional settings
#define Lora_1278_RegPllHop                                0x44
#define Lora_1278_RegTcxo                                  0x4B
#define Lora_1278_RegPaDac                                 0x4D
#define Lora_1278_RegFormerTemp                            0x5B
#define Lora_1278_RegAgcRef                                0x61
#define Lora_1278_RegAgcThresh1                            0x62
#define Lora_1278_RegAgcThresh2                            0x63
#define Lora_1278_RegAgcThresh3                            0x64
#define Lora_1278_RegPll                                   0x70
#define NUM_PAYLOAD                                        20

extern u8 Txd[64];

extern void EntryAndSend(void);
