#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()

#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include <sys/time.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <stdint.h>
#include <math.h> // compile with -lm


#define MAX 5
#define PORT 8888
#define SA struct sockaddr


// For WIFI
union float_union1 {
    unsigned char bytes[4];
    float flt;
};

union float_union2 {
    unsigned char bytes[8];
    float floats[2];
};


union float_union1 reply;
union float_union2 flt_un;


// CANBUS
#define ENC_MAX 65535
#define TICKS_PER_REV 589815 // enc_max * gear ratio 
#define DELTA_THRESH 20000
#define TICKS_PER_DEG 1638.375 // TICKS_PER_REV / 360.0

int ret;
int s, nbytes;
struct sockaddr_can addr;
struct ifreq ifr;
struct can_frame frame;
struct can_frame recv_frame;

long total_ticks = 0;
long prev_encoder = 0;
long curr_encoder = 0;
long delta_ticks = 0;
int iq = 0;
int temp = 0;

float motor_angle = 0.0;



// I2C
const float RAD_TO_DEG = 57.2958;
int file_i2c;
int length;
unsigned char buffer[30] = {0};  // for i2c initialization
unsigned char i2cbuf[10] = {0};
unsigned char i2cdata[23] = {0};
int x,y,z;
float fx,fy,fz;
double theta;



// forward declarations
int read_imu_i2c();
int talk_to_motor();







// to talk to WiFi Server
void func(int sockfd)
{
    printf("in func!\n");
    int msgcount = 0;
    //clock_t start_time = clock();
    //long milliseconds = 10 * 1000.0;
    //clock_t start = clock();
    int count = 0;
    while(count < 10000) {
	//printf("talking to motor\n");
	talk_to_motor();
	//printf("talkied to motor\n");
	read_imu_i2c();
	motor_angle = total_ticks / TICKS_PER_DEG;
        bzero(flt_un.bytes, 8);
        flt_un.floats[0] = theta;
        flt_un.floats[1] = motor_angle;
    	printf("sent\n");
        write(sockfd, flt_un.bytes, 8);         // write the angles to the server
        read(sockfd, reply.bytes, 4);   // read the command torque from the server
	printf("received %f\n", reply.flt);
	count++;
    }
}





int i2c_init() {
  char *filename = "/dev/i2c-1";
  if ((file_i2c = open(filename, O_RDWR)) <0) {
    printf("failed to open i2c bus\n");
    return 0;
  }
  int addr = 0x68;
  if (ioctl(file_i2c, I2C_SLAVE, addr) < 0) {
    printf("failed to acquire bus access and/or talk to the slave.\n");
    return 0;
  }
  printf("found the device\n");

  // control register specific to BMX160
  __u8 reg = 0x7E;
  __u32 res;
  char buf[10] = {0};

  usleep(100000);

  buf[0] = reg;
  buf[1] = 0x11;
  if (write(file_i2c, buf, 2) != 2) {
    printf("1\n");
    return 0;
  }
  usleep(100000);

  buf[1] = 0x15;
  if (write(file_i2c, buf, 2) != 2) {
    printf("2\n");
    return 0;
  }
  usleep(100000);

  buf[1] = 0x19;
  if (write(file_i2c, buf, 2) != 2) {
    printf("3\n");
    return 0;
  }
  usleep(100000);
  return 1;
}


int read_imu_i2c() {

  i2cbuf[0] = 0x04; // mag data addr
//  buf[1] = 0x00;

  if (write(file_i2c, i2cbuf, 1) != 1) {
    printf("Transaction failed\n");
    return 0;
  }
  if (read(file_i2c, i2cdata, 23) != 23) {
    printf("read failed\n");
    return 0;
  }

  x = (int16_t) (((unsigned)i2cdata[15] << 8) | i2cdata[14]);
  y = (int16_t) (((unsigned)i2cdata[17] << 8) | i2cdata[16]);
  z = (int16_t) (((unsigned)i2cdata[19] << 8) | i2cdata[18]);

  fx = x * 0.000061035;
  fy = y * 0.000061035;
  fz = z * 0.000061035;

//    printf("fx:%f, fy:%f, fz:%f\n", fx, fy, fz);
  theta = atan2(fx, fy) * RAD_TO_DEG;
  printf("the tilt angle is %f\n", theta);
  return 1;
}



int can_init() {
    
  memset(&frame, 0, sizeof(struct can_frame));
  system("sudo ip link set can0 type can bitrate 1000000");
  system("sudo ifconfig can0 up");
  printf("this is a can send demo\n");

  // create socket
  s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (s < 0)  {
    perror("socket PF_CAN failed");
    return 0;
  }
  printf("CAN socket success!!\n");

  // specify CAN0 device
  strcpy(ifr.ifr_name, "can0");
  ret = ioctl(s, SIOCGIFINDEX, &ifr);
  if (ret < 0) {
    perror("ioctl failed");
    return 0;
  }
  printf("ioctl success!!\n");

  // bind the socket to can
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0) {
    perror("bind failed");
    return 0;
  }
  printf("bind Success!!\n");
  
  //  frame.can_id = 0x123;
  frame.can_id = 0x141;
  frame.can_dlc = 8;
  frame.data[0] = 0xA1;
  frame.data[1] = 0;
  frame.data[2] = 0;
  frame.data[3] = 0;
  frame.data[4] = 0;
  frame.data[5] = 0;
  frame.data[6] = 0;
  frame.data[7] = 0;


  return 1;
}

int talk_to_motor() {
  printf("writing\n");
  nbytes = write(s, &frame, sizeof(frame));
  if (nbytes != sizeof(frame)) {
    printf("send error frame[0]!\r\n");
    system("sudo ifconfig can0 down");
    return 0;
  }


  printf("reading\n");
  nbytes = read(s, &recv_frame, sizeof(recv_frame));
  if (nbytes < 0) {
    perror("Read error");
    return 0;
  }
  curr_encoder = ((int)recv_frame.data[7] << 8) | recv_frame.data[6];
  iq = ((int)recv_frame.data[3] << 8) | recv_frame.data[2];
  temp = recv_frame.data[1];
  delta_ticks = curr_encoder - prev_encoder;
  if (delta_ticks > DELTA_THRESH) {
    delta_ticks = ((ENC_MAX - curr_encoder) + prev_encoder) * -1;
  }
  else if (delta_ticks < -DELTA_THRESH) {
      delta_ticks = (ENC_MAX - prev_encoder) + curr_encoder;
  }
  prev_encoder = curr_encoder;
  total_ticks += delta_ticks;
//  printf("0x%03X {%d} ", recv_frame.can_id, recv_frame.can_dlc);
//  for (int i = 0; i < recv_frame.can_dlc; ++i) {
//    printf("%02X ", recv_frame.data[i]);
//  }
//  printf("\n");
  return 1;

}



int main(){

  
      // initialize I2C
  if (i2c_init()) {
    printf("I2C init SUCCESS!\n");
  }
  else {
    printf("I2C failed!!!");
    return 1;
  }
  
  // initialize CAN
  if (can_init()) {
    printf("CAN init SUCCESS!\n");
  }
  else {
    printf("CAN failed!!!");
    return 1;
  }
  

  
  
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else {
        printf("Socket successfully created..\n");
    }

    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    const char* host = "192.168.5.1";
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(host);
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
        != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
  
    // function for chat
    func(sockfd);

    // close the socket
    close(sockfd);
    return 0;
}
