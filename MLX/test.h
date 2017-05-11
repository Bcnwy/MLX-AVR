/*
 * test.h
 *
 * Created: 14/09/2016 2:52:15 PM
 *  Author: u1056080
 */ 


#ifndef TEST_H_
#define TEST_H_

void i2c_start();
void i2c_write_byte(char byte);
uint8_t i2c_read_byte();
void i2c_receive_pec(); 
void i2c_stop();
double i2c_read_temperature_f();

#endif /* TEST_H_ */