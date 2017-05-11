/*
 * main.h
 *
 * Created: 21/07/2016 3:54:09 PM
 *  Author: u1056080
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 16000000UL
#define Off 0
#define Manual 1
#define Auto 2
#include <stdint.h>

//#define debug

void setup();
unsigned long int millis(void);
uint8_t read_byte(uint8_t* data);
void WDT_setup(void);
void Temp_setup(void);

void display_temp(double*,double*,int*);

#endif /* MAIN_H_ */