/*  
This code is for the Teensy acting as an IMU Hub
*/

#include <FlexCAN_T4.h>

FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_64> can;
CANFD_message_t canfd_msg;


const int THIS_CAN_ID = 0x300;
const int TEENSY_CONTROLLER_ID = 0x200;
bool received = 0;

void setup(void) {
  Serial.begin(115200);
  delay(2000);
  initCAN();
  delay(2000);
  Serial.println("Finished setup!");

}

void isr(const CANFD_message_t &msg) {
  for ( uint8_t i = 0; i < 8; i++ ) {
    Serial.print(msg.buf[i], HEX); 
    Serial.print(" ");
  } 
  Serial.println();
  received = 1;
}

void loop() {
  while(!received) { can.events(); }
  received = 0;
 // for (int i = 0; i < 64; i++)
 // {
 //   canfd_msg.buf[i] = 13;
 // }
  //can.write(canfd_msg);
}


void initCAN()
{
  canfd_msg.id = TEENSY_CONTROLLER_ID;  // msg to send back to controller

  can.begin();
  CANFD_timings_t cfg;
  cfg.clock = CLK_24MHz;
  cfg.baudrate = 1000000;
  cfg.baudrateFD = 1000000;
  cfg.propdelay = 190;
  cfg.bus_length = 1;
  cfg.sample = 70;
  can.setRegions(64);
  can.setBaudRate(cfg);

  can.setMBFilter(MB0, THIS_CAN_ID); // Set mailbox 6 to allow CAN ID 0x123 to be collected. 
  can.onReceive(MB0, isr);
  can.enableMBInterrupts();
}