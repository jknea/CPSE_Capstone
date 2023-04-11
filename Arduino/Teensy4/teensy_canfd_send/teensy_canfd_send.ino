/*
This code is for the Teensy acting as the motor control hub which
also talks to the IMU hub Teensies
*/

#include <FlexCAN_T4.h>
#include <elapsedMillis.h>

FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_128> can;
elapsedMillis t_ms;
CANFD_message_t msg;

void setup() {
  can.begin();
  CANFD_timings_t cfg;
  cfg.clock = CLK_80MHz;
  cfg.baudrate = 1000000;
  cfg.baudrateFD = 8000000;
  cfg.propdelay = 190;
  cfg.bus_length = 1;
  cfg.sample = 70;
  can.setRegions(64);
  can.setBaudRate(cfg);
  msg.id = 0x200;
  msg.len = 64;
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  if (t_ms >= 1000) {
    t_ms = 0;
    msg.buf[0]++;
    can.write(msg);
  }
}