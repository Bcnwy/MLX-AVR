/*
UART header file 
For sending serial commands using the hardware UART in the ATmega88A
*/
#ifndef uart_h
#define uart_h
#ifndef  F_CPU
#define F_CPU 16000000UL
#endif

#define BAUDRATE 9600//Default Baud rate
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)
#define HEX 16
#define DEC 10
#include <stdio.h>
//Declaration of functions
// USART class for sending bytes and chars in HEX or Decimal over serial
class USART
{
	public:
		void init(void); 
		bool available(void);
		unsigned char read(void);
		bool flush(void);
		void send(uint8_t data);
		void send(char data) {send((uint8_t)data); };
		void send(uint8_t* data) {send((uint8_t)*data); };
			
		void send(int data);
			void send(int data,int base);
		//void send(int data, int base){};	
			
		//void send(uint16_t data, int base);
		//void send(uint32_t data, int base);
		//void send(uint64_t data, int base){};		
		//void send(uint16_t data){send(data,HEX);};		
		void send(const char* StringPtr);
		//void send_s(char * Str);
		void send(uint8_t data, int base);
		//void send_int(int d);
		void send(double data, int base);
		//void send(uint32_t str);
			
		void sendln(void) {send("\r\n");};
		void sendln(const char* c) {send(c);send("\r\n");};
		void sendln(char c)   {send(c);send("\r\n");};
		//void sendln(unsigned int data, uint8_t base) {send(data,base);send("\n\r");};	
		//no protected or private functions left undeclared
		
		long int Get_st();
		long int Get_ft();
	protected:
	private:
		 long int st;
		 long int ft;
};
extern USART Serial; //Re-name class for external use
// USART Serial;
#endif