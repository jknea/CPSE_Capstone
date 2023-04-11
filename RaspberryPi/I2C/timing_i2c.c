#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>

#include <stdio.h>
#include <stdint.h>

#include <math.h>

#include <sys/time.h>

const float RAD_TO_DEG = 57.2958;

int file_i2c;
int length;
unsigned char buffer[30] = {0};

//  time_t begin = time(NULL);
  struct timeval start, end;

int main() {

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


__u8 reg = 0x7E;
__u32 res;
  char buf[10] = {0};

/*
res = i2c_smbus_write_word_data(file_i2c, 0x7E, 0x11);
printf("res = %d\n", res);
usleep(100000);
res = i2c_smbus_write_word_data(file_i2c, 0x7E, 0x15);
printf("res = %d\n", res);
usleep(100000);
res = i2c_smbus_write_word_data(file_i2c, 0x7E, 0x19);
printf("res = %d\n", res);
usleep(100000);
*/

  buf[0] = reg;
  buf[1] = 0x11;
  if (write(file_i2c, buf, 2) != 2) {
    printf("Transaction failed\n");
  }
  printf("success\n");
  usleep(100000);

  buf[1] = 0x15;
  if (write(file_i2c, buf, 2) != 2) {
    printf("Transaction failed\n");
  }
  printf("success\n");
  usleep(100000);

  buf[1] = 0x19;
  if (write(file_i2c, buf, 2) != 2) {
    printf("Transaction failed\n");
  }
  printf("success\n");
  usleep(100000);


  buf[0] = 0x04; // mag data addr
  buf[1] = 0x00;
  unsigned char data[23] = {0};

  // request imu data

  int x, y, z;
  double fx, fy, fz;

  float time_sum = 0.0;

  for (int i = 0; i < 1000; ++i) {
    gettimeofday(&start, NULL);


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
    double theta = atan2(fx, fy) * RAD_TO_DEG;

    gettimeofday(&end, NULL);
    long seconds = (end.tv_sec - start.tv_sec);
    long micros = ((seconds*1000000) + end.tv_usec) - start.tv_usec;
    float elapsed_time = micros/1000000.0;
    printf("Elapsed time is %f\n", elapsed_time);
    time_sum += elapsed_time;


    printf("the tilt angle is %f\n", theta);
    usleep(10000);
  }

  printf("average elapsed time is %f\n", time_sum/1000);

  close(file_i2c);
  return 0;
}
