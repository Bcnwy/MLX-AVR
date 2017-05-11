/*
Serial converting functions to output data in bytes over serial (TXRX) connection
*/
//Include libraries

#include <avr/io.h>
#include "main.h"
#include "uart.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>

USART Serial; //Initialize new instance of the serial class
//Setup Baud rate in header file
//set to 8 bits, no stop bit, no parity

/*********************************************************************/
void USART::init(void){
	UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);

	UCSR0B |= (1 << RXCIE0); // Enable the USART Receive Complete interrupt (USART_RXC)
	sei();
}
/*********************************************************************/
bool USART::available(void)
{
	 if (UCSR0A & (1<<RXC0)) {
		 return true;
	 }
	 return false;
}
/*********************************************************************/
bool USART::flush(void)
{
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
	return false;
}
/*********************************************************************/
unsigned char USART::read(void){

 while(!(UCSR0A & (1<<RXC0))); //wait until data is in buffer
 return UDR0;// read buffer
}
/*********************************************************************/
void USART::send(uint8_t data){

	while(!(UCSR0A & (1<<UDRE0))); //wait until buffer is empty
	UDR0 = data; //Load data into buffer
}
/*********************************************************************/
//send one char at a time
void USART::send(const char* StringPtr)
{
	//st = millis();
	while(*StringPtr != 0x00)
	{
		send(*StringPtr++);
	}
	//ft = millis();
}
/*********************************************************************/
void USART::send(int data)
{
	char str[10];
	sprintf(str,"%i",data);
	send(str);
}
void USART::send(int data, int base)
{
	char str[10];
	if (base == DEC){
		sprintf(str,"%d",data);
		}else{
		sprintf(str,"%x",data);
	}
	send(str);
}
/*********************************************************************/
//convert int to ASCII number, display in decimal or hex
void USART::send(uint8_t data, int base)
{
	char str[3];
	if (base == DEC){
		sprintf(str,"%d",data);
	}else{
		sprintf(str,"%x",data);	
	}
	send(str);
	/*
	uint8_t Output_unit, Output_ten, Output_hundred, Output;
	Output = data / base;
	if(base == DEC)
	{
		if (Output>9)
		{
			Output_hundred = Output/base;
			send(Output_hundred+48);
			Output_ten = Output % base;
			send(Output_ten+48);
			Output_unit = data % base;
			send(Output_unit + 48);
		}
		else if (Output <=9 && Output > 0)
		{
			Output_ten= Output;
			send(Output_ten + 48);
			Output_unit = data % base;
			send(Output_unit + 48);
		}
		else if (data == 0)
		{
			send(Output + 48);
		}
		else
		{
			Output_unit = data % base;
			send(Output_unit + 48);
		}
	}
	else
	{
		if (Output>9)
		{
			send(Output+55);
		}
		else
		{

			send(Output+48);
		}

		Output_unit = data % base;

		if (Output_unit>9)
		{
			send(Output_unit+55);
		}
		else
		{
			send(Output_unit+48);
		}
	}*/
}
/*********************************************************************/
void USART::send(double data,int base)
{
	double intr;
	int frac = modf (data, &intr);
	send((int)intr,base);

	send(".");
	frac = (frac+0.005) * 100;
	send((uint8_t)frac,base);
}
 /*********************************************************************/
// void USART::send(uint16_t data, int base)
// {
// 	uint16_t output;
// 	uint8_t smallout;
// 	if(base == HEX)
// 	{
// 		if(data>255)
// 		{
// 			output = (data>>8);
// 			smallout = (uint8_t)output;
// 			send(smallout,base);
// 		}
// 		output = data;
// 		smallout = (uint8_t)output;
// 		send(smallout, base);
// 	}
// 	else
// 	{
// 		if(data>255)
// 		{
// 			output = (data/base);
// 			if (output>255)
// 			{
// 				uint16_t out = (output/base);
// 				if (out > 255 )
// 				{
// 					smallout = (uint8_t)(out/base);
// 					send(smallout,base);
// 
// 					smallout = (uint8_t)(out % base);
// 					//Serial.send("/") ;
// 					send(smallout, base);
// 
// 					smallout = (uint8_t)output % base;
// 					send(smallout, base);
// 				}
// 				else
// 				{
// 					smallout = (uint8_t)out;
// 					send(smallout,base);
// 
// 					out = output % base;
// 					smallout = (uint8_t)out;
// 					//Serial.send("/") ;
// 					send(smallout, base);
// 				}
// 			}
// 			else
// 			{
// 				smallout = (uint8_t)output;
// 				//Serial.send("/") ;
// 				send(smallout, base);
// 			}
// 
// 				output = data % base;
// 				smallout = (uint8_t)output;
// 				//Serial.send("/") ;
// 				send(smallout, base);
// 		}
// 		else
// 		{
// 			output = data;
// 			smallout = (uint8_t)output;
// 			send(smallout, base);
// 		}
// 	}
// }
/*********************************************************************/
// void USART::send(uint32_t str)
// {
// 	send((uint8_t)(str>>24));
// 	send((uint8_t)(str>>16));
// 	send((uint8_t)(str>>8));
// 	send((uint8_t)str);
// }
/*********************************************************************/
//void USART::send_int(int d)
// {
// 	/*
// 	uint8_t ten = d/100;
// 	uint8_t unit = (d % 100)/10;
// 	uint8_t decimal = d % 10;
// 	//string s;
// 
// 	const char* str;
// 
// 	 (char)ten,(char)unit,".",(char)decimal)
// 	//send(str);*/
// }
long int USART::Get_st(){
	return st;
}
long int USART::Get_ft(){
	return ft;
}