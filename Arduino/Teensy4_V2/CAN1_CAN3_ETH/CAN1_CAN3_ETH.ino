/* 
This code gets data from CAN1 and CAN3 and sends it over ETH to Raspberry Pi
*/

#include <FlexCAN_T4.h>
#include <NativeEthernet.h>

#define BUFF_SIZE 8
#define PORT      8888
#define CAN_1M    1000000
#define DELAY_MS  100


/* CANBus Variables */
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;  // can1 port 
FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can3;  // can1 port 

const int RMD_CAN_ID0 = 0x145;
const int RMD_CAN_ID1 = 0x141;

CAN_message_t msg;
CAN_message_t zero_torque0;
CAN_message_t zero_torque1;

CAN_message_t msg_recv1, msg_recv3;


//CANFD_message_t canfd_msg;


bool received1 = 0;
bool received3 = 0;


/* ETH variables */
IPAddress server(192,168,5,1); // IP address of the RPi WiFi AP
EthernetClient client;// Initialize the Ethernet client library
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
uint8_t eth_buf[2*BUFF_SIZE] = {0};






void setup() 
{
  Serial.begin(115200);
  delay(2000);
  initETH();
  delay(2000);
  initCAN();
  delay(2000);
}

bool flag1 = 0, flag3 = 0;
int ok = 0;

unsigned long t0 = 0, dt = 0;

unsigned count = 0;

void loop() 
{
  t0 = micros();
  send_all();
  recv_all();
  dt = micros() - t0;
  for (int i = 0; i < 8; i++)
  {
    eth_buf[i] = msg_recv1.buf[i];
    eth_buf[i+8] = msg_recv3.buf[i];
  }
  client.write(eth_buf, 16);
  delay(DELAY_MS);
}


void send_all()
{
  while(! (flag1 && flag3))
  {
    if (!flag1) 
    {
      ok = can1.write(zero_torque0);
      if (ok == 1)
      {
        flag1 = 1;
      }
    }
    if (!flag3) 
    {
      ok = can3.write(zero_torque1);
      if (ok == 1)
      {
        flag3 = 1;
      }
    }
  }
  flag1 = 0;
  flag3 = 0;
}


void recv_all()
{
  while(! (flag1 && flag3))
  {
    if (!flag1) 
    {
      ok = can1.read(msg_recv1);
      if (ok == 1)
      {
        flag1 = 1;
      }
    }
    if (!flag3) 
    {
      ok = can3.read(msg_recv3);
      if (ok == 1)
      {
        flag3 = 1;
      }
    }
  }
  flag1 = 0;
  flag3 = 0;
}

void printCAN(uint8_t *buf, int n)
{
  for (int i = 0; i < n; i++)
  {
    Serial.print(buf[i]);
    Serial.print(" ");
  }
  Serial.print("        ");
}


void wait(unsigned us) {
  unsigned long T0 = micros();
  while ((micros() - T0) < us);
}


void initCAN() {
  zero_torque0.id = RMD_CAN_ID0;
  zero_torque1.id = RMD_CAN_ID1;

  zero_torque0.buf[0] = 0xA1;
  zero_torque1.buf[0] = 0xA1;

  for (int i = 1; i < 8; ++i) {
    zero_torque0.buf[i] = 0;
    zero_torque1.buf[i] = 0;
  }

  can1.begin();
  can3.begin();

  can1.setBaudRate(CAN_1M);     // 1 Mbps data rate
  can3.setBaudRate(CAN_1M);     // 1 Mbps data rate
  
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

