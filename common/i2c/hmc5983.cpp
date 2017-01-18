#include <cstdio>
#include <cassert>

#include "i2c.h"
#include "hmc5983.h"

char CONFIG_A = 0x00;
char CONFIG_B = 0x01;
char MODE = 0x02;

char XOUT_H = 0x03;
char XOUT_L = 0x04;
char ZOUT_H = 0x05;
char ZOUT_L = 0x06;
char YOUT_H = 0x07;
char YOUT_L = 0x08;

char TEMP_H = 0x31;
char TEMP_L = 0x32;

unsigned int M_LSB = 1090;

char hmc_self_adress = 0x1e;


void hmc5983_init(uint8_t adress) {
	hmc_self_adress = adress;
	i2cSetAddress(hmc_self_adress);

	i2cWrite(CONFIG_A, 0b11111000);

	i2cWrite(CONFIG_B, 0b00100000);

	i2cWrite(MODE, 0b00000000);

	switch (i2cRead(CONFIG_B)) {
		case 0b00000000:
			M_LSB = 1370;
		break;
		case 0b00100000:
			M_LSB = 1090;
		break;
		case 0b01000000:
			M_LSB = 820;
		break;
		case 0b01100000:
			M_LSB = 660;
		break;
		case 0b10000000:
			M_LSB = 440;
		break;
		case 0b10100000:
			M_LSB = 390;
		break;
		case 0b11000000:
			M_LSB = 330;
		break;
		case 0b11100000:
			M_LSB = 230;
		break;
		default:
		break;
	}
}

void hmc5983_selfTest() {
	i2cSetAddress(hmc_self_adress);
}

void hmc5983_read(hmc5983Data &data) {
	i2cSetAddress(hmc_self_adress);

	char rdata[6];
	i2cRead(XOUT_H, rdata, 6);
	data.x = (float)((int16_t)(rdata[0] << 8) | rdata[1]) / M_LSB;
	data.z = (float)((int16_t)(rdata[2] << 8) | rdata[3]) / M_LSB;
	data.y = (float)((int16_t)(rdata[4] << 8) | rdata[5]) / M_LSB;

	char rtdata[2];
	i2cRead(TEMP_H, rtdata, 2);

	//data.temp = 25 + (float)((int16_t)(rdata[0] << 8) | rdata[1]);

	data.temp = (rtdata[0] * 2^8 + rtdata[1]) / (2^4 * 8) + 25; //idk from datasheet
}
