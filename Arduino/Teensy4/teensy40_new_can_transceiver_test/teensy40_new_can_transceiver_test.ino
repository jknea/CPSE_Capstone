#include <SD.h>
#include <FlexCAN_T4.h>

#define SERIAL_BAUD                   115200
#define TRUE                          1
#define FILENAME_MAX_LEN              8
#define TXT_EXT_WITH_NULL_CHAR_LEN    5
#define LOG_ITERATIONS                1000
#define DELAY_MS                      1000


/* CANBus Variables */
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;  // can1 port 
const int RMD_CAN_ID = 0x141;
CAN_message_t zero_torque;
bool received = 0;

uint8_t buf[8] = {0};

void setup() 
{
  Serial.begin(SERIAL_BAUD);
  delay(2000);
  initCAN();
  delay(2000);
}


void loop() 
{
  can2.write(zero_torque);
  while(!received) { wait(1); }
  for (int i = 0; i < 8; i++)
  {
    Serial.print(buf[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  delay(1);
}


void wait(unsigned us) {
  unsigned long T0 = micros();
  while ((micros() - T0) < us);
}

// global callback
void canSniff20(const CAN_message_t &msg)
{ 
  for(int i = 0; i < 8; i++)
  {
    buf[i] = msg.buf[i];
  }
  received = 1;
}


void initCAN() {
  zero_torque.id = RMD_CAN_ID;
  zero_torque.buf[0] = 0xA1;
  for (int i = 1; i < 8; ++i) {
    zero_torque.buf[i] = 0;
  }
  can2.begin();
  can2.setBaudRate(1000000);     // 1 Mbps data rate
  can2.enableFIFO();
  can2.enableFIFOInterrupt();
  can2.onReceive(FIFO, canSniff20);
//  can2.mailboxStatus();
}