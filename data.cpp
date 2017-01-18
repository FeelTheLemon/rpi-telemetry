#include <thread>
#include <unistd.h>
#include <cstdio>

#include <cmath>
#include <ctgmath>
#define PI 3.14159265

#include "data.h"
#include "common/i2c/i2c.h"



//thread t1(runLoader);
sensorsData data;


#define K 0.2
void calcAngle() {

	float pitch = atan2(-data.mpu.acc_z, sqrt(data.mpu.acc_y * data.mpu.acc_y + data.mpu.acc_x * data.mpu.acc_x)) * 180 / PI;
	//data.ang.pitch = atan2(-data.mpu.acc_z, -data.mpu.acc_y) * 180 / Math.PI
	float roll = atan2(-data.mpu.acc_y, -data.mpu.acc_x) * 180 / PI - 90;
	float yaw = atan2(data.hmc.x, -data.hmc.z) * 180 / PI;

	if (pitch > 90) { pitch = 90 - (pitch - 90); }
	if (pitch < -90) { pitch = (90 + (pitch + 90)) * -1; }
	if (roll < -180) { roll = (180 + (roll + 180)); }
	if (yaw < 0) { yaw = (270 + (90 + yaw)); }

	data.ang.pitch = data.ang.pitch * (1 - K) + pitch * K;
	data.ang.yaw = data.ang.yaw * (1 - K) + yaw * K;
	data.ang.roll = data.ang.roll * (1 - K) + roll * K;
}

void i2cWorker() {
	i2cOpen();
	mpu6050_init(0x68);
	hmc5983_init(0x1e);

	while (1) {
		mpu6050_read(data.mpu);
		hmc5983_read(data.hmc);
		calcAngle();


		


		//printf("tread %f\n", data.ang.pitch);

		usleep(30 * 1000);
	}

	i2cClose();
}

void gpsWorker() {
	gpsInit();
	gpsUpdateData(data.gps);
}



void InitData() {
	std::thread i2cTread(i2cWorker); i2cTread.detach();
	std::thread gpsTread(gpsWorker); gpsTread.detach();

	printf("Data initialized!\n" );
}

