/*
Default Mailbox Status:
	Mailboxes:
		MB0 code: RX_EMPTY	(Standard Frame)
		MB1 code: RX_EMPTY	(Standard Frame)
		MB2 code: RX_EMPTY	(Standard Frame)
		MB3 code: RX_EMPTY	(Standard Frame)
		MB4 code: RX_EMPTY	(Extended Frame)
		MB5 code: RX_EMPTY	(Extended Frame)
		MB6 code: RX_EMPTY	(Extended Frame)
		MB7 code: RX_EMPTY	(Extended Frame)
		MB8 code: TX_INACTIVE
		MB9 code: TX_INACTIVE
		MB10 code: TX_INACTIVE
		MB11 code: TX_INACTIVE
		MB12 code: TX_INACTIVE
		MB13 code: TX_INACTIVE
		MB14 code: TX_INACTIVE
		MB15 code: TX_INACTIVE
*/

#include <SD.h>
#include <FlexCAN_T4.h>

#define SERIAL_BAUD                   115200
#define TRUE                          1
#define FILENAME_MAX_LEN              8
#define TXT_EXT_WITH_NULL_CHAR_LEN    5
#define LOG_ITERATIONS                1000
#define DELAY_MS                      1

#define CAN_1M                        1000000


/* CANBus Variables */
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;  // can1 port 
//FlexCAN_T4<CAN3, RX_SIZE_256, TX_SIZE_16> can3;  // can3 port


const int RMD_CAN_ID0 = 0x141;
const int RMD_CAN_ID1 = 0x145;

CAN_message_t zero_torque_RMD0;
CAN_message_t zero_torque_RMD1;

CAN_message_t msg_MB0;
CAN_message_t msg_MB1;

//int packetSize = 0;
//unsigned long canId;
unsigned motor_temp0, motor_temp1;
int iq0, iq1;
int motor_speed0, motor_speed1;
int motor_angle0, motor_angle1;
bool received0 = 0, received1 = 0;


/* Timer Variables */
unsigned long t0 = 0, t1 = 0, t2 = 0, t3 = 0;
unsigned long send_time, wait_time, recv_time, total_time;


/* file variables */
File myFile;
String SD_data;
char fname[FILENAME_MAX_LEN + TXT_EXT_WITH_NULL_CHAR_LEN] = {0};
char c;
int idx = 0;
bool filename_received = 0;
String instr = "";


void setup() 
{
  Serial.begin(SERIAL_BAUD);
  delay(3000);
  instr.reserve(13);
  initSD();    // MUST USE A FAT32 FORMATTED SD CARD, AND MAKE SURE TO EJECT PROPERLY (more important to keep character count within limit specified above);
  delay(2000);
  initCAN();
}


void loop() 
{
  for (int i = 0; i < LOG_ITERATIONS; i++) {
    t0 = micros();
    for (int i = 1; i < 8; ++i) // only update the value fields of the torque control command 
    {
      zero_torque_RMD0.buf[i] = 0;
      zero_torque_RMD1.buf[i] = 0;
    }
  //  can1.write(zero_torque_RMD0);
    int x = can3.write(zero_torque_RMD1);
    Serial.println(x);
    while(!(received1 )) { wait(1); }
    t1 = micros();
    received0 = 0;
    received1 = 0;
    total_time = t1 - t0;

    myFile.print(total_time);
    myFile.print(",");

    delay(DELAY_MS);
  }

  Serial.println("Done logging!");
  myFile.close();
  while(TRUE) 
  { 
    wait(100); 
  }
}


void initSD() {
  if (!SD.begin(BUILTIN_SDCARD)) {
    Serial.println(F("SD CARD FAILED, OR NOT PRESENT!"));
    while (1);
  }
  Serial.println("SD CARD INITIALIZED.");
  Serial.println("Enter a file name <= 8 characters long:");
  while (!filename_received) {
     getFilename();
     if (SD.exists(fname)) {
        Serial.println("File already exists on SD, do you want to overwrite name? (y/n) : ");
        while (Serial.available()) { Serial.read(); } // clear garbage chars
        while (!Serial.available()) {} ;
        char yn = Serial.read();
        if (yn == 'y') {
          filename_received = 1;
        }
        else {
          idx = 0;
          memset(&fname[0], 0, 13);
        }
     }
     else {
        filename_received = 1;
     }
  }
  myFile = SD.open(fname, FILE_WRITE);
  Serial.println("File opened for writing!");
} 

// helper function for init_sd
void getFilename() {
    // get a valid nam  SERIAL.println("Enter the desired filename (without .txt and must be <= 8 characters) : ");
  while (Serial.available()) { Serial.read(); }  // clear garbage chars
  while (!Serial.available()) {};
  while (TRUE) {
      if (idx == FILENAME_MAX_LEN) {
        break;
      }
      if (Serial.available()) {
        c = Serial.read();
        if (c == '\n') {
          break;
        }
        fname[idx] = c;
        idx++;
      }
  }
  fname[idx++] = '.';
  fname[idx++] = 't';
  fname[idx++] = 'x';
  fname[idx++] = 't';
  fname[idx] = '\0';
  Serial.print("you entered : ");
  Serial.println(fname);
}



void wait(unsigned us) {
  unsigned long T0 = micros();
  while ((micros() - T0) < us);
}

// global callback
void canSniff20_MB0(const CAN_message_t &msg_MB0)
{ 
//  t2 = micros();

  motor_temp0 = msg_MB0.buf[1];
  iq0 = (int16_t) msg_MB0.buf[3] << 8 | msg_MB0.buf[2];
  motor_speed0 = (uint16_t) msg_MB0.buf[5] << 8 | msg_MB0.buf[4];
  motor_angle0 = (uint16_t) msg_MB0.buf[7] << 8 | msg_MB0.buf[6];
  received0 = 1;
//  Serial.println("received 0");
}

// global callback
void canSniff20_MB1(const CAN_message_t &msg_MB1)
{ 
//  t2 = micros();

  motor_temp1 = msg_MB1.buf[1];
  iq1 = (int16_t) msg_MB1.buf[3] << 8 | msg_MB1.buf[2];
  motor_speed1 = (uint16_t) msg_MB1.buf[5] << 8 | msg_MB1.buf[4];
  motor_angle1 = (uint16_t) msg_MB1.buf[7] << 8 | msg_MB1.buf[6];

  received1 = 1;
//  Serial.println("received 1");

}



void initCAN() {
  zero_torque_RMD0.id = RMD_CAN_ID0;
  zero_torque_RMD1.id = RMD_CAN_ID1;
  zero_torque_RMD0.buf[0] = 0x90;
  zero_torque_RMD1.buf[0] = 0x90;
  for (int i = 1; i < 8; ++i) {
    zero_torque_RMD0.buf[i] = 0;
    zero_torque_RMD1.buf[i] = 0;
  }


//  can1.begin();
  can3.begin();


//  can1.setBaudRate(CAN_1M);     // 1 Mbps data rate
  can3.setBaudRate(CAN_1M);     // 1 Mbps data rate

//  can1.setMBFilter(MB0, RMD_CAN_ID0); // Set mailbox 0 to allow CAN ID 0x141 to be collected. 
  can3.setMBFilter(MB1, RMD_CAN_ID1); // Set mailbox 1 to allow CAN ID 0x145 to be collected. 

//  can1.onReceive(MB0, canSniff20_MB0); // allows mailbox 0 messages to be received in the supplied callback.
  can3.onReceive(MB1, canSniff20_MB1); // allows mailbox 1 messages to be received in the supplied callback.

//  can1.enableMBInterrupts();
  can3.enableMBInterrupts();

//  can1.enableMBInterrupt(MB0);
//  can1.enableMBInterrupt(MB1);
  
  //can1.enableFIFO();
//  can1.enableFIFOInterrupt();
//  can1.mailboxStatus();

}

    
//  can1.enableMBInterrupts(); // enable all at once
  
//  can1.enableFIFO();
//  can1.enableFIFOInterrupt();
//  can1.onReceive(FIFO, canSniff20);