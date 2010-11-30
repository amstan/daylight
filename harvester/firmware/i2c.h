#ifndef I2C_H
#define I2C_H

#include "pic/pic16f88.h"

#define SCL_PIN     RB0
#define SCL_TRIS    TRISB0
#define SDA_PIN     RB1
#define SDA_TRIS    TRISB1

void i2c_dly(void);
void i2c_start(void);
void i2c_stop(void);
unsigned char i2c_rx(unsigned char);
unsigned char i2c_tx(unsigned char);

#endif