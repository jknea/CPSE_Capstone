#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>

#include <stdio.h>
#include <stdint.h>

int file_i2c;
int length;
unsigned char buffer[60] = {0};

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

if (write(file_i2c, buf, 1) != 1) {
  printf("Transaction failed\n");
}
if (read(file_i2c, data, 23) != 23) {
  printf("read failed\n");
}

int x = (int16_t) (((unsigned)data[15] << 8) | data[14]);
int y = (int16_t) (((unsigned)data[17] << 8) | data[16]);
int z = (int16_t) (((unsigned)data[19] << 8) | data[18]);

printf("x:%d, y:%d, z:%d\n", x, y, z);


float fx = x * 0.000061035;
float fy = y * 0.000061035;
float fz = z * 0.000061035;


printf("fx:%f, fy:%f, fz:%f\n", fx, fy, fz);


close(file_i2c);
return 0;

}
