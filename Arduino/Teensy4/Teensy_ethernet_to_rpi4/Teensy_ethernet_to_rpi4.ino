/*

 Using Tindie development board for Teensy4.1

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe, based on work by Adrian McEwen
 modified 5 Apr 2023
 by James Knea, for use with my H.A.L.O. project
 */

#include <SPI.h>
#include <NativeEthernet.h>

#define PORT  8888

uint8_t buffer[256] = {0};

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server(192,168,5,1); // IP address of the RPi WiFi AP

// Initialize the Ethernet client library
EthernetClient client;

// Variables to measure the speed
unsigned long t0, dt;

void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

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

  init_buf(buffer, 256);
}

void init_buf(uint8_t *buf, int n)
{
  for (int i = 0; i < n; i++) {
    buf[i] = i;
  }
}

int n = 0;

void loop() {
  t0 = micros();
  n = client.write(buffer, 256);  // write data to the server
  dt = micros() - t0;
//  if (n != 256) {
 //   client.flush()
 //   Serial.println("Did not write 256 bytes, exiting...");
 //   while(1);
 // }
  /*
  int len = client.available();
  if (len > 0) {
    byte buffer[80];
    if (len > 80) len = 80;
    client.read(buffer, len);
    if (printWebData) {
      Serial.write(buffer, len); // show in the serial monitor (slows some boards)
    }
    byteCount = byteCount + len;
  }
  */
  delay(1000);
}

