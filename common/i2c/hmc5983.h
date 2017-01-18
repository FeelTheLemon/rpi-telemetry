#pragma once
#include <stdint.h>




struct hmc5983Data {
	float x;
	float y;
	float z;
	float temp;
};

void hmc5983_init(uint8_t adress);
void hmc5983_read(hmc5983Data &data);
