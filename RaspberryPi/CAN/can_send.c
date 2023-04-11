#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
  int ret;
  int s, nbytes;
  struct sockaddr_can addr;
  struct ifreq ifr;
  struct can_frame frame;
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


  setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
//  frame.can_id = 0x123;
  frame.can_id = 0x150;
  frame.can_dlc = 8;
  frame.data[0] = 'a';
  frame.data[1] = 'b';
  frame.data[2] = 'c';
  frame.data[3] = 'd';
  frame.data[4] = 'e';
  frame.data[5] = 'f';
  frame.data[6] = 'g';
  frame.data[7] = 'h';

  printf("can_id = 0x%X\r\n", frame.can_id);
  printf("can_dlc = %d\r\n", frame.can_dlc);
  nbytes = write(s, &frame, sizeof(frame));
  if (nbytes != sizeof(frame)) {
    printf("send error frame[0]!\r\n");
    system("sudo ifconfig can0 down");
  }
  else {
    printf("send Success!\n");
  }


  close(s);
  system("sudo ifconfig can0 down");
  return 0;
}
