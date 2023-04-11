#include <SD.h>

#define SERIAL_BAUD                   115200
#define TRUE                          1
#define FILENAME_MAX_LEN              8
#define TXT_EXT_WITH_NULL_CHAR_LEN    5

/* file variables */
File myFile;
String SD_data;
char fname[FILENAME_MAX_LEN + TXT_EXT_WITH_NULL_CHAR_LEN] = {0};
char c;
int idx = 0;
bool filename_received = 0;
String instr = "";

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(3000);
  instr.reserve(13);
  init_sd();                               // MUST USE A FAT32 FORMATTED SD CARD, AND MAKE SURE TO EJECT PROPERLY (more important to keep character count within limit specified above);
}


void loop() 
{
}


void init_sd() {
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
  myFile.println("Hello world!");
  Serial.println("Wrote to file!");
  myFile.close();
}


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

