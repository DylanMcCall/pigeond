#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include "pdf417lib.h"
#include <dmtx.h>

#define adalogo_width  75
#define adalogo_height 75

static const unsigned char adalogo_data[] = {
  0x00,0x00,0x00,0x00,0x00,0xe0,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x01,0xf0,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x03,0xf0,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x03,0xf8,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x07,0xf8,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x0f,0xf8,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x1f,0xfc,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x1f,0xfc,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x3f,0xfc,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x7f,0xfe,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x7f,0xfe,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0xff,0xfe,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x01,0xff,0xff,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x03,0xff,0xff,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x03,0xff,0xff,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x07,0xff,0xff,0x80,0x00,0x00,0x00,
  0x00,0x00,0x00,0x07,0xff,0xff,0x80,0x00,0x00,0x00,
  0x00,0x00,0x00,0x07,0xff,0xff,0x80,0x00,0x00,0x00,
  0x00,0x00,0x00,0x0f,0xff,0xff,0x80,0x00,0x00,0x00,
  0x00,0x00,0x00,0x0f,0xff,0xff,0x80,0x00,0x00,0x00,
  0x7f,0xff,0xfc,0x0f,0xff,0xff,0x80,0x00,0x00,0x00,
  0xff,0xff,0xff,0x0f,0xff,0xff,0x80,0x00,0x00,0x00,
  0xff,0xff,0xff,0xcf,0xff,0xff,0x80,0x00,0x00,0x00,
  0xff,0xff,0xff,0xef,0xff,0xff,0x80,0x00,0x00,0x00,
  0x7f,0xff,0xff,0xf7,0xff,0xff,0x80,0x00,0x00,0x00,
  0x3f,0xff,0xff,0xff,0xfb,0xff,0x00,0x00,0x00,0x00,
  0x3f,0xff,0xff,0xff,0xf1,0xff,0x3f,0xf0,0x00,0x00,
  0x1f,0xff,0xff,0xff,0xf1,0xfe,0xff,0xfe,0x00,0x00,
  0x0f,0xff,0xff,0xff,0xf1,0xff,0xff,0xff,0xc0,0x00,
  0x0f,0xff,0xff,0xff,0xe1,0xff,0xff,0xff,0xf8,0x00,
  0x07,0xff,0xff,0xff,0xe1,0xff,0xff,0xff,0xff,0x00,
  0x03,0xff,0xff,0xff,0xe1,0xff,0xff,0xff,0xff,0xc0,
  0x01,0xff,0xff,0x3f,0xe1,0xff,0xff,0xff,0xff,0xe0,
  0x01,0xff,0xfe,0x07,0xe3,0xff,0xff,0xff,0xff,0xe0,
  0x00,0xff,0xff,0x03,0xe3,0xff,0xff,0xff,0xff,0xe0,
  0x00,0x7f,0xff,0x00,0xf7,0xff,0xff,0xff,0xff,0xc0,
  0x00,0x3f,0xff,0xc0,0xff,0xc0,0x7f,0xff,0xff,0x80,
  0x00,0x1f,0xff,0xf0,0xff,0x00,0x3f,0xff,0xff,0x00,
  0x00,0x0f,0xff,0xff,0xff,0x00,0x7f,0xff,0xfc,0x00,
  0x00,0x07,0xff,0xff,0xff,0x01,0xff,0xff,0xf8,0x00,
  0x00,0x01,0xff,0xff,0xff,0xff,0xff,0xff,0xf0,0x00,
  0x00,0x00,0x7f,0xff,0xff,0xff,0xff,0xff,0xc0,0x00,
  0x00,0x00,0x1f,0xfc,0x7f,0xff,0xff,0xff,0x80,0x00,
  0x00,0x00,0x7f,0xf8,0x78,0xff,0xff,0xfe,0x00,0x00,
  0x00,0x00,0xff,0xf0,0x78,0x7f,0xff,0xfc,0x00,0x00,
  0x00,0x01,0xff,0xe0,0xf8,0x7f,0xff,0xf0,0x00,0x00,
  0x00,0x03,0xff,0xc0,0xf8,0x3f,0xdf,0xc0,0x00,0x00,
  0x00,0x07,0xff,0xc1,0xfc,0x3f,0xe0,0x00,0x00,0x00,
  0x00,0x07,0xff,0x87,0xfc,0x1f,0xf0,0x00,0x00,0x00,
  0x00,0x0f,0xff,0xcf,0xfe,0x1f,0xf8,0x00,0x00,0x00,
  0x00,0x0f,0xff,0xff,0xff,0x1f,0xf8,0x00,0x00,0x00,
  0x00,0x1f,0xff,0xff,0xff,0x1f,0xfc,0x00,0x00,0x00,
  0x00,0x1f,0xff,0xff,0xff,0xff,0xfc,0x00,0x00,0x00,
  0x00,0x1f,0xff,0xff,0xff,0xff,0xfe,0x00,0x00,0x00,
  0x00,0x3f,0xff,0xff,0xff,0xff,0xfe,0x00,0x00,0x00,
  0x00,0x3f,0xff,0xff,0xff,0xff,0xfe,0x00,0x00,0x00,
  0x00,0x3f,0xff,0xff,0x3f,0xff,0xfe,0x00,0x00,0x00,
  0x00,0x7f,0xff,0xff,0x3f,0xff,0xfe,0x00,0x00,0x00,
  0x00,0x7f,0xff,0xff,0x3f,0xff,0xfe,0x00,0x00,0x00,
  0x00,0x7f,0xff,0xfe,0x3f,0xff,0xfe,0x00,0x00,0x00,
  0x00,0xff,0xff,0xfc,0x1f,0xff,0xfe,0x00,0x00,0x00,
  0x00,0xff,0xff,0xf8,0x1f,0xff,0xfe,0x00,0x00,0x00,
  0x00,0xff,0xff,0xe0,0x0f,0xff,0xfe,0x00,0x00,0x00,
  0x01,0xff,0xff,0x80,0x07,0xff,0xfe,0x00,0x00,0x00,
  0x01,0xff,0xfc,0x00,0x03,0xff,0xfe,0x00,0x00,0x00,
  0x01,0xff,0xe0,0x00,0x01,0xff,0xfe,0x00,0x00,0x00,
  0x01,0xff,0x00,0x00,0x00,0xff,0xfe,0x00,0x00,0x00,
  0x00,0xf8,0x00,0x00,0x00,0x7f,0xfe,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x1f,0xfe,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x0f,0xfe,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x07,0xfe,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x01,0xfe,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x1c,0x00,0x00,0x00
};

void scaleByte(char* newBytes, char oldByte, int scale)
{
  int bitIndex = 0;
  int spotInArray = 0;
  memset(newBytes, 0, scale);
  //8 bits to a byte
  for(int i = 0; i < 8; i++) {
    //The ith bit is set
    int isSet = 0;
    if((oldByte & (1 << i)) != 0) {
        isSet = 1;
    }
    for(int j = 0; j < scale; j++) {
      newBytes[(scale - spotInArray) - 1] = newBytes[(scale - spotInArray) - 1] | (isSet << bitIndex % 8);
      bitIndex++;
      if (bitIndex % 8 == 0) {
        spotInArray++;
      }
    }
  }
}

//Scale must be less than 8!
void printBitmap(char* input, int rows, int columns, int scale)
{
  //Need to know the number of bytes a column will take, so round up to nearest multiple of 8
  int roundedColumns = (columns + 7)/8;

  int scaledColumns = scale * roundedColumns;

  //The printer has a maximum width of 384 (8*48) pixels
  if(scaledColumns > 48) {
    printf("The image is too wide to print. Please use a value less than 384\n");
    return;
  }

  int fd; /* File descriptor for the printer*/
  
  fd = open("/dev/ttyO5", O_WRONLY | O_NOCTTY | O_NDELAY);
  //fd = open("/dev/ttyO0", O_WRONLY);
  //fd = open("test.txt", O_WRONLY);
  if (fd == -1){
    printf("%s\n", strerror(errno));  
  }
  else {
    fcntl(fd, F_SETFL, 0);
  }

  struct timespec pollingDelay = {0, 50000000};

  write(fd, "\n", 1);
  
  for (int i = 0; i < rows; i++) {
    //Each row is printed scale times
    for(int j = 0; j < scale; j++) {
      
      //Tell the printer we are starting a new row
      write(fd, "\x12", 1);
      write(fd, "*", 1);
      //1 row at a time, so number of rows is 1
      write(fd, "\x1", 1);
      //Always padding up to maximum width, so width is always 0x30 or 48
      write(fd, "\x30", 1);

      for (int k = 0; k < 48; k++) {
        //Each pixel is printed scale times
        char *newBytes;
        newBytes = (char*)malloc(scale);
        scaleByte(newBytes, input[i * roundedColumns + k], scale);
        for(int l = 0; l < scale; l++) {
          //Pad any image less than 384 pixels with whitespace
          if (k >= roundedColumns) {
            write(fd, "\x00", 1);
          }
          else {
            write(fd, &newBytes[l], 1);  
          }
        }
        free(newBytes); 
      }
      nanosleep(&pollingDelay, (struct timespec *) NULL);
    }
  }

  write(fd, "\n", 1);
  close(fd);
}


void printPDF417Barcode(char *input) 
{
  pdf417param barcode;
  pdf417init(&barcode);
  barcode.text = input;
  barcode.codeColumns = 1;
  barcode.options = PDF417_FIXED_COLUMNS;
  paintCode(&barcode);

  printBitmap(barcode.outBits, barcode.codeRows, barcode.bitColumns, 4);

  pdf417free(&barcode);

}

void make1bitBitmap(char* output, unsigned char *input, int width, int height, int bytesPerPixel)
{
  int whatByte = 0;
  int whatBit = 0;

  for(int i = 0; i < height*bytesPerPixel; i += bytesPerPixel) {
    for(int j = 0; j < width*bytesPerPixel; j += bytesPerPixel) {
      //Pixel is black
      if(!(input[i * width + j] & 0xff)) {
        output[whatByte] += (1 << (7-whatBit));
      }
      whatBit++;
      if(whatBit % 8 == 0) {
        whatBit = 0;
        whatByte++;
      }
    }
    if(whatBit % 8 != 0) {
      whatByte++;
    }
    whatBit = (whatBit + 7)/8;
  }
}

void printString(char* input)
{
  int fd; /* File descriptor for the printer*/
  
  fd = open("/dev/ttyO5", O_WRONLY | O_NOCTTY | O_NDELAY);
  //fd = open("test.txt", O_WRONLY);
  if (fd == -1){
    printf("%s\n", strerror(errno));  
  }

  write(fd, "\n", 1);
  for(int i = 0; i < strlen(input); i++) {
    write(fd, input + i, 1);
  }
  write(fd, "\n", 1);
  close(fd);
}

void printDataMatrix(char* input)
{
  size_t width, height, bytesPerPixel;
  unsigned char *pxl;
  DmtxEncode *enc;

  /* 1) ENCODE a new Data Matrix barcode image (in memory only) */

  enc = dmtxEncodeCreate();
  dmtxEncodeDataMatrix(enc, strlen(input), (unsigned char *)input);

  /* 2) COPY the new image data before releasing encoding memory */

  width = dmtxImageGetProp(enc->image, DmtxPropWidth);
  height = dmtxImageGetProp(enc->image, DmtxPropHeight);
  bytesPerPixel = dmtxImageGetProp(enc->image, DmtxPropBytesPerPixel);

  printf("Width %d Height %d BytesPerPixel %d\n", width, height, bytesPerPixel);

  pxl = (unsigned char *)malloc(width * height * bytesPerPixel);
  memcpy(pxl, enc->image->pxl, width * height * bytesPerPixel);

  //Sadly though the library says it supports 1 bit per pixel bitmaps. If you try to use it, 
  //it fails on an assertion. On reading the code it actually says within the code that it is 
  //not supported. So transform the bitmap into a 1 bit per pixel bitmap
  char *pxl1d = (char *)malloc( (width + 7)/8 * height );
  make1bitBitmap(pxl1d, pxl, width, height, bytesPerPixel);
  
  //printf("%s\n", input);
  printBitmap(pxl1d, height,width, 1);

  //Cleanup!
  dmtxEncodeDestroy(&enc);
  free(pxl);
  free(pxl1d);
}

void printTestBitmap()
{
  int fd; /* File descriptor for the printer*/
  
  fd = open("/dev/ttyO5", O_WRONLY | O_NOCTTY | O_NDELAY);
  //fd = open("test.txt", O_WRONLY);
  if (fd == -1){
    printf("%s\n", strerror(errno));  
  }

  struct timespec pollingDelay = {0, 50000000};

  write(fd, "\n", 1);
  
  for (int i = 0; i < adalogo_height; i++) {
    write(fd, "\x12", 1);
    write(fd, "*", 1);
    write(fd, "\x1", 1);
    write(fd, "\x30", 1);
    for (int j = 0; j < 48; j++) {
      if (j >= 10) {
        write(fd, "\x00", 1);
      }
      else {
        write(fd, &adalogo_data[i * 10 + j], 1);  
      } 
    }
    nanosleep(&pollingDelay, (struct timespec *) NULL);
  }

  write(fd, "\n", 1);
  close(fd);
}