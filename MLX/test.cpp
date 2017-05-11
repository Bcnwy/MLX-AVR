/*
 * mlx90614_I2C_test.cpp
 *
 * Created: 13/09/2016 12:11:32 PM
 * Author : u1056080
 */
#include "main.h"
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"


void i2c_start() {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // send start condition
	while (!(TWCR & (1 << TWINT)));
}

void i2c_write_byte(char byte) {
	TWDR = byte;
	TWCR = (1 << TWINT) | (1 << TWEN); // start address transmission
	while (!(TWCR & (1 << TWINT)));
}

uint8_t i2c_read_byte() {
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN);  // start data reception, transmit ACK
	while (!(TWCR & (1 << TWINT)));
	return TWDR;
}

void i2c_receive_pec() {
	TWCR = (1 << TWINT) | (1 << TWEN);  // start PEC reception, transmit NACK
	while (!(TWCR & (1 << TWINT)));
}

void i2c_stop() {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // send stop condition
}

//Returns 100 times the temperature read by the sensor giving a 0.01 degree resolution.
double i2c_read_temperature_f() {
	uint8_t low_byte, high_byte;
	
	DDRC = 0;                           // all inputs
	PORTC = (1 << PORTC4) | (1 << PORTC5);  // enable pull-ups on SDA and SCL, respectively
	
	TWSR = 0;       // clear bit-rate prescale bits
	TWBR = 192;     // produces an SCL frequency of 50 kHz with a 20 MHz CPU clock speed.
	
	i2c_start();
	// The expected value of TWSR & 0xF8 is now 0x08 (Start condition transmitted).
	
	i2c_write_byte(0);// 0 is the universal write address for slaves.
	// The expected value of TWSR & 0xF8 is now 0x18 (SLA+W transmitted ACK received).
	
	i2c_write_byte(0x07); // read TObj1 (0x07) from RAM
	// The expected value of TWSR & 0xF8 is now 0x28 (Data transmitted ACK received).
	
	i2c_start();
	// The expected value of TWSR & 0xF8 is now 0x10 (Repeated start has been transmitted).
	
	i2c_write_byte(1); // 1 is the universal read address for slaves.
	// The expected value of TWSR & 0xF8 is now 0x40 (SLA+R transmitted ACK received).
	
	low_byte = i2c_read_byte();
	// The expected value of TWSR & 0xF8 is now 0x50 (Data received ACK received).
	
	high_byte = i2c_read_byte();
	// The expected value of TWSR & 0xF8 is now 0x50 (Data received ACK received).
	
	i2c_receive_pec();  // read packet error code (PEC)
	// The expected value of TWSR & 0xF8 is now 0x58 (Data received NOT ACK received).
	
	i2c_stop();
	
	// Tk is temperature in Kelvin, Tf is temperature in degrees Fahrenheit, To is the raw
	// value of the object temperature as returned by the sensor
	// 100 Tk = To × 2 (from the datasheet section 8.7.2--To has the units 0.02K)
	// Tf = Tk × 9/5 - 459.67 (conversion from Kelvin to Farenheit)
	
	// 100 × Tf = 100 × Tk ×  9/5 - 45967
	// 100 × Tf = To  × 2  × 9/5  - 45967
	// 100 × Tf = To  × 18/5      - 45967
	
	
	uint16_t temp = (high_byte<<8) | low_byte;
	double t = temp * 0.02;
	t -= 273.15;
// 	Serial.send(o,DEC);
// 	Serial.sendln();
// 	
	return (t);  // return temperature in units of 0.01°F
}