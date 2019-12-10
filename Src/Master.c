#include "My_type.h"
#include "mainApp.h"
#include "main.h"
#include "Master.h"
#include "Slave.h"
#include "SW_Timer.h"
#include "Lora_transmission_HAL.h"
#include "logging.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "usbd_cdc_if.h"
LoraMaster myLoraMaster;
u32 count_success = 0;
u32 count_total = 0;
u8 time = 0, time1 = 0;
u32 packet_count = 0;
/**********************************************************
**Name:     Send_Broadcast_Data
**Function: Send_Broadcast_Data
**Input:    data to send in CHAR*
**Output:   none
**********************************************************/
void Send_Broadcast_Data(char* data){  
  u8 tx_b[PACKET_LENGTH];
  u8 Tx_Packet_b[PACKET_LENGTH];
  HAL_Delay(TIME_BETWEEN_DATA_SENT);                            			
  sprintf((char*)tx_b,"%s\n", data);		
  sprintf((char*)Tx_Packet_b, "Data sent: all\n\n");														
  printUSB((char*)Tx_Packet_b);
  Switch_To_Tx();																											
  Send_Tx_Packet((u8*)tx_b, PACKET_LENGTH);																								
  Switch_To_Rx();																			
  myLoraMode.mode = MASTER_RX;	
  myLoraMode.uni_or_broad = BROADCAST;
  for(u8 i = 1; i < 100; i++){
        myLoraMaster.status[i] = 0;
  }
}

/**********************************************************
**Name:     Send_Unicast_Data
**Function: Send_Unicast_Data
**Input:    data to send in CHAR*
**Output:   none
**********************************************************/
/*void Send_Unicast_Data(){
  u8 tx_u[PACKET_LENGTH];
  u8 Tx_Packet_u[PRINTUSB_LENGTH];
  myLoraMaster.uni_received = NOT_RECEIVED_YET;
  myLoraMaster.uni_sent = NOT_SENT_YET;
  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == 1){
    myLoraSlave.slave_id = "1";
    myLoraMode.uni_or_broad = UNICAST;
  }
  else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == 1){
    myLoraSlave.slave_id = "2";
    myLoraMode.uni_or_broad = UNICAST;
  }
  else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 1){
    myLoraSlave.slave_id = "3";
    myLoraMode.uni_or_broad = UNICAST;
  }
  else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 1){
    myLoraSlave.slave_id = "4";
    myLoraMode.uni_or_broad = UNICAST;
  }				
  sprintf((char*)tx_u,"%s", myLoraSlave.slave_id);		
  sprintf((char*)Tx_Packet_u, "Data sent: Unicast to 1.%s\n\n", (char*)tx_u);														
  printUSB((char*)Tx_Packet_u);
  Switch_To_Tx();																											
  Send_Tx_Packet((u8*)tx_u, PACKET_LENGTH);																								
  Switch_To_Rx();																			
  myLoraMode.mode = MASTER_RX;
  myLoraMaster.sent = ALREADY_SENT;
}*/

void Send_Unicast_Data(){
  u8 tx_u[PACKET_LENGTH];
  u8 Tx_Packet_u[PRINTUSB_LENGTH];
  myLoraMaster.uni_received = NOT_RECEIVED_YET;
  myLoraMaster.uni_sent = NOT_SENT_YET;
  if(strncmp((char*)received_USB,"1",1) == 0){
    myLoraSlave.slave_id = "1";
    myLoraMode.uni_or_broad = UNICAST;
  }
  else if(strncmp((char*)received_USB,"2",1) == 0){
    myLoraSlave.slave_id = "2";
    myLoraMode.uni_or_broad = UNICAST;
  }
  else if(strncmp((char*)received_USB,"3",1) == 0){
    myLoraSlave.slave_id = "3";
    myLoraMode.uni_or_broad = UNICAST;
  }
  else if(strncmp((char*)received_USB,"4",1) == 0){
    myLoraSlave.slave_id = "4";
    myLoraMode.uni_or_broad = UNICAST;
  }
  
  for(u8 i = 0; i <8 ; i++){
      received_USB[i] = 0;
  }
  sprintf((char*)tx_u,"%s", myLoraSlave.slave_id);		
  sprintf((char*)Tx_Packet_u, "Data sent: Unicast to 1.%s\n\n", (char*)tx_u);														
  printUSB((char*)Tx_Packet_u);
  Switch_To_Tx();																											
  Send_Tx_Packet((u8*)tx_u, PACKET_LENGTH);																								
  Switch_To_Rx();																			
  myLoraMode.mode = MASTER_RX;
  myLoraMaster.sent = ALREADY_SENT;
}



void Send_Unicast_Data_Test(){
  
  time = HAL_GetTick();
  
  u8 tx_u[PACKET_LENGTH];
  u8 Tx_Packet_u[PRINTUSB_LENGTH];
  myLoraSlave.slave_id = "2";	
  sprintf((char*)tx_u,"%s_%d", myLoraSlave.slave_id, packet_count++);		
  sprintf((char*)Tx_Packet_u, "Rxdone: %d, Unicast number: %d\n\n" ,LoraTime.timeRxdone, count_total++);														
  printUSB((char*)Tx_Packet_u);

  Switch_To_Tx();																											
  Send_Tx_Packet((u8*)tx_u, PACKET_LENGTH);
  
  Switch_To_Rx();																			
  myLoraMode.mode = MASTER_RX;
}

/**********************************************************
**Name:     Master_Send_Data
**Function: Master_Send_Data
**Input:    none
**Output:   none
**********************************************************/
/*void Master_Send_Data(){
  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1){
    Send_Broadcast_Data("abb");
  }
  else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == 1 || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == 1 
          || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 1 || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 1){
    if(myLoraMaster.sent == NOT_SENT_YET){
      Send_Unicast_Data();
    }
  }      
}*/

void Master_Send_Data(){
  if(strncmp((char*)received_USB,"broad",5) == 0){
    for(u8 i = 0; i <8 ; i++){
        received_USB[i] = 0;
    }
    Send_Broadcast_Data("abb");
  }
  else if(strncmp((char*)received_USB,"1",1) == 0 || strncmp((char*)received_USB,"2",1) == 0 
          || strncmp((char*)received_USB,"3",1) == 0 || strncmp((char*)received_USB,"4",1) == 0){
    if(myLoraMaster.sent == NOT_SENT_YET){
      Send_Unicast_Data();
    }
  }      
}

void Master_Send_Data_Test(){
  Send_Unicast_Data_Test();
}

/**********************************************************
**Name:     Start_Timer
**Function: Start_Timer
**Input:    function to call when timeout and time
**Output:   none
**********************************************************/
void Start_Timer(SW_TIMER_CALLBACK function, u32 time){
  SW_TIMER_CREATE_FunCallBack(SW_TIMER1, time, function);
  SW_TIMER_START(SW_TIMER1);
}

/**********************************************************
**Name:     Receive_Data
**Function: Receive_Data
**Input:    none
**Output:   none
**********************************************************/
void Receive_Data(){
  myLoraMaster.uni_sent = ALREADY_SENT;
  u8 store_packet[PACKET_LENGTH];
  u8 store_id[NUMBER_OF_SLAVES];                            
  for(uint32_t i = 0; i < NUMBER_OF_SLAVES - 1; i++){
          store_id[i] = 0;
  }
  strncpy((char*)store_id,(char*)(RxData + myLoraPtr.current_ptr),1);
  myLoraMaster.status[atoi((char*)store_id)] = 1;
  strcpy((char*)store_packet, (char*)(RxData + myLoraPtr.current_ptr));
  myLoraPtr.current_ptr++;
  if(myLoraPtr.current_ptr == LAST_POSITION_OF_QUEUE) myLoraPtr.current_ptr = FIRST_POSITION_OF_QUEUE;
  myLoraMode.slave_count++;
  myLoraSlave.rssi_value = sx1276_7_8_LoRaReadRSSI();	
  sprintf(myLoraMode.strBuf,"Node 1.%s connected with RSSI: %d\n\n", (char*)store_id, myLoraSlave.rssi_value);				
  printUSB(myLoraMode.strBuf);	  
}

void Receive_Data_Test(){
  myLoraMaster.uni_sent = ALREADY_SENT;                       
  myLoraPtr.current_ptr++;
  myLoraSlave.rssi_value = sx1276_7_8_LoRaReadRSSI();	
  //sprintf(myLoraMode.strBuf,"Txconf: %d, Txdone: %d, Rxconf: %d, RSSI: %d, number: %d\n\n",LoraTime.timeTxconf, LoraTime.timeTxdone, LoraTime.timeRxconf, myLoraSlave.rssi_value, count_success++);	
  sprintf(myLoraMode.strBuf,"Txconf: %d, Txdone: %d, Rxconf: %d, Time1: %d\n\n",LoraTime.timeTxconf, LoraTime.timeTxdone, LoraTime.timeRxconf, time1);
  printUSB(myLoraMode.strBuf);	  
}

/**********************************************************
**Name:     Master_Receive_Data
**Function: Master_Receive_Data
**Input:    none
**Output:   none
**********************************************************/
void Master_Receive_Data(){
  if(myLoraMode.flag_timer == 0){
    SW_TIMER_CALLBACK callback_function = fun1;
    myLoraMode.flag_timer = TIMER_SET;
    Start_Timer(callback_function, TIME_OUT);
  }
  if(Indicate_Rx_Packet("10", 0) == 1) //Receive a legal packet
  {
    myLoraMode.flag_timer = TIMER_RESET;
    SW_TIMER_CLEAR(SW_TIMER1);
    
    Receive_Data();
                             
    if(myLoraMode.uni_or_broad == 1){
      if(myLoraMode.slave_count == 7){
        myLoraMode.mode = MASTER_TX;	
        myLoraMode.slave_count = 0;
      }
    }
    else{       
      SW_TIMER_CLEAR(SW_TIMER1);
      myLoraMode.mode = 7;
    }
  } 
}

void Master_Receive_Data_Test(){
  if(myLoraMode.flag_timer == 0){
    SW_TIMER_CALLBACK callback_function = fun1;
    myLoraMode.flag_timer = TIMER_SET;
    Start_Timer(callback_function, TIME_OUT);
  }
  if(LoraTime.status == RXSTARTDONE)
  {
    
    if(Indicate_Rx_Packet("10", 0) == 1) //Receive a legal packet
    {	
      time1 = HAL_GetTick() - time;
      myLoraMode.flag_timer = TIMER_RESET;
      SW_TIMER_CLEAR(SW_TIMER1);
      
      if(LoraTime.status == GETACK)
      {
      Receive_Data_Test();
      
      }                               
      myLoraMode.mode = MASTER_TX;	

    }
  }
}