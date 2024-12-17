#include <stdio.h>
#include <stdlib.h>
#include <pigpiod_if2.h>

// spi channel
#define SPICH 0
#define SPISPEED 1000000  // sampling rate

int ad_read(int pd, int fd, int ch);


int main(){
  int fd, ch, i;
  int pd;
  pd = pigpio_start(NULL, NULL);
  if (pd < 0){
    printf("Failed to connect pigpiod\n");
    exit(EXIT_FAILURE);
  }

  fd = spi_open(pd, SPICH, SPISPEED, 0);
  if (fd < 0){
    printf("Failed to initialize SPI\n");
    exit(EXIT_FAILURE);
  }

  ch = 0;
  while (ch >= 0){
    printf("Trying to load A/D converted value from mpc3008\n");
    printf("Input channel (0-7, otherwise exits)\n");
    scanf("\n%d", &ch);
    if (ch > 7){
      for (i = 0; i < 8; i++){
        printf("ch%d = %d\n", i, ad_read(pd, fd, i));
      }
    } else if (ch >= 0){
      while (1){
        printf("ch%d = %d\n", ch, ad_read(pd, fd, ch));
        time_sleep(0.3);
      }
    }
    spi_close(pd, fd);
    pigpio_stop(pd);
    return 0;
  } 
}

int ad_read(int pd, int fd, int ch){
  unsigned char wdata[3], rdata[3];
  int val;

  wdata[0] = 0x1;
  wdata[1] = 128;
  printf("wdata=============\n");
  printf("%d\n", wdata[0]);
  printf("%d\n", wdata[1]);
  printf("%d\n", wdata[2]);
  spi_xfer(pd, fd, wdata, rdata, 3);
  // printf("2!\n");
  // printf("%x\n", rdata[0]);
  // printf("%x\n", rdata[1]);
  // printf("%x\n", rdata[2]);

  val = rdata[2];
  // printf("val %d\n", val);
  // printf("val %d\n", (rdata[1] & 0x03) << 8);
  val += (rdata[1] & 0x03) << 8;
  return val;
}
