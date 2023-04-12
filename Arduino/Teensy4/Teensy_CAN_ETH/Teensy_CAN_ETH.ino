#include <FlexCAN_T4.h>
#include <NativeEthernet.h>


#define BUFF_SIZE 256
#define PORT      8888
IPAddress server(192,168,5,1); // IP address of the RPi WiFi AP
EthernetClient client;// Initialize the Ethernet client library
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
uint8_t eth_buf[8] = {0};

#define SERIAL_BAUD                   115200
#define TRUE                          1
#define FILENAME_MAX_LEN              8
#define TXT_EXT_WITH_NULL_CHAR_LEN    5
#define LOG_ITERATIONS                1000
#define DELAY_MS                      1


/* CANBus Variables */
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;  // can1 port 
const int RMD_CAN_ID = 0x141;
CAN_message_t zero_torque;
CAN_message_t read_encoder;
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

/*
File myFile;
String SD_data;
char fname[FILENAME_MAX_LEN + TXT_EXT_WITH_NULL_CHAR_LEN] = {0};
char c;
int idx = 0;
bool filename_received = 0;
String instr = "";
*/

void setup() 
{
  Serial.begin(SERIAL_BAUD);
  delay(3000);
//  instr.reserve(13);
//  initSD();    // MUST USE A FAT32 FORMATTED SD CARD, AND MAKE SURE TO EJECT PROPERLY (more important to keep character count within limit specified above);
//  delay(2000);
  initCAN();
  delay(1000);
  initETH();
  delay(1000);
}


void loop() 
{
  Serial.println("in loop");
  can1.write(zero_torque);
  while(!received) { wait(1); }
  client.write(eth_buf, 8);
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
  for (int i = 0; i < 8; i++)
  {
    eth_buf[i] = msg.buf[i];
  }
  motor_temp = msg.buf[1];
  iq = (uint16_t) msg.buf[3] << 8 | msg.buf[2];
  motor_speed = (uint16_t) msg.buf[5] << 8 | msg.buf[4];
  motor_angle = (uint16_t) msg.buf[7] << 8 | msg.buf[6];
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
  can1.setBaudRate(1000000);     // 1 Mbps data rate
  can1.enableFIFO();
  can1.enableFIFOInterrupt();
  can1.onReceive(FIFO, canSniff20);
//  can1.mailboxStatus();
}