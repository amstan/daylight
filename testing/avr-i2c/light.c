#include "light.h"
#include "i2cmaster.h"
#include "bitop.h"

int light_init(void) {
	i2c_init();
	return light_write(0x00,0x03);
}

unsigned char light_read(unsigned char reg) {
	reg|=0b10100000;
	char ret;
	ret = i2c_start(I2C_ADD+I2C_WRITE);
	if ( ret ) {
		i2c_stop();
		return -1;
	} else {
		i2c_write(reg); //location
		i2c_rep_start(I2C_ADD+I2C_READ);
		ret = i2c_readNak(); //read the data
		i2c_stop();
	}
	return ret;
}

int light_write(unsigned char reg,unsigned char data) {
	reg|=0b10100000;
	char ret;
	ret = i2c_start(I2C_ADD+I2C_WRITE);
	if ( ret ) {
		i2c_stop();
		return -1;
	} else {
		i2c_write(reg);  //location
		i2c_write(data); //data
		i2c_stop();
	}
	return ret;
}

unsigned int light_level(unsigned char reg) {
	return light_read(reg+1)*256+light_read(reg);
}

unsigned char light_ready(void) {
	return test_bit(light_read(0x00),4);
}