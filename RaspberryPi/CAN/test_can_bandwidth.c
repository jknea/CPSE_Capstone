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

int main() {

  system("sudo ifconfig can0 down");


  int num;
  FILE *fptr;
  fptr = fopen("CAN_SPI_10M.txt", "w");


  int ret;
  int s, nbytes;
  struct sockaddr_can addr;
  struct ifreq ifr;
  struct can_frame frame;
  struct can_frame recv_frame;

  memset(&frame, 0, sizeof(struct can_frame));
  system("sudo ip link set can0 type can bitrate 1000000");
  system("sudo ifconfig can0 up");
  printf("this is a can send demo\n");

  // create socket
  s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
  if (s < 0)  {
    perror("socket PF_CAN failed");
    return 1;
  }
  printf("CAN socket success!!\n");

  // specify CAN0 device
  strcpy(ifr.ifr_name, "can0");
  ret = ioctl(s, SIOCGIFINDEX, &ifr);
  if (ret < 0) {
    perror("ioctl failed");
  }
  printf("ioctl success!!\n");

  // bind the socket to can
  addr.can_family = AF_CAN;
  addr.can_ifindex = ifr.ifr_ifindex;
  ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0) {
    perror("bind failed");
  }
  printf("bind Success!!\n");


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

  printf("can_id = 0x%X\r\n", frame.can_id);
  printf("can_dlc = %d\r\n", frame.can_dlc);

//  time_t begin = time(NULL);
  struct timeval start, end;

  float max_time = 0.0;
  float time_sum = 0.0;

  int iterations = 1000;
  for (int i = 0; i < iterations; ++i) {

    nbytes = write(s, &frame, sizeof(frame));
    if (nbytes != sizeof(frame)) {
      printf("%d\n", nbytes);
      printf("send error frame[0]!\r\n");
//      system("sudo ifconfig can0 down");
    }
    else {
      printf("send Success!\n");
    }
    gettimeofday(&start, NULL);



    nbytes = read(s, &recv_frame, sizeof(struct can_frame));
    if (nbytes < 0) {
      perror("Read error");
      return 1; 
    }
    gettimeofday(&end, NULL);
    long seconds = (end.tv_sec - start.tv_sec);
    long micros = ((seconds*1000000) + end.tv_usec) - start.tv_usec;
    float elapsed_time = micros/1000000.0;

//    printf("send took %f seconds\n", elapsed_time);
    if (elapsed_time > max_time) {
      max_time = elapsed_time;
    }
    time_sum += elapsed_time;
    fprintf(fptr, "%f", elapsed_time);
    fprintf(fptr, "%c", ',');

    usleep(1000);

  }
  fclose(fptr);

  printf("Average time was %f seconds\n", time_sum/iterations);
  printf("Maximum time was %f seconds\n", max_time);


  close(s);
  system("sudo ifconfig can0 down");
  return 0;
}
