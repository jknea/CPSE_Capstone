/*  
This code is for the Teensy acting as an IMU Hub
*/

#include <FlexCAN_T4.h>
FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_64> can;
CANFD_message_t msg, send_msg;

bool received = 0;

void setup(void) {
  Serial.begin(115200);
  delay(400);
  
  send_msg.id = 0x300;
  send_msg.len = 64;

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


  can.setMBFilter(MB0, 0x200); // Set mailbox 6 to allow CAN ID 0x123 to be collected. 
  can.onReceive(MB0, isr);

  can.enableMBInterrupts();


//  can.mailboxStatus();
  //can.setMBFilter(REJECT_ALL);
}

int x = 0;

void isr(const CANFD_message_t &msg) {
//  Serial.print("MB "); Serial.print(msg.mb);
//  Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
//  Serial.print("  LEN: "); Serial.print(msg.len);
//  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
//  Serial.print(" TS: "); Serial.print(msg.timestamp);
//  Serial.print(" ID: "); Serial.print(msg.id, HEX);
//  Serial.print(" Buffer: ");
//  Serial.println();
  for ( uint8_t i = 0; i < msg.len; i++ ) {
//    Serial.print(msg.buf[i], HEX); Serial.print(" ");
      x = msg.buf[i];
  } 
//  Serial.println();
  received = 1;
}

void loop() {
  //can.mailboxStatus();
  while(!received) { can.events(); }
  received = 0;
  for (int i = 0; i < 64; i++) {
    send_msg.buf[i] = i;
  }
  can.write(send_msg);
//  Serial.println("wrote message");
  //delay(1000);
}