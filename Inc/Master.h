#define DELAY_DONE 3
#define SET_DELAY 2
#define BROADCAST 1
#define UNICAST 0
#define NOT_SENT_YET 0
#define NOT_RECEIVED_YET 0
#define ALREADY_SENT 1
#define TIME_BETWEEN_DATA_SENT 100000 * 0.5
#define TIMER_SET 1
#define TIMER_RESET 0
#define POSITION_START 2
typedef struct{
  u8 uni_received;
  u8 uni_sent;
  u8 sent;  
  u8 status[100];
  u8 status_prev[100];
  u8 mode;
  u32 delay[100];
  u32 broad_count;
  u32 delay_count;
}LoraMaster;

extern LoraMaster myLoraMaster;
extern void Master_Send_Data();
extern void Master_Send_Data_Test();
extern void Master_Receive_Data();
extern void Master_Receive_Data_Test();
extern void Send_Unicast_Data();
extern void Send_Broadcast_Data(char* data);
extern u8 time1;
extern u8 delay_position;