#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

//#include <time.h>
#include <sys/time.h>

//#include <unistd.h>
#include <fcntl.h>
//#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
//#include <stdio.h>
#include <stdint.h>
#include <math.h> // compile with -lm


// for I2C IMU
const float RAD_TO_DEG = 57.2958;
int file_i2c;
int length;
unsigned char buffer[30] = {0};


  // CANBUS
  int ret;
  int s, nbytes;
  struct sockaddr_can addr;
  struct ifreq ifr;
  struct can_frame frame;
  struct can_frame recv_frame;


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


  buf[0] = reg;
  buf[1] = 0x11;
  if (write(file_i2c, buf, 2) != 2) {
    return 0;
  }
  usleep(100000);

  buf[1] = 0x15;
  if (write(file_i2c, buf, 2) != 2) {
    return 0;
  }
  usleep(100000);

  buf[1] = 0x19;
  if (write(file_i2c, buf, 2) != 2) {
    return 0;
  }
  usleep(100000);
  return 1;
}


int read_imu_i2c() {
  unsigned char buf[10] = {0};
  unsigned char data[23] = {0};
  int x,y,z;
  float fx,fy,fz;
  double theta;

  buf[0] = 0x04; // mag data addr
//  buf[1] = 0x00;

  if (write(file_i2c, buf, 1) != 1) {
    printf("Transaction failed\n");
    return 0;
  }
  if (read(file_i2c, data, 23) != 23) {
    printf("read failed\n");
    return 0;
  }

  x = (int16_t) (((unsigned)data[15] << 8) | data[14]);
  y = (int16_t) (((unsigned)data[17] << 8) | data[16]);
  z = (int16_t) (((unsigned)data[19] << 8) | data[18]);

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
  return 1;
}

int talk_to_motor() {
  nbytes = write(s, &frame, sizeof(frame));
  if (nbytes != sizeof(frame)) {
    printf("send error frame[0]!\r\n");
    system("sudo ifconfig can0 down");
    return 0;
  }
  nbytes = read(s, &recv_frame, sizeof(struct can_frame));
  if (nbytes < 0) {
    perror("Read error");
    return 0;
  }
  printf("0x%03X {%d} ", recv_frame.can_id, recv_frame.can_dlc);
  for (int i = 0; i < recv_frame.can_dlc; ++i) {
    printf("%02X ", recv_frame.data[i]);
  }
  printf("\n");
  return 1;
}


int main() {
  
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
  
  
  
  

/*
  for (int i = 0; i < 1; ++i) {
    if (read_imu_i2c()) {
    }
    else {
      printf("read IMU error\n");
      return 1;
    }
    usleep(10000);
  }
*/


  
  










  //setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
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

//  printf("can_id = 0x%X\r\n", frame.can_id);
//  printf("can_dlc = %d\r\n", frame.can_dlc);

//  struct timeval start, end;
//  gettimeofday(&start, NULL);


  // write & read CAN frame
  talk_to_motor();
  read_imu_i2c();
//    usleep(500); // this also works
//    usleep(1000); // this works, but go too fast and we get error
//    usleep(10000);
//  }



//  printf("done!!\n");
//  gettimeofday(&end, NULL);
//  long seconds = (end.tv_sec - start.tv_sec);
//  long micros = ((seconds*1000000) + end.tv_usec) - start.tv_usec;
//  printf("elapsed time is %f seconds\n", micros/1000000.0);

  close(s);
  system("sudo ifconfig can0 down");

  close(file_i2c);


  return 0;
}

