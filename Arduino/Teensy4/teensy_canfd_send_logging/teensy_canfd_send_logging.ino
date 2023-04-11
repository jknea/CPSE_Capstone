/*
This code is for the Teensy acting as the motor control hub which
also talks to the IMU hub Teensies
*/
#include <SD.h>
#include <FlexCAN_T4.h>
#include <elapsedMillis.h>


#define SERIAL_BAUD                   115200
#define TRUE                          1
#define FILENAME_MAX_LEN              8
#define TXT_EXT_WITH_NULL_CHAR_LEN    5
#define LOG_ITERATIONS                1000
#define DELAY_MS                      1


FlexCAN_T4FD<CAN3, RX_SIZE_256, TX_SIZE_128> can;
elapsedMillis t_ms;
CANFD_message_t msg;


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


void setup() {

  // sending message info
  msg.id = 0x200;
  msg.len = 64;

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


  can.setMBFilter(MB0, 0x300); // to receive message
  can.onReceive(MB0, isr);


  can.enableMBInterrupts();


  Serial.begin(115200);
  delay(1000);
  Serial.println("logger started!");
  initSD();
}


void loop() 
{
  for (int i = 0; i < LOG_ITERATIONS; i++) {
    t0 = micros();
    for (int i = 0; i < 64; ++i) 
    {
      msg.buf[i] = 0;
    }
    can.write(msg);
    t1 = micros();
    while(!received) { can.events() ; }
    t3 = micros();
    received = 0;
    send_time = t1 - t0;
    wait_time = t2 - t1;
    recv_time = t3 - t2;
    total_time = t3 - t0;


    myFile.print(send_time);
    myFile.print(",");
    myFile.print(wait_time);
    myFile.print(",");
    myFile.print(recv_time);
    myFile.print(",");
    myFile.print(total_time);
    myFile.println();    

    delay(DELAY_MS);
  }

  Serial.println("Done logging!");
  myFile.close();
  while(TRUE) {}

}

int x = 0;

void isr(const CANFD_message_t &msg) {
  t2 = micros();
//  Serial.print("MB "); Serial.print(msg.mb);
//  Serial.print("  OVERRUN: "); Serial.print(msg.flags.overrun);
//  Serial.print("  LEN: "); Serial.print(msg.len);
//  Serial.print(" EXT: "); Serial.print(msg.flags.extended);
//  Serial.print(" TS: "); Serial.print(msg.timestamp);
//  Serial.print(" ID: "); Serial.print(msg.id, HEX);
//  Serial.print(" Buffer: ");
//  Serial.println();
  for ( uint8_t i = 0; i < msg.len; i++ ) {
    x = msg.buf[i];
//    Serial.print(msg.buf[i], HEX); Serial.print(" ");
  } 
//  Serial.println();
  received = 1;
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
