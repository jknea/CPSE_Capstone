#include <SD.h>
#include "LIS3DHTR.h"
#include <Wire.h>
LIS3DHTR<TwoWire> LIS; //IIC


#define SERIAL_BAUD                   115200
#define TRUE                          1
#define FILENAME_MAX_LEN              8
#define TXT_EXT_WITH_NULL_CHAR_LEN    5
#define LOG_ITERATIONS                1000
#define DELAY_MS                      1

#define WIRE Wire

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
  Serial.begin(115200);
  delay(2000);
  LIS.begin(WIRE, 0x19); //SPI SS/CS
  //LIS.openTemp();//If ADC3 is used, the temperature detection needs to be turned off.
  //  LIS.closeTemp();//default
  delay(100);
  LIS.setFullScaleRange(LIS3DHTR_RANGE_2G);
  //  LIS.setFullScaleRange(LIS3DHTR_RANGE_4G);
  //  LIS.setFullScaleRange(LIS3DHTR_RANGE_8G);
  //  LIS.setFullScaleRange(LIS3DHTR_RANGE_16G);
  //  LIS.setOutputDataRate(LIS3DHTR_DATARATE_1HZ);
  //  LIS.setOutputDataRate(LIS3DHTR_DATARATE_10HZ);
  //  LIS.setOutputDataRate(LIS3DHTR_DATARATE_25HZ);
  //LIS.setOutputDataRate(LIS3DHTR_DATARATE_50HZ);
  //  LIS.setOutputDataRate(LIS3DHTR_DATARATE_100HZ);
  //  LIS.setOutputDataRate(LIS3DHTR_DATARATE_200HZ);
    LIS.setOutputDataRate(LIS3DHTR_DATARATE_1_6KH);
  //  LIS.setOutputDataRate(LIS3DHTR_DATARATE_5KHZ);
  //delay(3000);
  instr.reserve(13);
  initSD();    
}

float ax, ay, az;
unsigned long t0, t1, dt;


void loop() {
  for (int i = 0; i < 1000; i++)
  {
    while (!(LIS.available())) {wait(1);}
    t0 = micros();
    LIS.getAcceleration(&ax, &ay, &az);
    t1 = micros();
    dt = t1 - t0;
    myFile.print(dt);
    myFile.print(",");
    delay(1);
  }
  Serial.println("done!");
  myFile.close();
  while(1) {}
}



void wait(unsigned us) {
  unsigned long T0 = micros();
  while ((micros() - T0) < us);
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
