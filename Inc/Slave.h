#define BROADCAST_DELAY_TIME 100000/100 //10ms
#define SET_DELAY_TIME 100000

typedef struct{
  char* slave_id;  
  s8 rssi_value;
  u8 slave_mode;
  u32 slave_delay;
  u8 receive_T;
  u8 receive_S;
}LoraSlave;

extern LoraSlave myLoraSlave;
extern void Delay_By_Id(char* slave_id);
extern void Slave_Send_Response_Broadcast(char* slave_id);
extern void Slave_Send_Response_Unicast(char* slave_id);
extern void Slave_Send_Response(u8 slave_mode, char* slave_id);
extern void Slave_Receive_Data(char* slave_id);