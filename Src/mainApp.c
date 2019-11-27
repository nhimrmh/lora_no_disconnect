/*
 * mainApp.c
 *
 *  Created on: Sep 8, 2017
 *      Author: dkhairnar
 */

#include "mainApp.h"
#include "stdio.h"
#include "stdlib.h"
#include "Lora_transmission_HAL.h"
#include "string.h"
#include "logging.h"
#include "SW_Timer.h"
#include "test.h"
uint32_t count =0 ;

char strBuf[64];
char Buff[64];
char strBuf1[64];
u8 strBuf_1[64] = "\n";
u8 testx[64];
u16 SysTime;
u16 time2_count;
u16 key1_time_count;
u16 key2_time_count;
u8 rf_rx_packet_length;

u8 mode;//lora--1/FSK--0
u8 Freq_Sel;//
u8 Power_Sel;//
u8 Lora_Rate_Sel;//
u8 BandWide_Sel;//
uint32_t flag_clk = 0;
u8 Fsk_Rate_Sel;//
u8 key1_count;
u8 time_flag;
u8 operation_flag;
u8 key_flag;


void mainApp()
{
	Reset_LoraModule();
        Lora_1278_Init();
	sx1276_7_8_Config_Init();
	sx1276_7_8_LoRaEntryRx();
	key1_count = 0;
        u16 e = 0;
	while (1)
	{
		switch(key1_count)
		{
			case 0://lora Tx	
				sprintf((char*) Txd, "2x_back");
                                timer_measure_start();
                                EntryAndSend();
				e = timer_measure_stop();
				sprintf(strBuf1,"Tx_sent_%d",e);
				print_data(strBuf1,strlen(strBuf1));
			
				sx1276_7_8_LoRaEntryRx();
				key1_count =1;
				
			break;
			case 1://lora Rx countinous
                                if(Indicate_Rx_Comming())
				{
                                    u8 packet;
                                    packet = Read_packet();
                                    sprintf(strBuf,"Rx_%d_%s",packet,(char*) RxData);
                                    print_data(strBuf,strlen(strBuf)-13);
                                    key1_count = 0;
				}
			break;
			case 2: //lora Rx single

			break;
			case 3:
			break;
			default:
				

			break;
		}
	}
}
