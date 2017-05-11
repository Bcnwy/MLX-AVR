/*************************************************** 
  This is a library for the TMP007 Temp Sensor

  Designed specifically to work with the Adafruit TMP007 Breakout 
  ----> https://www.adafruit.com/products/2023

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
#ifndef  F_CPU
#define F_CPU 16000000UL
#endif

#include <util/delay.h>
#include "Adafruit_TMP007.h"	
#include "I2C.h"
#include "uart.h"
#define TESTDIE 0x0C78
#define TESTVOLT 0xFEED
I2C_bus I2c;
//#define TMP007_DEBUG 


Adafruit_TMP007::Adafruit_TMP007(uint8_t addr) 
{
  addr = addr;
  I2c.init();
}
 void Adafruit_TMP007::set(uint8_t addr)
 {
	 _addr = addr;
 }
bool Adafruit_TMP007::begin(uint16_t samplerate) 
{
	
  write16(TMP007_CONFIG, TMP007_CFG_MODEON | TMP007_CFG_ALERTEN | TMP007_CFG_TRANSC | samplerate );
  write16(TMP007_STATMASK, TMP007_STAT_ALERTEN |TMP007_STAT_CRTEN);
  // enable conversion ready alert

  uint8_t did;
  did = read16(TMP007_DEVID);
    
#ifdef TMP007_DEBUG	
Serial.sendln("Device id read");
 Serial.send("did = 0x"); Serial.send(did,(uint8_t)HEX);Serial.sendln();
#endif
  if (did != 0x78) return false;
  return true;
}

//////////////////////////////////////////////////////

double Adafruit_TMP007::readDieTempC(void) {
   double Tdie = readRawDieTemperature();
   Tdie *= 0.03125; // convert to Celsius
#ifdef TMP007_DEBUG
   Serial.send("Tdie = "); Serial.send(Tdie,DEC); Serial.sendln(" C");
#endif
   return Tdie;
}

double Adafruit_TMP007::readObjTempC(void) {
	//Serial.send("read16");
	//int neg=1;
  uint16_t raw = read16(TMP007_TOBJ);
  // invalid
  if (raw & 0x1) return NAN;
  //if(raw == 0) while (1);
  /*if(raw & 0x1000)
	{
		neg = -1;
		raw &= 0x0FFF;
	}*/

  raw >>=2;

  double Tobj = raw ;
  Tobj *= 0.03125; // convert to Celsius
#ifdef TMP007_DEBUG
   Serial.send("Tobj = "); Serial.send(Tobj,DEC);Serial.sendln(" C");
#endif
   return Tobj;
}

//////////////////////////////////////////////////////
int16_t Adafruit_TMP007::readRawDieTemperature(void) {
  uint16_t raw = read16(TMP007_TDIE);

#ifdef TMP007_DEBUG

#ifdef TESTDIE
  raw = TESTDIE;
#endif

  Serial.send("Raw ambient: 0x"); Serial.send(raw, HEX);
  

  double v = raw/4;
  v *= 0.03125;
  Serial.send(" ("); Serial.send(v, DEC); Serial.sendln(" *C)");
#endif
  raw >>= 2;
  return raw;
}
//////////////////////////////////////////////////////
int16_t Adafruit_TMP007::readRawVoltage(void) {
  uint16_t raw;

  raw = read16(TMP007_VOBJ);

#ifdef TMP007_DEBUG 

#ifdef TESTVOLT
  raw = TESTVOLT;
#endif

  Serial.send("Raw voltage: 0x"); Serial.send (raw,HEX);
  double v = raw;
  v *= 156.25;
  v /= 1000;
  Serial.send(" ("); Serial.send(v,DEC); Serial.sendln(" uV)");
#endif
  return raw; 
}

/*********************************************************************/
uint16_t Adafruit_TMP007::Read_Reg(uint8_t reg)
 {
	 //Serial.sendln("read reg");
	uint16_t ret;

	I2c.start((_addr<<1) + I2C_WRITE); // start transmission to device
	I2c.write(reg); // sends register address to read from

	I2c.start((_addr<<1) + I2C_READ); // start transmission to device
	// Serial.sendln("addr"); 
	ret = I2c.read_ack(); // receive DATA
	//Serial.sendln("ack") ;
	// ret <<= 8;
	 //ret |=I2c.read_nack(); // receive DATA
	// Serial.sendln("nack");
	I2c.stop(); // end transmission

	return ret;
 }

//////////////////////////////////////////////////////
uint16_t Adafruit_TMP007::read16(uint8_t a)
 {
	uint16_t ret;

	I2c.start((_addr<<1) + I2C_WRITE); // start transmission to device 
	I2c.write(a); // sends register address to read from
  	   
	I2c.start((_addr<<1) + I2C_READ); // start transmission to device 
		 //   Serial.sendln("rep start")  ;
	ret = I2c.read_ack(); // receive DATA
   if (ret == 0)
   {
	   //Serial.sendln("Error read16");
   }
	 ret <<= 8;	   
		 
	  ret |=I2c.read_nack();  

	 // receive DATA
	 I2c.stop(); // end transmission
	  if (ret == 0)
	  {
		  // Serial.sendln("error read16nack");
	  }
	  
	return ret;
}
//////////////////////////////////////////////////////
void Adafruit_TMP007::write16(uint8_t a, uint16_t d) 
{
	
	I2c.start((_addr<<1)+I2C_WRITE); // start transmission to device 
	//Serial.sendln("Start write")  ;
	 I2c.write(a); // sends register address to read from
	 I2c.write(d>>8);  // write data
	 I2c.write(d);  // write data

	I2c.stop(); // end transmission
}

