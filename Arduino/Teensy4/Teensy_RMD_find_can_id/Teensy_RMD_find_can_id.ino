/*
****************************************************************************************************
 *  Author: James Knea
 *  
 *  Description:  This code is for 
 *  pin4 flipped away from 4 is 0x141
 *  pin4 & 3 flipped away is 0x145
 *  pin4, 3, and 2 flipped is 0x147
 *  all 4 flipped up is 0x148
****************************************************************************************************
*/

#include <FlexCAN_T4.h>
#define RMD_BASE_ID     0x140
#define SERIAL_BAUD     115200

/* CANBus variables */
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;  // can1 port 
CAN_message_t send_msg;
bool received = 0;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(3000);
  initCAN();
}

void loop() {
  Serial.print("Sending packet ... ");
  for (int i = 1; i <= 32; i++) {
    send_msg.id = RMD_BASE_ID + i;
    can1.write(send_msg);
    delay(5);
    if (received) {
      Serial.print("The current id is 0x");
      Serial.println(RMD_BASE_ID + i, HEX);
      while(1);
    }
    delay(50);
  }
}

// global callback, process the received message "msg" here
void canSniff20(const CAN_message_t &msg) 
{ 
  received = 1; 
}

void populateMsg(CAN_message_t &msg, uint16_t send_id, uint8_t *send_buf)
{
  msg.id = RMD_BASE_ID;
  for (unsigned i = 0; i < 8; ++i) 
  {
    msg.buf[i] = send_buf[i];
  }
}

void initCAN() 
{
  can1.begin();
  can1.setBaudRate(1000000);     // 1 Mbps data rate
  can1.enableFIFO();
  can1.enableFIFOInterrupt();
  can1.onReceive(FIFO, canSniff20);
  can1.mailboxStatus();
}
