/*
 
 Using Tindie development board for Teensy4.1

 created 18 Dec 2009 by David A. Mellis
 modified 9 Apr 2012 by Tom Igoe, based on work by Adrian McEwen
 modified 5 Apr 2023 by James Knea, for use with my H.A.L.O. project

 */

#include <SPI.h>
#include <NativeEthernet.h>

#define BUFF_SIZE 256
#define PORT      8888
#define DELAY_MS  1

uint8_t buffer[BUFF_SIZE] = {0};

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192,168,5,1); // IP address of the RPi WiFi AP

// Initialize the Ethernet client library
EthernetClient client;

// Variables to measure the speed
unsigned long t0, dt;

// variables for writing data
int bytes_written = 0;


/* file variables */
File myFile;
String SD_data;
char fname[FILENAME_MAX_LEN + TXT_EXT_WITH_NULL_CHAR_LEN] = {0};
char c;
int idx = 0;
bool filename_received = 0;
String instr = "";



void setup() {
  Serial.begin(115200);
  delay(2000);

  instr.reserve(13);
  initSD();    // MUST USE A FAT32 FORMATTED SD CARD, AND MAKE SURE TO EJECT PROPERLY (more important to keep character count within limit specified above);
  delay(2000);

  initETH();
  
  init_buf(buffer, 256);
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

void init_buf(uint8_t *buf, int n)
{
  for (int i = 0; i < n; i++) {
    buf[i] = i;
  }
}


void loop() {
  if (client.connected())
  {
    t0 = micros();
    bytes_written = client.write(buffer, BUFF_SIZE);  // write data to the server
    dt = micros() - t0;
    delay(DELAY_MS);
  }
  else
  {
    while(1)
    {
      delay(DELAY_MS);
    }
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



