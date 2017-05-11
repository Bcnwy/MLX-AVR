/*
 * HC_SR04.h
 *
 * Created: 26/01/2017 4:32:25 PM
 *  Author: u1056080
 */ 


#ifndef HC_SR04_H_
#define HC_SR04_H_
#include <stdint.h>


class HC_SR04
{
	public:
	void init();
	uint8_t read();
	volatile float Distance;
	volatile float raw_Distance;
	protected:
	private:
};

#endif /* HC_SR04_H_ */