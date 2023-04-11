#include <FlexCAN_T4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;  // can1 port 

const int RMD_CAN_ID = 0x141;

CAN_message_t zero_torque;
CAN_message_t read_encoder;


/* CANBus Variables */
int packetSize = 0;
unsigned long canId;
unsigned motor_temp;
int iq;
int motor_speed;
int motor_angle;
bool received = 0;


/* Timer Variables */
unsigned long t0 = 0, t1 = 0, t2 = 0, t3 = 0;
unsigned long send_time, wait_time, recv_time, total_time;


void setup() {
  Serial.begin(115200);
  delay(2000);
  InitCAN();
}

void loop() {
  t0 = micros();
  for (int i = 1; i < 8; ++i) {
    zero_torque.buf[i] = 0;
  }
  can1.write(zero_torque);
  t1 = micros();
  while(!received) { t2 = micros(); }
  t3 = micros();
  received = 0;
  send_time = t1 - t0;
  wait_time = t2 - t1;
  recv_time = t3 - t2;
  total_time = t3 - t0;
  Serial.print("elapsed time (in us): ");
  Serial.print(send_time); 
  Serial.print(", ");
  Serial.print(wait_time); 
  Serial.print(", ");
  Serial.print(recv_time); 
  Serial.print(", ");
  Serial.println(total_time); 
  
  /*
  Serial.print("Received data: ");
  Serial.print(motor_temp);
  Serial.print(", ");
  Serial.print(iq);
  Serial.print(", ");
  Serial.print(motor_speed);
  Serial.print(", ");
  Serial.print(motor_angle);
  Serial.println();
*/
  delay(1000);
}

void wait(unsigned us) {
  unsigned long T0 = micros();
  while ((micros() - T0) < us);
}

// global callback
void canSniff20(const CAN_message_t &msg)
{ 
  t2 = micros();
  motor_temp = msg.buf[1];
  iq = (uint16_t) msg.buf[3] << 8 | msg.buf[2];
  motor_speed = (uint16_t) msg.buf[5] << 8 | msg.buf[4];
  motor_angle = (uint16_t) msg.buf[7] << 8 | msg.buf[6];
  received = 1;
}

void InitCAN() {
  zero_torque.id = RMD_CAN_ID;
  zero_torque.buf[0] = 0xA1;
  for (int i = 1; i < 8; ++i) {
    zero_torque.buf[i] = 0;
  }
  can1.begin();
  can1.setBaudRate(1000000);     // 1 Mbps data rate
  can1.enableFIFO();
  can1.enableFIFOInterrupt();
  can1.onReceive(FIFO, canSniff20);
  can1.mailboxStatus();
}
