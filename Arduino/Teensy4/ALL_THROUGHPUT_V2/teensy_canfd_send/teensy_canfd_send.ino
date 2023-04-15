/*
This code is for the Teensy acting as the motor control hub which
also talks to the IMU hub Teensies
*/

#include <FlexCAN_T4.h>
#include <elapsedMillis.h>

FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_128> can;
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_128> can1;
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_128> can2;

elapsedMillis t_ms;
CANFD_message_t msg;
CAN_message_t msg1, msg1_recv, msg2;

void setup() {
/*
  can.begin();
  CANFD_timings_t cfg;
  cfg.clock = CLK_8MHz;
  cfg.baudrate = 1000000;
  cfg.baudrateFD = 1000000;
  cfg.propdelay = 190;
  cfg.bus_length = 1;
  cfg.sample = 70;
  can.setRegions(64);
  can.setBaudRate(cfg);
*/
  msg.id = 0x200;
  msg.len = 64;

  can1.begin();
  can1.setBaudRate(1000000);
  can2.begin();
  can2.setBaudRate(1000000);

  msg1.id = 0x145;
  msg1.buf[0] = 0xA1;
  msg2.id = 0x141;
  msg2.buf[0] = 0xA1;
  
  for (int i = 1; i < 8; i++)
  {
    msg1.buf[i] = 0;
    msg2.buf[i] = 0;

  }


  Serial.begin(115200);
  delay(1000);
}

void loop() {
 // can.write(msg);
  while (!can1.read(msg1_recv))
  {
    can1.write(msg1);
  }
 // while (!can2.read(msg1_recv))
 // {
 //   can2.write(msg2);
 // }
  Serial.println("received from both!");
  //msg.buf[0]++;
  delay(1000);
}