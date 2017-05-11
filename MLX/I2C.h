#ifndef I2C_h
#define I2C_h

#include <stdint.h>

#define I2C_READ 1
#define I2C_WRITE 0
//Time out for receive functions in milliseconds.
#define TimeOut 250	/*longer time is more stable as peripherals have more time to complete response.
However shorter time stops the bus from getting hung, quicker operation with shorter time out.
Set to desired operation weather speed or stability. This is only affects operation when an error has occurred.
******************WARNING DO NOT SET LESS THAN 50ms******************/

class I2C_bus
{
	public:	
		void init(void);
		void begin(void);
		uint8_t start(uint8_t address);
		uint8_t start(void);
		uint8_t write(uint8_t data);
		uint8_t read_ack(void);
		uint8_t read_nack(void);
		void stop(void);
		void beginTransmission(uint8_t addr);
		void requestFrom(uint8_t _addr);
		uint8_t endTransmission(bool);
		uint8_t available(void);
		uint8_t read(void);
		uint8_t Status(void);
		void Restart(uint8_t);
		void Lockup(void);
	protected:
	
	private:
};
#endif 