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
#include "My_type.h"

extern u8 _index;
extern u8 store_packet[PACKET_LENGTH];
extern u8 store_id[NUMBER_OF_SLAVES];  
//extern u8 tx_b[PACKET_LENGTH];
//extern u8 Tx_Packet_b[PRINTUSB_LENGTH];
extern u8 tx_u[PACKET_LENGTH];
extern u8 Tx_Packet_u[PRINTUSB_LENGTH];
LoraMaster myLoraMaster;
u32 count_success = 0;
u32 count_total = 0;
u8 time = 0, time1 = 0;
u32 packet_count = 0;
u32 broad_count = 0;
u32 TIME_OUT = 0;
u8 delay_position;
SW_TIMER_CALLBACK callback_function;
//u32 broadcast_count = 0;
/**********************************************************
**Name:     Send_Broadcast_Data
**Function: Send_Broadcast_Data
**Input:    data to send in CHAR*
**Output:   none
**********************************************************/
void Send_Broadcast_Data(char* data){ 
  if(strncmp(myLoraSlave.slave_id, "0", 1) == 0){
    
  }
  else{    
    HAL_Delay(myLoraSlave.slave_delay);
  }
  
  u8 tx_b[PACKET_LENGTH];
  u8 Tx_Packet_b[PRINTUSB_LENGTH];
  HAL_Delay(TIME_BETWEEN_DATA_SENT);  
  if(strncmp(data, "a",1) == 0){
    sprintf((char*)tx_b,"%s_%d\n", data, myLoraMaster.broad_count);	
    sprintf((char*)Tx_Packet_b, "Data sent: %s, %d\n ",data, myLoraMaster.broad_count++);
  }
  else if(strncmp(data, "T",1) == 0){
    delay_position = 0;
    sprintf((char*)tx_b,"%s_%d\n", data, myLoraMaster.delay_count);	
    sprintf((char*)Tx_Packet_b, "Data sent: %s, %d\n ",data, myLoraMaster.delay_count);
  }
  //GetRealTime();
  
  printUSB((char*)Tx_Packet_b);
  Switch_To_Tx();																											
  Send_Tx_Packet((u8*)tx_b, PACKET_LENGTH);																								
  Switch_To_Rx();																			
  myLoraMode.mode = MASTER_RX;	
  myLoraSlave.slave_mode = BROADCAST;
  if(myLoraMaster.mode == 0){
    for(u8 i = 1; i < 100; i++){
          myLoraMaster.status[i] = 0;
    }
  }
}

/**********************************************************
**Name:     Send_Unicast_Data
**Function: Send_Unicast_Data
**Input:    data to send in CHAR*
**Output:   none
**********************************************************/

void Send_Unicast_Data(){

  for(u8 i = 1; i <PACKET_LENGTH ; i++){
        tx_u[i] = 0;
        //Tx_Packet_u[i] = 0;
  }
  if(strncmp((char*)received_USB,"1",1) == 0){
    myLoraSlave.slave_id = "1";
    myLoraSlave.slave_mode = UNICAST;
  }
  else if(strncmp((char*)received_USB,"2",1) == 0){
    myLoraSlave.slave_id = "2";
    myLoraSlave.slave_mode = UNICAST;
  }
  else if(strncmp((char*)received_USB,"3",1) == 0){
    myLoraSlave.slave_id = "3";
    myLoraSlave.slave_mode = UNICAST;
  }
  else if(strncmp((char*)received_USB,"4",1) == 0){
    myLoraSlave.slave_id = "4";
    myLoraSlave.slave_mode = UNICAST;
  }
  
  for(u8 i = 0; i <8 ; i++){
      received_USB[i] = 0;
  }
  sprintf((char*)tx_u,"%s_%d", myLoraSlave.slave_id, count_total++);		
  sprintf((char*)Tx_Packet_u, "Unicast to 1.%s, count: %d\n", (char*)tx_u, count_total);														
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

void Master_Send_Data(){										  
  if(strncmp((char*)received_USB,"b",1) == 0){
    Send_Broadcast_Data("abb");
  }
  else if(strncmp((char*)received_USB,"t",1) == 0){    
    for(u8 i = 0; i <8 ; i++){
        received_USB[i] = 0;
    }
    Send_Broadcast_Data("T");
    myLoraMaster.mode = 1;
  }
  else if(strncmp((char*)received_USB,"1",1) == 0 || strncmp((char*)received_USB,"2",1) == 0 
          || strncmp((char*)received_USB,"3",1) == 0 || strncmp((char*)received_USB,"4",1) == 0){
    if(myLoraMaster.sent == NOT_SENT_YET){
      Send_Unicast_Data();
    }
  } 
  _index  = 0;
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
  for(uint32_t i = 0; i < PACKET_LENGTH - 1; i++){
          store_packet[i] = 0;
  }                   
  for(uint32_t i = 0; i < NUMBER_OF_SLAVES - 1; i++){
          store_id[i] = 0;
  } 
  strncpy((char*)store_id,(char*)(RxData + myLoraPtr.current_ptr),1);

  if(atoi((char*)store_id) > 0)
  {
    myLoraMaster.status[atoi((char*)store_id)] = 1;
  }

  strcpy((char*)store_packet, (char*)(RxData + myLoraPtr.current_ptr));
  myLoraPtr.current_ptr++;
  if(myLoraPtr.current_ptr == LAST_POSITION_OF_QUEUE) myLoraPtr.current_ptr = FIRST_POSITION_OF_QUEUE;
  myLoraMode.slave_count++;
  myLoraSlave.rssi_value = sx1276_7_8_LoRaReadRSSI();	
  sprintf(myLoraMode.strBuf,"Node 1.%s - RSSI: %d, count: %d\n", (char*)store_id, myLoraSlave.rssi_value, ++count_success);				
  printUSB(myLoraMode.strBuf);	  
}

/**********************************************************
**Name:     Receive_Data_Delay
**Function: Receive_Data_Delay
**Input:    none
**Output:   none
**********************************************************/
void Receive_Data_Delay(void){
    strcpy((char*)store_packet, (char*)(RxData + myLoraPtr.current_ptr));
    myLoraPtr.current_ptr++;
    if(myLoraPtr.current_ptr == LAST_POSITION_OF_QUEUE) myLoraPtr.current_ptr = FIRST_POSITION_OF_QUEUE;
    myLoraMaster.delay[delay_position] = atoi(substring((char*)store_packet, POSITION_START, strlen((char*)store_packet) - 1));
    delay_position++;
}

/**********************************************************
**Name:     Master_Receive_Data
**Function: Master_Receive_Data
**Input:    none
**Output:   none
**********************************************************/
void Master_Receive_Data(){
    if(myLoraMode.flag_timer == TIMER_RESET){
    if(myLoraMaster.mode == 0){
      callback_function = fun1;
      TIME_OUT = 300000;
    }
    else if(myLoraMaster.mode == 1){
      callback_function = fun2;
      TIME_OUT = 500000;
    }
    else{
      callback_function = fun1;
      TIME_OUT = 300000;
    }
    myLoraMode.flag_timer = TIMER_SET;
    Start_Timer(callback_function, TIME_OUT);
  }
  if(Indicate_Rx_Packet("10", 0) == 1) //Receive a legal packet
  {
    if(strncmp((char*)(RxData + myLoraPtr.current_ptr),"R",1) == 0){
      myLoraMode.flag_timer = TIMER_RESET;
      SW_TIMER_CLEAR(SW_TIMER1);
      Receive_Data_Delay();               
    }
    else{
      myLoraMode.flag_timer = TIMER_RESET;
      SW_TIMER_CLEAR(SW_TIMER1);
      Receive_Data();
                               
      if(myLoraSlave.slave_mode == BROADCAST){
        if(myLoraMode.slave_count == 7){
          myLoraMode.mode = MASTER_TX;	
          myLoraMode.slave_count = 0;
        }
      }
      else{       
        SW_TIMER_CLEAR(SW_TIMER1);
        myLoraMode.mode = MASTER_TX;
      }
    }
  }
}
