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

uint32_t count =0 ;

char strBuf[64];
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
u8	operation_flag;
u8 key_flag;


void mainApp()
{
	//u16 i=0;//,j,k=0,g;

	SysTime = 0;
	operation_flag = 0x00;
	key1_count = 0x00;
	mode = 0x01;//lora mode
	Freq_Sel = 0x00;//433M
	Power_Sel = 0x00;//
	Lora_Rate_Sel = 0x00; // 6,7,8,9,10,11,12 -Spread factor
												// 0,1,2,3,4 ,5 ,6 
	BandWide_Sel = 0x09;  //7.8KHz,10.4KHz,15.6KHz,20.8KHz,31.2KHz,41.7KHz,62.5KHz,125KHz,250KHz,500KHz
												//0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09
	Fsk_Rate_Sel = 0x00;

	Reset_LoraModule();
	sx1276_7_8_Config_Init();
	sx1276_7_8_LoRaEntryRx();
        
       /* SW_TIMER_CALLBACK func1;
        func1 = fun1;
        SW_TIMER_CREATE_FunCallBack(SW_TIMER1,1000,func1);
        SW_TIMER_START(SW_TIMER1);
        */
	key1_count = 0;
	u8 RData[64];
        u16 s = 0;
        u16 e = 0;
	while (1)
	{
		switch(key1_count)
		{
			case 0://lora Tx	
				sprintf((char*) Txdata, "2x_back");
				s = timer_measure_start();
				sx1276_7_8_LoRaEntryTx();
				Send_Tx_Packet();
				Wait_Tx_Done();
				e = timer_measure_stop();
				sprintf(strBuf1,"Tx_sent_%d",e-s);
				print_data(strBuf1,strlen(strBuf1));
			
				//rs = timer_measure_start();
				sx1276_7_8_LoRaEntryRx();
				key1_count =0;
				
			break;
			case 1://lora Rx countinous
				/*if( sx1276_7_8_LoRaRxPacket())
					{
						re = timer_measure_stop();
						if( strncmp( (char*) RxData, "2", 1)==0)
						{
							sprintf(strBuf,"Rx_%s",(char*) RxData);
							print_data(strBuf,strlen(strBuf));
							
							//key1_count = 0;
						}
					}*/
				if(Indicate_Rx_Comming())
				{
					u8 packet;
					packet = Read_packet();
					sprintf(strBuf,"Rx_%s_%d",(char*) RxData,packet);
					print_data(strBuf,strlen(strBuf));
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
