/* 
This code is for sending zero-torque commands to RMDx8, reading reply, and sending reply to RPi over Ethernet
*/

#include <FlexCAN_T4.h>
#include <NativeEthernet.h>

#define BUFF_SIZE 8
#define PORT      8888
#define CAN_1M    1000000
#define DELAY_MS  1

/* ETH variables */
IPAddress server(192,168,5,1); // IP address of the RPi WiFi AP
EthernetClient client;// Initialize the Ethernet client library
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
uint8_t eth_buf[BUFF_SIZE] = {0};



/* CANBus Variables */
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;  // can1 port 
const int RMD_CAN_ID = 0x141;
CAN_message_t zero_torque;
bool received = 0;


void setup() 
{
  Serial.begin(115200);
  delay(3000);
  initCAN();
  delay(1000);
  initETH();
  delay(1000);
}


void loop() 
{
  can1.write(zero_torque);
  while(!received) { wait(1); }
  client.write(eth_buf, BUFF_SIZE);
  delay(DELAY_MS);
}

void wait(unsigned us) {
  unsigned long T0 = micros();
  while ((micros() - T0) < us);
}

// global callback
void canSniff20(const CAN_message_t &msg)
{ 
  for (int i = 0; i < BUFF_SIZE; i++)
  {
    eth_buf[i] = msg.buf[i];
  }
  received = 1;
}


void initETH()
{
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }
  else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  delay(1000);
  Serial.print("connecting to ");
  Serial.print(server);
  Serial.println("...");

  if (client.connect(server, PORT)) {
    Serial.print("connected to ");
    Serial.println(client.remoteIP());
  } 
  else 
  {
    Serial.println("connection failed");
  }
}


void initCAN() {
  zero_torque.id = RMD_CAN_ID;
  zero_torque.buf[0] = 0xA1;
  for (int i = 1; i < 8; ++i) {
    zero_torque.buf[i] = 0;
  }
  can1.begin();
  can1.setBaudRate(CAN_1M);     // 1 Mbps data rate
  can1.enableFIFO();
  can1.enableFIFOInterrupt();
  can1.onReceive(FIFO, canSniff20);
//  can1.mailboxStatus();
}