#include <cstdio>
#include <cassert>

#include "i2c.h"
#include "mpu6050.h"


char ACCEL_XOUT_H = 0x3B;
char ACCEL_XOUT_L = 0x3C;
char ACCEL_YOUT_H = 0x3D;
char ACCEL_YOUT_L = 0x3E;
char ACCEL_ZOUT_H = 0x3F;
char ACCEL_ZOUT_L = 0x40;

char TEMP_OUT_H = 0x41;
char TEMP_OUT_L = 0x42;

char GYRO_XOUT_H = 0x43;
char GYRO_XOUT_L = 0x44;
char GYRO_YOUT_H = 0x45;
char GYRO_YOUT_L = 0x46;
char GYRO_ZOUT_H = 0x47;
char GYRO_ZOUT_L = 0x48;

char PWR_MGMT_1 = 0x6B;
char GYRO_CONFIG = 0x1B;
char ACCEL_CONFIG = 0x1C;
char SMPRT_DIV = 0x19;
char CONFIG = 0x1A;

char FF_THR = 0x1D;
char FF_DUR = 0x1E;

char MOT_THR = 0x1F;
char MOT_DUR = 0x20;

char ZRMOT_THR = 0x21;
char ZRMOT_DUR = 0x22;

char FIFO_EN = 0x23;

unsigned int GYR_LSB = 131;
unsigned int ACC_LSB = 16384;

char mpu_self_adress = 0x69;


void mpu6050_init(uint8_t adress) {
	mpu_self_adress = adress;
	i2cSetAddress(mpu_self_adress);

	//Disable sleep mode
	i2cWrite(PWR_MGMT_1,0x00);
	
	//Sets sample rate to 8000/1+7 = 1000Hz
	i2cWrite(SMPRT_DIV,0x07);
	//Disable FSync, 256Hz DLPF
	i2cWrite(CONFIG, 0x00);

	//Disable gyro self tests, scale of 500 degrees/s
	//0x00 ± 	250 °/s		131 LSB/°/s
	//0x08	± 500 °/s		65.5 LSB/°/s
	//0x10	± 1000 °/s		32.8 LSB/°/s
	//0x18	± 2000 °/s		16.4 LSB/°/s
	i2cWrite(GYRO_CONFIG, 0x00);

	//Disable accel self tests, scale of +-2g, no DHPF
	//0x00	±2g		16384 LSB/mg
	//0x08	±4g		8192 LSB/mg
	//0x10	±8g		4096 LSB/mg
	//0x18	±16g	2048 LSB/mg
	i2cWrite(ACCEL_CONFIG, 0x00);
	//Freefall threshold of |0mg|
	i2cWrite(FF_THR, 0x00);
	//Freefall duration limit of 0
	i2cWrite(FF_DUR, 0x00);
	//Motion threshold of 0mg
	i2cWrite(MOT_THR, 0x00);
	//Motion duration of 0s
	i2cWrite(MOT_DUR, 0x00);
	//Zero motion threshold
	i2cWrite(ZRMOT_THR, 0x00);
	//Zero motion duration threshold
	i2cWrite(ZRMOT_DUR, 0x00);
	//Disable sensor output to FIFO buffer
	i2cWrite(FIFO_EN, 0x00);

	switch (i2cRead(GYRO_CONFIG)) {
		case 0x00:
			GYR_LSB = 131;
		break;
		case 0x08:
			GYR_LSB = 65.5;
		break;
		case 0x10:
			GYR_LSB = 32.8;
		break;
		case 0x18:
			GYR_LSB = 16.4;
		break;
		default:
			printf("ERROR: GYR_LSB is wrong\n");
			assert(0);
		break;
	}

	switch (i2cRead(ACCEL_CONFIG)) {
		case 0x00:
			ACC_LSB = 16384;
		break;
		case 0x08:
			ACC_LSB = 8192;
		break;
		case 0x10:
			ACC_LSB = 4096;
		break;
		case 0x18:
			ACC_LSB = 2048;
		break;
		default:
			printf("ERROR: ACC_LSB is wrong\n");
			assert(0);
		break;
	}

	//printf("GYR_LSB is: %u\n",GYR_LSB);
	//printf("ACC_LSB is: %u\n",ACC_LSB);
}

void mpu6050_selfTest() {
	i2cSetAddress(mpu_self_adress);
	i2cWrite(GYRO_CONFIG, 0xE0);
	i2cWrite(ACCEL_CONFIG, 0xE0);
}


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

void mpu6050_read(mpu6050Data &data) {
	i2cSetAddress(mpu_self_adress);
	char rdata[14];
	i2cRead(ACCEL_XOUT_H, rdata, 14);
	data.acc_x = (float)((int16_t)(rdata[0] << 8) | rdata[1]) / ACC_LSB;
	data.acc_y = (float)((int16_t)(rdata[2] << 8) | rdata[3]) / ACC_LSB;
	data.acc_z = (float)((int16_t)(rdata[4] << 8) | rdata[5]) / ACC_LSB;

	data.temp = 35 + (float)((int16_t)(rdata[6] << 8) | rdata[7]) / 340.0;

	data.gyr_x = (float)((int16_t)(rdata[8] << 8) | rdata[9]) / GYR_LSB;
	data.gyr_y = (float)((int16_t)(rdata[10] << 8) | rdata[11]) / GYR_LSB;
	data.gyr_z = (float)((int16_t)(rdata[12] << 8) | rdata[13]) / GYR_LSB;
	//printf("%16i\n", (int16_t)(rdata[6] << 8) | rdata[7]);
	//printf("temp %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", rdata[0], rdata[1], rdata[2], rdata[3], rdata[4], rdata[5], rdata[6], rdata[7], rdata[8], rdata[9], rdata[10], rdata[11], rdata[12], rdata[13]);

}
