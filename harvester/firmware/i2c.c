
#include "i2c.h"

void i2c_dly(void)
{
    _asm
    nop
    _endasm;
}

void i2c_start(void)
{
    // i2c start bit sequence
    SDA_TRIS = 1;
    i2c_dly();
    SCL_TRIS = 1;
    i2c_dly();
    SDA_PIN = SDA_TRIS = 0; //must manually set pin low, as writing to another pin
    i2c_dly();              // on the same port can set this pin high!
    SCL_PIN = SCL_TRIS = 0;
    i2c_dly();
}

void i2c_stop(void)
{
    // i2c stop bit sequence
    SDA_PIN = SDA_TRIS = 0;
    i2c_dly();
    SCL_TRIS = 1;
    i2c_dly();
    SDA_TRIS = 1;
    i2c_dly();
}

unsigned char i2c_rx(unsigned char ack)
{
    char x, d=0;
    SDA_TRIS = 1; 
    for (x=0; x<8; x++) {
        d <<= 1;
        do {
            SCL_TRIS = 1;
        } while (SCL_PIN==0);   // wait for any SCL clock stretching
        i2c_dly();
        if (SDA_PIN)
            d |= 1;
        SCL_PIN = SCL_TRIS = 0;
    }
    if (ack) SDA_PIN = SDA_TRIS = 0;
    else SDA_TRIS = 1;
    SCL_TRIS = 1;
    i2c_dly();                  // send (N)ACK bit
    SCL_PIN = SCL_TRIS = 0;
    SDA_TRIS = 1;
    return d;
}

unsigned char i2c_tx(unsigned char d)
{
    char x;
    static unsigned char bb;
    for(x=8; x; x--) {
        if (d&0x80)
            SDA_TRIS = 1;
        else
            SDA_PIN = SDA_TRIS = 0;
        SCL_TRIS = 1;
        d <<= 1;
        SCL_PIN = SCL_TRIS = 0;
    }
    SDA_TRIS = 1;
    SCL_TRIS = 1;
    i2c_dly();
    bb = SDA_PIN;               // possible ACK bit
    SCL_PIN = SCL_TRIS = 0;
    return bb;
}