#pragma once
#include <stdint.h>




struct mpu6050Data {
	float acc_x;
	float acc_y;
	float acc_z;

	float gyr_x;
	float gyr_y;
	float gyr_z;
	
	float temp;
};

void mpu6050_init(uint8_t adress);
void mpu6050_read(mpu6050Data &data);
