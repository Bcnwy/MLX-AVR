
#include "MLX90614.h"
#include "I2C.h"
//#include "stdint.h"
#include "main.h"
#include <util/delay.h>
#include "uart.h"
I2C_bus I2C;


MLX::MLX()
{
	// Set initial values for all private member variables
	_deviceAddress = 0x00;
	_defaultUnit = TEMP_C;
	_rawObject = 0;
	_rawAmbient = 0;
	_rawObject2 = 0;
	_rawMax = 0;
	_rawMin = 0;
	I2C.init(); // Initialize I2c
}

uint8_t MLX::begin(uint8_t address)
{
	_deviceAddress = address; // Store the address in a private member
	//I2C.init(); // Initialize I2c
	
	if (readAddress() == _deviceAddress)
	{
		return 1;// Return success
	}
	return 0; //return fail
}

void MLX::setUnit(temperature_units unit)
{
	_defaultUnit = unit; // Store the unit into a private member
}

uint8_t MLX::read()
{
	readObject();
	//_delay_us(1);
	//_delay_ms(1);
	readAmbient();
	// read both the object and ambient temperature values
	return 1; // Else return fail
}

uint8_t MLX::readRange(void)
{
	// Read both minimum and maximum values from EEPROM
	if (readMin() && readMax())
	{
		// If the read succeeded, return success
		return 1;
	}
	return 0; // Else return fail
}

float MLX::ambient(void)
{
	// Return the calculated ambient temperature
	return calcTemperature(_rawAmbient);
}

float MLX::object(void)
{
	// Return the calculated object temperature
	return calcTemperature(_rawObject);
}

float MLX::minimum(void)
{
	// Return the calculated minimum temperature
	return calcTemperature(_rawMin);
}

float MLX::maximum(void)
{
	// Return the calculated maximum temperature
	return calcTemperature(_rawMax);
}

uint8_t MLX::readObject()
{
	int16_t rawObj;
	// Read from the TOBJ1 register, store into the rawObj variable
	if (I2cReadWord(MLX90614_REGISTER_TOBJ1, &rawObj))
	{
		// If the read succeeded
		if (rawObj & 0x8000) // If there was a flag error
		{
			return 0; // Return fail
		}
		// Store the object temperature into the class variable
		_rawObject = rawObj;
		return 1;
	}
	return 0;	
}

uint8_t MLX::readObject2()
{
	int16_t rawObj;
	// Read from the TOBJ2 register, store into the rawObj variable
	if (I2cReadWord(MLX90614_REGISTER_TOBJ2, &rawObj))
	{
		// If the read succeeded
		if (rawObj & 0x8000) // If there was a flag error
		{
			return 0; // Return fail
		}
		// Store the object2 temperature into the class variable
		_rawObject2 = rawObj;
		return 1;
	}
	return 0;	
}

uint8_t MLX::readAmbient()
{
	int16_t rawAmb;
	// Read from the TA register, store value in rawAmb
	if (I2cReadWord(MLX90614_REGISTER_TA, &rawAmb))
	{
		// If the read succeeds, store the read value
		_rawAmbient = rawAmb; // return success
		return 1;
	}
	return 0; // else return fail
}

uint8_t MLX::readMax()
{
	int16_t toMax;
	// Read from the TOMax EEPROM address, store value in toMax
	if (I2cReadWord(MLX90614_REGISTER_TOMAX, &toMax))
	{
		_rawMax = toMax;
		return 1;
	}
	return 0;
}

uint8_t MLX::readMin()
{
	int16_t toMin;
	// Read from the TOMin EEPROM address, store value in toMax
	if (I2cReadWord(MLX90614_REGISTER_TOMIN, &toMin))
	{
		_rawMin = toMin;
		return 1;
	}
	return 0;
}

uint8_t MLX::setMax(float maxTemp)
{
	// Convert the unit-ed value to a raw ADC value:
	int16_t rawMax = calcRawTemp(maxTemp);
	// Write that value to the TOMAX EEPROM address:
	return writeEEPROM(MLX90614_REGISTER_TOMAX, rawMax);
}

uint8_t MLX::setMin(float minTemp)
{
	// Convert the unit-ed value to a raw ADC value:
	int16_t rawMin = calcRawTemp(minTemp);
	// Write that value to the TOMIN EEPROM address:	
	return writeEEPROM(MLX90614_REGISTER_TOMIN, rawMin);
}

uint8_t MLX::setEmissivity(float emis)
{
	// Make sure emissivity is between 0.1 and 1.0
	if ((emis > 1.0) || (emis < 0.1))
		return 0; // Return fail if not
	// Calculate the raw 16-bit value:
	uint16_t ke = uint16_t(65535.0 * emis);
	//ke = constrain(ke, 0x2000, 0xFFFF);

	// Write that value to the ke register
	return writeEEPROM(MLX90614_REGISTER_KE, (int16_t)ke);
}

float MLX::readEmissivity(void)
{
	int16_t ke;
	if (I2cReadWord(MLX90614_REGISTER_KE, &ke))
	{
		// If we successfully read from the ke register
		// calculate the emissivity between 0.1 and 1.0:
		return (((float)((uint16_t)ke)) / 65535.0);
	}
	return 0; // Else return fail
}

uint8_t MLX::readAddress(void)
{
	int16_t tempAdd;
	// Read from the 7-bit I2c address EEPROM storage address:
	if (I2cReadWord(MLX90614_REGISTER_ADDRESS, &tempAdd))
	{
		// If read succeeded, return the address:
		return (uint8_t) tempAdd;
	}
	return 0; // Else return fail
}

uint8_t MLX::setAddress(uint8_t newAdd)
{
	int16_t tempAdd;
	// Make sure the address is within the proper range:
	if ((newAdd >= 0x80) || (newAdd == 0x00))
		return 0; // Return fail if out of range
	// Read from the I2c address address first:
	if (I2cReadWord(MLX90614_REGISTER_ADDRESS, &tempAdd))
	{
		tempAdd &= 0xFF00; // Mask out the address (MSB is junk?)
		tempAdd |= newAdd; // Add the new address
		
		// Write the new address back to EEPROM:
		return writeEEPROM(MLX90614_REGISTER_ADDRESS, tempAdd);
	}	
	return 0;
}

uint8_t MLX::readID(void)
{	
	for (int i=0; i<4; i++)
	{
		int16_t temp = 0;
		// Read from all four ID registers, beginning at the first:
		if (!I2cReadWord(MLX90614_REGISTER_ID0 + i, &temp))
			return 0;
		// If the read succeeded, store the ID into the id array:
		id[i] = (uint16_t)temp;
	}
	return 1;
}

uint32_t MLX::getIDH(void)
{
	// Return the upper 32 bits of the ID
	return ((uint32_t)id[3] << 16) | id[2];
}

uint32_t MLX::getIDL(void)
{
	// Return the lower 32 bits of the ID
	return ((uint32_t)id[1] << 16) | id[0];
}

uint8_t MLX::sleep(void)
{
	// Calculate a crc8 value.
	// Bits sent: _deviceAddress (shifted left 1) + 0xFF
	uint8_t crc = crc8(0, (_deviceAddress << 1));
	crc = crc8(crc, MLX90614_REGISTER_SLEEP);
	
	// Manually send the sleep command:
	I2C.beginTransmission(_deviceAddress);
	I2C.write(MLX90614_REGISTER_SLEEP);
	I2C.write(crc);
	I2C.endTransmission(true);
	
	// Set the SCL pin LOW, and SDA pin HIGH (should be pulled up)
	return 0;
}

uint8_t MLX::wake(void)
{
	// Wake operation from data sheet. (Doesn't seem to be working.)
	return 0;
}

float MLX::calcRawTemp(float calcTemp)
{
	float rawTemp; // Value to eventually be returned
	
	if (_defaultUnit == TEMP_RAW)
	{
		// If unit is set to raw, just return that:
		rawTemp = (int16_t) calcTemp;
	}
	else
	{
		float tempFloat =0;
		// First convert each temperature to Kelvin:
		if (_defaultUnit == TEMP_F)
		{
			// Convert from Fahrenheit to Kelvin
			tempFloat = (calcTemp - 32.0) * 5.0 / 9.0 + 273.15;
		}
		else if (_defaultUnit == TEMP_C)
		{
			tempFloat = calcTemp + 273.15;
		}
		else if (_defaultUnit == TEMP_K)
		{
			tempFloat = calcTemp;

		}
		// Then multiply by 0.02 degK / bit
		tempFloat *= 50;
		rawTemp = tempFloat;
	}
	return rawTemp;
}

float MLX::calcTemperature(int16_t rawTemp)
{
	float retTemp;
	
	if (_defaultUnit == TEMP_RAW)
	{
		retTemp = (float) rawTemp;
	}
	else
	{
		retTemp = float(rawTemp) * 0.02;
		if (_defaultUnit != TEMP_K)
		{
			retTemp -= 273.15;
			if (_defaultUnit == TEMP_F)
			{
				retTemp = retTemp * 9.0 / 5.0 + 32;
			}
		}
	}
	
	return retTemp;
}

uint8_t MLX::I2cReadWord(uint8_t reg, int16_t * dest)
{
	//int timeout = I2C_READ_TIMEOUT;

 	if(I2C.start((_deviceAddress<<1) + I2C_WRITE)) // start transmission to device
 	{
 		//Serial.sendln("start fail");
		 return 0;
 	}
 	I2C.write(reg);
 	I2C.start((_deviceAddress<<1) + I2C_READ); // start transmission to device
 	//while ((I2C.available() < 3) && (timeout-- > 0))
	_delay_ms(1);
// 	//if (timeout <= 0)
// 	//	return 0;

 	uint8_t lsb = I2C.read_ack();
 	uint8_t msb = I2C.read_ack();
 	uint8_t pec = I2C.read_nack();
	I2C.stop();
	
	uint8_t crc = crc8(0, (_deviceAddress << 1));
	crc = crc8(crc, reg);
	crc = crc8(crc, (_deviceAddress << 1) + 1);
	crc = crc8(crc, lsb);
	crc = crc8(crc, msb);
	
	if (crc == pec)
	{
		*dest = (msb << 8) | lsb;
		return 1;
	}
	return 0;
}

uint8_t MLX::writeEEPROM(uint8_t reg, int16_t data)
{	
	// Clear out EEPROM first:
	if (I2cWriteWord(reg, 0) != 0)
		return 0; // If the write failed, return 0
	_delay_ms(5); // Delay tErase
	
	uint8_t I2cRet = I2cWriteWord(reg, data);
	_delay_ms(5); // Delay tWrite
	
	if (I2cRet == 0)
		return 1;
	else
		return 0;	
}

uint8_t MLX::I2cWriteWord(uint8_t reg, int16_t data)
{
	uint8_t crc;
	uint8_t lsb = data & 0x00FF;
	uint8_t msb = (data >> 8);
	
	crc = crc8(0, (_deviceAddress << 1));
	crc = crc8(crc, reg);
	crc = crc8(crc, lsb);
	crc = crc8(crc, msb);
	
	I2C.beginTransmission(_deviceAddress);
	I2C.write(reg);
	I2C.write(lsb);
	I2C.write(msb);
	I2C.write(crc);
	return I2C.endTransmission(true);
}

uint8_t MLX::crc8 (uint8_t inCrc, uint8_t inData)
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