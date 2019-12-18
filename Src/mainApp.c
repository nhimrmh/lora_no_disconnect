
#include "mainApp.h"
#include "Lora_transmission_HAL.h"
#include "string.h"
#include "SW_Timer.h"
#include "logging.h"
#include "Slave.h"
#include "Master.h"
#include "stdio.h"
#include "usbd_cdc_if.h"
#include "stdlib.h"
LoraMode myLoraMode;
u8 free_ptr = 0;

u8 Tx_Packet_delay[100];
u8 store_packet[PACKET_LENGTH];
u8 store_id[NUMBER_OF_SLAVES];    
//u8 tx_b[PACKET_LENGTH];
//u8 Tx_Packet_b[PRINTUSB_LENGTH];
u8 tx_u[PACKET_LENGTH];
u8 Tx_Packet_u[PRINTUSB_LENGTH];

void mainApp()
{
	//u16 i=0;//,j,k=0,g;
        
	sx1276_7_8_Reset();
		
	/*
        First initialization for Lora 
	*/
        
        for(uint32_t i = 1; i < 100; i++){
          myLoraMaster.status_prev[i] = 0;
        }
        
	Lora_1278_Init();
	sx1276_7_8_Config_Init();                                         //setting base parameter

	Switch_To_Rx();	
	
        /*
	Choose mode
	*/ 
        myLoraSlave.slave_id = "3"; //Id of slave
	myLoraMode.mode = SLAVE_RX; //Mode 1: Slave, 3:Master  
        //myLoraMode.mode = MASTER_TX;
        myLoraSlave.slave_mode = UNICAST;
	myLoraMode.slave_count = RESET_VALUE;
        myLoraPtr.current_ptr = RESET_VALUE;
        myLoraMode.flag_timer = TIMER_RESET;
        myLoraMaster.sent = NOT_SENT_YET;
        myLoraSlave.receive_T = 0;
        myLoraSlave.receive_S = 0;
        myLoraMaster.delay_count = 0;
        myLoraMaster.broad_count = 0;        
        
	while (1)
	{		
		switch(myLoraMode.mode)
		{
			case SLAVE_TX://lora slave Tx
                        {	
                          
                          Slave_Send_Response(myLoraSlave.slave_mode, myLoraSlave.slave_id);
                        }		
			break;
				
			case SLAVE_RX://lora slave Rx continuous
			{	
                          
                          Slave_Receive_Data(myLoraSlave.slave_id);
                          
                        }		
			break;	
                        
                        case MASTER_TX://lora master Tx
                        {
                          //HAL_Delay(1200);
                          Master_Send_Data();   
                        }		
			break;
				
			case MASTER_RX://lora master Rx continuous
			{	                                                   
                          Master_Receive_Data();     
                        }		
			break;	
                        
                        case 5:
                        {
                          for(uint32_t i = 1; i < 100; i++){
                            if(myLoraMaster.status_prev[i] == 1 && myLoraMaster.status[i] == 0){
                              sprintf(myLoraMode.strBuf,"Node 1.%d disconnected\n", i);				
                              printUSB(myLoraMode.strBuf);	                                                                                          
                            }
                          }
                          for(uint32_t i = 1; i < 100; i++){
                            myLoraMaster.status_prev[i] = myLoraMaster.status[i];
                          }                                                    
                          myLoraMode.mode = MASTER_TX; 
                        }
                        break;
                        
                        case 6:
                        {
                          //if(myLoraMaster.uni_received == 0){

                            //myLoraMaster.uni_received = 1;
                            sprintf(myLoraMode.strBuf,"Node 1.%s disconnected\n", myLoraSlave.slave_id);				
                            printUSB(myLoraMode.strBuf);	
                            myLoraMode.mode = 3;
                          //}
                        }
                        break;
                        
                        case 7:     
                          myLoraMode.flag_timer = 0;
                          if(atoi(myLoraSlave.slave_id) == 1){
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == 0){
                                myLoraMaster.sent = 0;
                            }
                          }
                          if(atoi(myLoraSlave.slave_id) == 2){
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == 0){
                                myLoraMaster.sent = 0;
                            }
                          }
                          if(atoi(myLoraSlave.slave_id) == 3){
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 0){
                                myLoraMaster.sent = 0;
                            }
                          }
                          if(atoi(myLoraSlave.slave_id) == 4){
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 0){
                                myLoraMaster.sent = 0;
                            }
                          }
                          if(myLoraMaster.sent == 0){
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1 || 
                               HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == 1 || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == 1
                               || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 1 || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 1){
                              myLoraMode.mode = 3;
                            }
                          }
                        break;
                        
                case 8:
                  {
                    u8 done_packet[20];
                    u8 temp_print[50];
                    myLoraMode.flag_timer = TIMER_RESET;
                    SW_TIMER_CLEAR(SW_TIMER1);
                    
                    for(u8 idx = 0; idx < delay_position; idx++){  
                      Switch_To_Tx();
                      u8 tx_delay[PACKET_LENGTH];
                      sprintf((char*)tx_delay,"S%s_%d&%d", myLoraSlave.slave_id, (idx + 1)*DELAY_TIME, myLoraMaster.delay[idx]);
                      Send_Tx_Packet((u8*)tx_delay, PACKET_LENGTH);		                                                
                      sprintf((char*)temp_print,"Set delay: %s\n", (char*)tx_delay);
                      printUSB((char*)temp_print); 
                      HAL_Delay(100000);
                      //sprintf((char*)Tx_Packet_delay, "Set delay 1 second to 1.1\n");														
                      //sprintf((char*)Tx_Packet_delay, "Set delay %d second to 1.%d\n", (idx + 1)*(DELAY_TIME/100000), myLoraMaster.delay[idx]);														
                      //printUSB((char*)Tx_Packet_delay);    																										                      
                    }
                    Switch_To_Tx();   
                    myLoraSlave.receive_T = 0;
                    myLoraSlave.receive_S = 0;
                    HAL_Delay(300000);
                    sprintf((char*)done_packet, "D_%d", myLoraMaster.delay_count++);
                    Send_Tx_Packet((u8*)done_packet, PACKET_LENGTH);	
                    printUSB((char*)done_packet);    																										
                    if(strncmp(myLoraSlave.slave_id, "0", 1) == 0) myLoraMode.mode = MASTER_TX;
                    else {
                      myLoraMode.mode = SLAVE_RX;
                      Switch_To_Rx();	
                    }
                  }
                  break;
	}
      }
}
