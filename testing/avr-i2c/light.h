#ifndef LIGHT_H
#define LGIHT_H   1

#include "i2cmaster.h"

//Library for the use with TCS3404 DIGITAL COLOR LIGHT SENSOR

#define I2C_ADD 0b01110010

#define LIGHT_RED 0x12
#define LIGHT_GREEN 0x10
#define LIGHT_BLUE 0x14
#define LIGHT_CLEAR 0x16

int light_init(void);
unsigned char light_read(unsigned char reg);
int light_write(unsigned char reg,unsigned char data);
unsigned int light_level(unsigned char reg);
unsigned char light_ready(void);

#endif