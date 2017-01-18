#include <stdio.h>
#include <cstring>
#include <cassert>
#include <bcm2835.h>
#include "i2c.h"

#define check(v) void();
//#define check(v) assert(v != BCM2835_I2C_REASON_ERROR_NACK) //bug? no ack from mpu6050
//#define check(v) assert(v != BCM2835_I2C_REASON_OK)

void i2cOpen() {
	if (!bcm2835_init()) {
		printf("bcm2835_init failed. Are you running as root??\n");
		return;
	}

	if (!bcm2835_i2c_begin()) {
		printf("bcm2835_i2c_begin failed. Are you running as root??\n");
		return;
	}

	bcm2835_i2c_setClockDivider(BCM2835_I2C_CLOCK_DIVIDER_626); //400kHz
}

void i2cClose() {
	bcm2835_i2c_end();
}

void i2cSetAddress(char address) {
	bcm2835_i2c_setSlaveAddress(address);
}

char wbuf[32];
char rbuf[32];

char i2cRead(char reg) {
	
	uint8_t rval;
	//memset(wbuf, 0, sizeof(wbuf));
	//memset(rbuf, 0, sizeof(rbuf));

	wbuf[0] = reg;
	rval = bcm2835_i2c_write(wbuf, 1);
	check(rval);
	rval = bcm2835_i2c_read(rbuf, 1);
	check(rval);
	return rbuf[0];
}

void i2cWrite(char reg, char data) {
	uint8_t rval;
	//memset(wbuf, 0, sizeof(wbuf));

	wbuf[0] = reg;
	wbuf[1] = data;
	rval = bcm2835_i2c_write(wbuf, 2);
	check(rval);
}

void i2cRead(char reg, char ret[], uint32_t len) {
	uint8_t rval;

	wbuf[0] = reg;
	rval = bcm2835_i2c_write(wbuf, 1);
	check(rval);
	rval = bcm2835_i2c_read(ret, len);
	check(rval);
}

void i2cWrite(char reg, char data[], uint32_t len) {
	uint8_t rval;

	//memset(wbuf, 0, sizeof(wbuf));

	wbuf[0] = reg;
	rval = bcm2835_i2c_write(wbuf, 1);
	check(rval);

	rval = bcm2835_i2c_write(data, len);
	check(rval);
}