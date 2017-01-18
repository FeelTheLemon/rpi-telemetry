#pragma once

#include "common/i2c/mpu6050.h"
#include "common/i2c/hmc5983.h"
#include "common/uart/gps.h"



struct powerData {
	float u12v;
	float u5v;
	float i12v;
	float i5v;
};

struct angle {
	float pitch;
	float yaw;
	float roll;
};
struct sensorsData {
	gpsData gps; 
	powerData power;
	mpu6050Data mpu;
	hmc5983Data hmc;
	angle ang;
};

extern sensorsData data;

void InitData();
