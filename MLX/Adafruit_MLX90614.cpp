/*************************************************** 
  This is a library for the MLX90614 Temp Sensor

  Designed specifically to work with the MLX90614 sensors in the
  adafruit shop
  ----> https://www.adafruit.com/products/1748
  ----> https://www.adafruit.com/products/1749

  These sensors use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_MLX90614.h"
#include "I2C.h"
#include "uart.h"
//#include <util/twi.h>
#include "main.h"
#include <util/delay.h>
I2C_bus Wire;

Adafruit_MLX90614::Adafruit_MLX90614(uint8_t i2caddr) {
  _addr = i2caddr;
}

/*********************************************************************/ 
bool Adafruit_MLX90614::begin(void) {
  Wire.init();
  return true;
}

/*********************************************************************/ 
double Adafruit_MLX90614::readObjectTempF(void) {
  return (readTemp(MLX90614_TOBJ1) * 9 / 5) + 32;
}


double Adafruit_MLX90614::readAmbientTempF(void) {
  return (readTemp(MLX90614_TA) * 9 / 5) + 32;
}

double Adafruit_MLX90614::readObjectTempC(void) {
  return readTemp(MLX90614_TOBJ1);
  
}

double Adafruit_MLX90614::readAmbientTempC(void) {
  return readTemp(MLX90614_TA);
}

double Adafruit_MLX90614::readTemp(uint8_t reg) {
  double temp;
   temp =  read16(reg) * .02;
	temp  -= 273.15;
//   if ((temp = read16(reg)) != 0)
//   { 
// 	  temp *= .02;
// 	  temp  -= 273.15;
//   }
//   else temp = 0;
 
  return temp;
}

/*********************************************************************/ 	
uint16_t Adafruit_MLX90614::read16(uint8_t reg) {
	uint16_t ret;
	//Wire.init();
// 	Wire.start((_addr<<1)+I2C_WRITE);
// 	Wire.write(reg);
// 	Wire.start((_addr<<1)+I2C_READ);
	
	 if (Wire.start((_addr<<1)+I2C_WRITE)) {
		 
		//Wire.Lockup((_addr<<1) + I2C_WRITE);
		 Serial.sendln("error.... start+W");
		// Serial.send(Wire.Status());
		// Serial.sendln();
		 return 0;
		 }
	 
	 if ( Wire.write(reg))
	 {
		  //Serial.send(Wire.Status());
		  Serial.sendln("error.... Write");
		 return 0;
	 }
	//Serial.Wire.Status()
//Wire.stop();
	 if (Wire.start((_addr<<1)+I2C_READ)){
		  //Wire.stop();
		  // Wire.Lockup();
		    Serial.send(Wire.Status());
		  Serial.sendln("error....start+R"); 
		  return 0;
	 }
	
	//_delay_ms(50);
	uint8_t lsb,msb,pec;
	 
	if (!(lsb = Wire.read_ack()) | !(msb = Wire.read_ack()))
	{
		Wire.Lockup();
		//Serial.sendln("read lock......");
		return 0;
	}
	 pec = Wire.read_nack();
	Wire.stop();
	
 	uint8_t crc = crc8(0, (_addr << 1));
	crc = crc8(crc, reg);
	crc = crc8(crc, (_addr << 1) + 1);
	crc = crc8(crc, lsb);
	crc = crc8(crc, msb);
	//crc = pec;
  
	if (crc == pec)
	{
		 ret = (msb << 8) | lsb;
		return ret;
		//Serial.send(ret, HEX);Serial.sendln();
		
	}
	else
	{
		return 0;
	}
 	
}

uint8_t  Adafruit_MLX90614::crc8 (uint8_t inCrc, uint8_t inData)
{
	uint8_t i;
	uint8_t data;
	data = inCrc ^ inData;
	for ( i = 0; i < 8; i++ )
	{
		if (( data & 0x80 ) != 0 )
		{
			data <<= 1;
			data ^= 0x07;
		}
		else
		{
			data <<= 1;
		}
	}
	return data;
}
// uint8_t Adafruit_MLX90614::I2cWriteWord(uint8_t reg, int16_t data)
// {
// 	uint8_t crc;
// 	uint8_t lsb = data & 0x00FF;
// 	uint8_t msb = (data >> 8);
// 	
// 	crc = crc8(0, (_addr << 1));
// 	crc = crc8(crc, reg);
// 	crc = crc8(crc, lsb);
// 	crc = crc8(crc, msb);
// 	
// 	Wire.start((_addr<<1)+I2C_WRITE);
// 	Wire.write(reg);
// 	Wire.write(lsb);
// 	Wire.write(msb);
// 	Wire.write(crc);
// 	
// 	Wire.stop();
// 	
// 	return 0;
// }

uint8_t Adafruit_MLX90614::readID(void)
{
	uint32_t id[4];
	for (int i=0; i<4; i++)
	{
		Wire.start((_addr<<1) + I2C_WRITE);
		// Read from all four ID registers, beginning at the first:
		Wire.write(MLX90614_ID1 + i);
		Wire.start((_addr<<1) + I2C_READ);
		if (i >= 3)
		{
			id[i] = Wire.read_nack();	
		}
		else id[i] = Wire.read_ack();	
		Wire.stop();
	}
	uint32_t ID =(id[3]<< 24) | (id[2]<<16) | (id[1] << 8) | id[0];
	Serial.send(ID);
	Serial.sendln();
	return 0;
}

uint8_t Adafruit_MLX90614::Sleep(void)
{
	Wire.start((_addr<<1)+I2C_WRITE);
	Wire.write(0xFF);
	Wire.write(0xF3);
	Wire.stop();
	return 0;
}

uint8_t Adafruit_MLX90614::Restart(void)
{
	Sleep();
	Wire.init();
	return 0;
}