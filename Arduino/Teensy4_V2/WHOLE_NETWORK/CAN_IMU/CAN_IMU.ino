/* 
This code is to be used with CAN1_CAN2_CAN3_ETH.ino
It acts as an IMU device.
*/

#include <FlexCAN_T4.h>

#define CAN_1M    1000000

/* CANBus Variables */
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;  // can1 port 

const int THIS_CAN_ID = 0x300;     // IMU ID for CAN2
const int CONTROL_CAN_ID = 0x200;  // ID for teensy4.1 control MCU
CAN_message_t imu_data, msg_recv;
bool flag1 = 0;
int ok = 0;

void setup() 
{
  Serial.begin(115200);
  delay(2000);
  initCAN();
  delay(2000);
}

uint8_t count = 0;

void loop() 
{
  wait(500);
  recv_all();
  for (int i = 0; i < 8; i++)
  {
    imu_data.buf[i] = i+count;
  }
  count++;
  if (count > 255)
  {
    count = 0;
  }
//  Serial.peek();
  wait(500);
  send_all();
 // Serial.peek();

}


void send_all()
{
  while(! (flag1))
  {
    if (!flag1) 
    {
      ok = can1.write(imu_data);
      if (ok == 1)
      {
        flag1 = 1;
      }
    }
  }
  // clear flags
  flag1 = 0;
}


void recv_all()
{
  while(! (flag1))
  {
    if (!flag1) 
    {
      ok = can1.read(msg_recv);
      if (ok == 1)
      {
          flag1 = 1;

      }
    }
  }
  flag1 = 0;
}


void wait(unsigned us) {
  unsigned long T0 = micros();
  while ((micros() - T0) < us);
}

void initCAN() {
  imu_data.id = CONTROL_CAN_ID;
  can1.begin();
  can1.setBaudRate(CAN_1M);     // 1 Mbps data rate
}