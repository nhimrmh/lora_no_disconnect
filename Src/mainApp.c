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
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "string.h"
#define printUSB(x) CDC_Transmit_FS((uint8_t*)x,strlen((char*)x))

uint32_t count =0 ;
uint32_t clk_count =0 ;
uint32_t clk_count1 = 0;

char strBuf[64];
char strBuf1[64];
char strBuf_1[64];
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
	u16 i=0;//,j,k=0,g;

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

	RED_LED_L();
	HAL_Delay(5000);
	RED_LED_H();

	HAL_GPIO_WritePin(Reset_GPIO_Port,Reset_Pin,GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(Reset_GPIO_Port,Reset_Pin,GPIO_PIN_SET);

	sx1276_7_8_Config();//
	sx1276_7_8_LoRaEntryRx();
	key1_count = 5;


	while (1)
	{
		switch(key1_count)
		{
			case 0://lora master Tx	

			break;
			case 1://lora slaver Rx countinous
	
			break;
			case 2: //lora slaver Rx single

			break;
			case 3: // packet return from RX 

			break;
			case 5:
				
			break;
			default:
				

			break;
		}
	}
}
