#include <SD.h>
#include <FlexCAN_T4.h>

#define SERIAL_BAUD                   115200
#define TRUE                          1
#define FILENAME_MAX_LEN              8
#define TXT_EXT_WITH_NULL_CHAR_LEN    5
#define LOG_ITERATIONS                1000
#define DELAY_MS                      1


/* CANBus Variables */
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> can2;  // can2 port 
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
//  delay(10000);
//  instr.reserve(13);
//  initSD();    // MUST USE A FAT32 FORMATTED SD CARD, AND MAKE SURE TO EJECT PROPERLY (more important to keep character count within limit specified above);
  delay(2000);
  initCAN();
  delay(2000);
}


void loop() 
{
  for (int i = 0; i < 2*LOG_ITERATIONS; i++) {
    t0 = micros();
    for (int i = 1; i < 8; ++i) 
    {
      zero_torque.buf[i] = 0;
    }
    can2.write(zero_torque);
    t1 = micros();
    while(!received) { t2 = micros(); }
    t3 = micros();
    received = 0;
    send_time = t1 - t0;
    wait_time = t2 - t1;
    recv_time = t3 - t2;
    total_time = t3 - t0;
    Serial.println(total_time); 
  /*  
    Serial.print("elapsed time (in us): ");
    Serial.print(send_time); 
    Serial.print(", ");
    Serial.print(wait_time); 
    Serial.print(", ");
    Serial.print(recv_time); 
    Serial.print(", ");
    Serial.println(total_time); 
  */
/*
    myFile.print(send_time);
    myFile.print(",");
    myFile.print(wait_time);
    myFile.print(",");
    myFile.print(recv_time);
    myFile.print(",");
    myFile.print(total_time);
    myFile.println();    
*/
    delay(DELAY_MS);
  }

  Serial.println("Done logging!");
//  myFile.close();
  while(TRUE) {}

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
//  myFile.println("Hello world!");
//  Serial.println("Wrote to file!");
//  myFile.close();
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
void canSniff20(const CAN_message_t &msg)
{ 
  t2 = micros();
  motor_temp = msg.buf[1];
  iq = (uint16_t) msg.buf[3] << 8 | msg.buf[2];
  motor_speed = (uint16_t) msg.buf[5] << 8 | msg.buf[4];
  motor_angle = (uint16_t) msg.buf[7] << 8 | msg.buf[6];
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
 // can2.mailboxStatus();
}