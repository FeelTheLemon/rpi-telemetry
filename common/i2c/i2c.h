#pragma once
#include <stdint.h>

void i2cOpen();
void i2cClose();
void i2cSetAddress(char reg);

char i2cRead(char reg);
void i2cRead(char reg, char ret[], uint32_t len);

void i2cWrite(char reg, char data);
void i2cWrite(char reg, char data[], uint32_t len);




/*
class i2c {

public:
	uint8_t read(uint8_t reg);
	//void read(uint8_t reg, char ret[], uint32_t len);
	void write(uint8_t reg, uint8_t data);
	//void write(uint8_t reg, char data[], uint32_t len);

	void setSlaveAdress(uint8_t address);

	i2c();
	~i2c();
};
*/