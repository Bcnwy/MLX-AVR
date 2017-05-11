#include <avr/io.h>
#include "main.h"
#include "I2C.h"
#include "uart.h"
#include <util/twi.h>
#include <util/delay.h>

#define F_SCL 50000UL // SCL frequency
#define Prescaler 1
#define TWBR_val ((((F_CPU / F_SCL) / Prescaler) - 16UL ) / 2)

unsigned long int time_start;
bool Time_out = false;
/*********************************************************************/ 
void I2C_bus::init(void){
	DDRC = 0;
	PORTC = (1 << PORTC4) | (1 << PORTC5);//enable pull up
	TWSR = 0x00;
	TWBR = TWBR_val;// 192;
}
/*********************************************************************/ 
void I2C_bus::begin(void){
	init();
}
/*********************************************************************/ 
uint8_t I2C_bus::start(uint8_t address){
	uint8_t twst;
	// reset TWI control register
	//TWCR = 0; WARNING Causes Smbus device (MLX90614) to return 0xFFFF all the time. Comment out if this problem is apparent. 
	// transmit START condition 
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	// wait for end of transmission
	while( !(TWCR & (1<<TWINT)) );
	twst = TW_STATUS & 0xF8;
	// check if the start condition was successfully transmitted
 	if((twst != TW_START) && (twst != TW_REP_START)){ return 1; }
	// load slave address into data register
	if (write(address)) {return 1;}
 	return 0;
}
uint8_t I2C_bus::start(){
	uint8_t twst;
	// reset TWI control register
	//TWCR = 0; WARNING Causes Smbus device (MLX90614) to return 0xFFFF all the time. Comment out if this problem is apparent.
	// transmit START condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	// wait for end of transmission
	while( !(TWCR & (1<<TWINT)) );
	twst = TW_STATUS & 0xF8;
	// check if the start condition was successfully transmitted
	if((twst != TW_START) && (twst != TW_REP_START)){ return 1; }
	// load slave address into data register
	return 0;
}
/*********************************************************************/ 
uint8_t I2C_bus::write(uint8_t data){
	uint8_t twst;
	// load data into data register
	TWDR = data;
	// start transmission of data
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wait for end of transmission
 	while( !(TWCR & (1<<TWINT)) );
	// check if the device has acknowledged the READ / WRITE mode

 	twst = TW_STATUS & 0xF8;			
	if ((twst != TW_MT_SLA_ACK)&&(twst != TW_MR_SLA_ACK)) { return 1;}
	return 0;
}
/*********************************************************************/ 
uint8_t I2C_bus::read_ack(void)
{
	// start TWI module and acknowledge data after reception
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	// wait for end of transmission	
	time_start = millis(); 
	while( (!(TWCR & (1<<TWINT))))
	{
		if (millis() - time_start >= TimeOut)
		{
			//Serial.sendln("I2c Time out ack");
			time_start = millis();
			//Lockup();
			return TWDR; //should be 0
		}
	}

	// Check for the acknowledgment
 //	 time_start = millis();
// 	while(((TWSR & 0xF8) != 0x50))
 //	{
//		if (millis() - time_start >= TimeOut)
 //		{
//			 Serial.sendln("I2c Time out ack2");
// 			 time_start = millis();
// 			// stop();
 		  // Time_out = true;
// 		   return 0;
// 		}
// 	}
	// return received data from TWDR
	return TWDR;
}
/*********************************************************************/ 
uint8_t I2C_bus::read_nack(void)
{
	// start receiving without acknowledging reception	 
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wait for end of transmission	   
	time_start = millis();	
	while(!(TWCR & (1<<TWINT)))
	{
		if (millis() - time_start >= TimeOut)
		{
			//Serial.sendln("I2c_nack Time out");
			time_start = millis();
			//Lockup();
			return 0;
		}
	}	
 	// Check for the acknowledgment
 //	 time_start = millis();
 //	while(((TWSR & 0xF8) != 0x58))
 //	{
//		if (millis() - time_start >= TimeOut)
 //		{
 //			Serial.sendln("I2c Time out Nack2");
// 			 time_start = millis();
// 		//	 stop();
// 		  // Time_out = true;
// 		   return 0;
// 		}
// 	}
	// return received data from TWDR
	return TWDR;
}

uint8_t I2C_bus::read(void)
{
	return read_ack();
}
/*********************************************************************/ 
void I2C_bus::stop(void){
	// transmit STOP condition
		TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) ;   // Send stop condition.
	//while(TWCR & (1<<TWSTO));
}
/*********************************************************************/ 
void I2C_bus::beginTransmission(uint8_t _addr)
{
	 start((_addr<<1) + I2C_WRITE);
}

/*********************************************************************/ 
uint8_t I2C_bus::endTransmission(bool b)
{
	if (b){
		stop();
		return 1;
	}	
	else{
		return start();
	}
}
/*********************************************************************/ 
 void I2C_bus::requestFrom(uint8_t _addr)
 {
	 start((_addr<<1) + I2C_READ);
 }
/*********************************************************************/ 
uint8_t I2C_bus::available(void)
{
	//TODO:: any data on i2c bus
	return 0;
}
/*********************************************************************/ 
uint8_t I2C_bus::Status(void)
{
	uint8_t status;
	//mask status
	status = TWSR;
	return status;
}
/*********************************************************************/ 
void I2C_bus::Restart(uint8_t data)
{
	Lockup();
	start(data);
}
/*********************************************************************/ 
void I2C_bus::Lockup()
{
	stop();
	init();
}