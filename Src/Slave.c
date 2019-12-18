#include "Lora_transmission_HAL.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "Slave.h"
#include "logging.h"
#include "mainApp.h"
#include "Master.h"

LoraSlave myLoraSlave;
u32 count_reponse = 0;
u8 x =0;
u32 t_count = 0;
u32 d_count = 0;
/**********************************************************
**Name:     Delay_By_Id
**Function: Delay_By_Id
**Input:    slave id to delay
**Output:   none
**********************************************************/
void Delay_By_Id(char* slave_id){  
  HAL_Delay(BROADCAST_DELAY_TIME * atoi(slave_id));    
}

/**********************************************************
**Name:     Slave_Send_Response_Broadcast
**Function: Slave_Send_Response_Broadcast
**Input:    slave id to send data
**Output:   none
**********************************************************/
void Slave_Send_Response(u8 slave_mode, char* slave_id){
  if(slave_mode == BROADCAST || slave_mode == SET_DELAY){
    Delay_By_Id(slave_id);                           
  }
  for(u8 i = 1; i <PACKET_LENGTH ; i++){
        sx1276_7_8Data[i] = 0;
  }
  if(slave_mode == SET_DELAY){
    sprintf((char*)sx1276_7_8Data,"R_%s\n", slave_id);		
  }
  else if(slave_mode == DELAY_DONE){
    HAL_Delay(myLoraSlave.slave_delay);
    sprintf((char*)sx1276_7_8Data,"T\n", slave_id);		
  }
  else{
    sprintf((char*)sx1276_7_8Data,"%s_%d\n", slave_id, myLoraSlave.rssi_value);		
  }
  sprintf((char*)(myTxPacket.Data), "Data sent: %s", (char*)sx1276_7_8Data);														
  printUSB((char*)(myTxPacket.Data));
  Switch_To_Tx();																											
  Send_Tx_Packet((u8*)sx1276_7_8Data, PACKET_LENGTH);																								
  Switch_To_Rx();																			
  myLoraMode.mode = SLAVE_RX;	
}

/**********************************************************
**Name:     Slave_Receive_Unicast
**Function: Slave_Receive_Unicast
**Input:    none
**Output:   none
**********************************************************/
void Slave_Receive_Unicast(){
  myLoraSlave.rssi_value = sx1276_7_8_LoRaReadRSSI();	
  sprintf(myLoraMode.strBuf,"Data received: %s\n",(char*)(RxData + myLoraPtr.current_ptr));		
  myLoraPtr.current_ptr++;
  if(myLoraPtr.current_ptr == LAST_POSITION_OF_QUEUE) myLoraPtr.current_ptr = FIRST_POSITION_OF_QUEUE;
  printUSB(myLoraMode.strBuf);										
  myLoraMode.mode = SLAVE_TX;                                                        
  myLoraSlave.slave_mode = UNICAST;                                
}

/**********************************************************
**Name:     Slave_Receive_Broadcast
**Function: Slave_Receive_Broadcast
**Input:    none
**Output:   none
**********************************************************/
void Slave_Receive_Broadcast(){
  myLoraSlave.rssi_value = sx1276_7_8_LoRaReadRSSI();						
  sprintf(myLoraMode.strBuf,"Data received: %s\n",(char*)(RxData + myLoraPtr.current_ptr));		
  myLoraPtr.current_ptr++;
  if(myLoraPtr.current_ptr == LAST_POSITION_OF_QUEUE) myLoraPtr.current_ptr = FIRST_POSITION_OF_QUEUE;
  printUSB(myLoraMode.strBuf);										
  myLoraMode.mode = SLAVE_TX;             
  myLoraSlave.slave_mode = BROADCAST;                                
}


/**********************************************************
**Name:     Slave_Receive_Delay
**Function: Slave_Receive_Delay
**Input:    none
**Output:   none
**********************************************************/
void Slave_Receive_Delay(){
  sprintf(myLoraMode.strBuf,"Data received: %s\n",(char*)(RxData + myLoraPtr.current_ptr));		  
  printUSB(myLoraMode.strBuf);										
  myLoraMode.mode = SLAVE_TX;                                                        
  myLoraSlave.slave_mode = SET_DELAY;                                
}

/**********************************************************
**Name:     Slave_Set_Delay
**Function: Slave_Set_Delay
**Input:    none
**Output:   none
**********************************************************/
void Slave_Set_Delay(){
  u8 print_usb[50];
  sprintf((char*)print_usb,"Data received: %s\n",(char*)(RxData + myLoraPtr.current_ptr));		
  sprintf(myLoraMode.strBuf,"%s\n",(char*)(RxData + myLoraPtr.current_ptr));		  
  printUSB(print_usb);										
  myLoraSlave.slave_delay = atoi(substring(myLoraMode.strBuf, indexOf(myLoraMode.strBuf, "_")+1, indexOf(myLoraMode.strBuf, "&")-1)); 
  u8 print_usb1[50];
  //sprintf((char*)print_usb1,"substring: %s\n",substring(myLoraMode.strBuf, indexOf(myLoraMode.strBuf, "_")+1, indexOf(myLoraMode.strBuf, "&")-1));		
  //printUSB(print_usb1);		
  //myLoraSlave.slave_delay = atoi(substring(myLoraMode.strBuf, 1, 5)); 
  //myLoraSlave.slave_delay = indexOf(myLoraMode.strBuf, "_"); 
  u8 set_delay_print[50];
  sprintf(set_delay_print,"Set delay: %d\n", myLoraSlave.slave_delay);	
  	
  printUSB(set_delay_print);		
  myLoraMode.mode = SLAVE_RX;                                                                                    
}

/**********************************************************
**Name:     Set_Delay_Done
**Function: Set_Delay_Done
**Input:    slave id to receive data if in unicast mode
**Output:   none
**********************************************************/
void Set_Delay_Done(void){
  delay_position = 0;
  Send_Broadcast_Data("T");
  myLoraMaster.mode = 1;  
}

/**********************************************************
**Name:     Slave_Receive_Data
**Function: Slave_Receive_Data
**Input:    slave id to receive data if in unicast mode
**Output:   none
**********************************************************/
void Slave_Receive_Data(char* slave_id){
  if(Indicate_Rx_Packet(slave_id, SLAVE_IS_RECEIVING) == 1) //Receive a legal packet
  {	
    if(strncmp((char*)(RxData + myLoraPtr.current_ptr),slave_id,1) == 0){																
        Slave_Receive_Unicast();
    }
    else if(strncmp((char*)(RxData + myLoraPtr.current_ptr),"a",1) == 0){																
        Slave_Receive_Broadcast();
    }
    else if(strncmp((char*)(RxData + myLoraPtr.current_ptr),"T",1) == 0 && myLoraSlave.receive_T == 0){	        
        if(atoi(substring((char*)(RxData + myLoraPtr.current_ptr), indexOf((char*)(RxData + myLoraPtr.current_ptr), "_") + 1, strlen((char*)(RxData + myLoraPtr.current_ptr)) - 1)) == t_count)   
        { 
          t_count++;
          myLoraSlave.receive_T = 1;
          Slave_Receive_Delay();
        }
        myLoraPtr.current_ptr++;
        if(myLoraPtr.current_ptr == LAST_POSITION_OF_QUEUE) myLoraPtr.current_ptr = FIRST_POSITION_OF_QUEUE;
    }
    else if(strncmp((char*)(RxData + myLoraPtr.current_ptr),"S",1) == 0 && myLoraSlave.receive_S == 0){   
      u8 set_delay_print[50];
      sprintf(set_delay_print,"Sub: %s, ID: %s, length: %d\n", substring((char*)(RxData + myLoraPtr.current_ptr), indexOf((char*)(RxData + myLoraPtr.current_ptr), "&") + 1, (unsigned int)(strlen((char*)(RxData + myLoraPtr.current_ptr))) - 1), myLoraSlave.slave_id, (unsigned int)(strlen((char*)(RxData + myLoraPtr.current_ptr))));	            
      printUSB(set_delay_print);		
      //if(strncmp(substring((char*)(RxData + myLoraPtr.current_ptr), indexOf((char*)(RxData + myLoraPtr.current_ptr), "&") + 1, strlen((char*)(RxData + myLoraPtr.current_ptr)) - 1), myLoraSlave.slave_id, strlen(myLoraSlave.slave_id)) == 0)      
      if(strcmp(substring((char*)(RxData + myLoraPtr.current_ptr), indexOf((char*)(RxData + myLoraPtr.current_ptr), "&") + 1, strlen((char*)(RxData + myLoraPtr.current_ptr)) - 1), myLoraSlave.slave_id) == 0)      
      {        
        myLoraSlave.receive_S = 1;
        Slave_Set_Delay();
      } 
      myLoraPtr.current_ptr++;
      if(myLoraPtr.current_ptr == LAST_POSITION_OF_QUEUE) myLoraPtr.current_ptr = FIRST_POSITION_OF_QUEUE;
    }
    else if(strncmp((char*)(RxData + myLoraPtr.current_ptr),"D",1) == 0){	
        u8 set_delay_print[50];
        sprintf(set_delay_print,"Received done: %s\n", (char*)(RxData + myLoraPtr.current_ptr));	            
        printUSB(set_delay_print);		
        if(atoi(substring((char*)(RxData + myLoraPtr.current_ptr), indexOf((char*)(RxData + myLoraPtr.current_ptr), "_") + 1, strlen((char*)(RxData + myLoraPtr.current_ptr)) - 1)) == d_count)   
        { 
          d_count++;
          Set_Delay_Done();
        }        
        myLoraPtr.current_ptr++;
        if(myLoraPtr.current_ptr == LAST_POSITION_OF_QUEUE) myLoraPtr.current_ptr = FIRST_POSITION_OF_QUEUE;
        
    }
  }    
}