#pragma once

void uart_init(const char * device);
int uart_read(char ret[], unsigned int len);
int uart_write(char data[], unsigned int len);
void uart_close();