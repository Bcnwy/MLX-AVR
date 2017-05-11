/*
 * MLX.cpp
 *
 * Created: 27/06/2016 3:06:17 PM
 * Author : u1056080
 * Version v1.6
 */ 
//Define operations for setting pin ports
#define set_pin(port, pin) (port |= (1 << pin))
#define clr_pin(port, pin) (port &= ~(1 << pin))
#define out_pin(port, pin) (port |= (1 << pin))
#define in_pin(port, pin) (port &= ~(1 << pin))
#define _BV(bit) (1 << (bit))
#define F_CPU 16000000UL
#include "main.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "math.h"
#include "MLX90614.h"
#include "I2C.h"
#include "uart.h"
#include "Adafruit_TMP007.h"
#include "HCSR04.h"
#include <stdio.h>
#include <stdlib.h>
#include <avr/wdt.h>
#include <string.h> 

struct Hardware
{	
	bool MLX = false;
	bool TMP = false;
};

Hardware Sensors;
//USART Serial;
//I2C_bus I;

HC_SR04 sonar;

uint8_t Temp_mode;
byte packet[16];
byte inbyte;
double prev_distance = 0;
volatile unsigned long int count = 0; 
//volatile int prev_count = 0;
int ndevices_mlx, ndevices_tmp;
Adafruit_TMP007 TMP;
MLX IR[4];

ISR(TIMER0_COMPA_vect)
{
	count++;
}
ISR(TIMER1_COMPA_vect)
{
}
// ISR(WDT_vect)
// {
// 	PORTB ^= (1<<PORTB5);
// 	//WDTCSR |= (1<<WDIE);
// 	
// }
/*********************************************************************/ 
void setup()
{	
	_delay_ms(10);
	DDRB |= (1<<PINB5);//pin 5 output
	PORTB |= (1<<PINB5);//output high
	
	//wdt_enable(WDTO_1S);	
	TCNT0 = 0;
	TCNT1 = 0;
	EIMSK |= (_BV(INT1) | _BV(INT0));
	EICRA |= (_BV(ISC01) | _BV(ISC00) | _BV(ISC11) | _BV(ISC10));
	TCCR0A |= (1<<WGM01);
	TCCR1A |= (1<<WGM12);
	OCR0A = 249;
	OCR1AH = 0xFF;
	OCR1AL = 0xFF;
	TIMSK0 |= (1 << OCIE0A);
	TIMSK1 |= (1 << OCIE1A);
	TCCR0B |= (1<<CS01) | (1<<CS00);
	TCCR1B |= (1<<CS11);
	sei();
	Serial.init();
	#ifdef debug
	Serial.sendln("start...........");
	#endif
	bool senorFail=  false;
	//IR[0] = MLX();
	//while (IR[0].begin(0x5A) == 0);
	//while (IR[1].begin(0x5B)== 0);
	//IR[2].begin(0x5c);
	//IR[3].begin(0x5d);
	_delay_ms(10);
	if (IR[0].begin(0x5A) == 0){Serial.sendln("not 5a");senorFail = true;}
		_delay_ms(10);
 	if (IR[1].begin(0x5B) == 0){Serial.sendln("not 5b");senorFail = true;}
 		_delay_ms(10);
 	if (IR[2].begin(0x5C) == 0){Serial.sendln("not 5c");senorFail = true;}
 		_delay_ms(10);
 	if (IR[3].begin(0x5D) == 0){Serial.sendln("not 5d");senorFail = true;}
 		_delay_ms(10);
	if(senorFail)while(1);
//while(1);
	//TMP.set(0x40);
	//TMP.begin(TMP007_CFG_1SAMPLE);
	ndevices_mlx = 4;
	ndevices_tmp = 0;
	Sensors.MLX = true;
	Sensors.TMP = false;
	sonar.init();
	//Serial.sendln("sonar...........");
	clr_pin(PORTB,PINB5);//output low
}
/*********************************************************************/ 
void WDT_setup(void)
{
	cli();
	
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1<<WDRF);
	/* Write logical one to WDCE and WDE */
	/* Keep old pre-scaler setting to prevent unintentional time-out */

	//WDTCSR |= (1<<WDCE) | (1<<WDE);
	//WDTCSR = 0;
	//WDTCSR |=(1<<WDE) | (1<<WDP2) | (1<<WDP1);
	wdt_enable(WDTO_1S);
	/* Turn off WDT */
	//WDTCSR = 0x00;
	sei();
}
/*********************************************************************/ 
unsigned long int millis(void)
{
	cli();
	unsigned long int d = count;
	sei();
	return d;
}
/*********************************************************************/
int main(void)
{

	//wdt_reset();
	//wdt_disable();
	//cli();
	setup();
	#ifdef debug
	Serial.sendln("IR started....");
	#endif
	Temp_mode =Auto;
	unsigned long start_time;
	while(1){
		//TODO: send no. of devices to pc//
		while (Temp_mode == Auto){
			start_time = millis();
			double objt[ndevices_mlx],dietemp[ndevices_mlx];
			int dist[ndevices_mlx];
			sonar.read();		
			if (Sensors.MLX){
				for(int s = 0; s < ndevices_mlx; s++){
					IR[s].read();
					objt[s] = IR[s].object();
					dietemp[s] = IR[s].ambient();
					dist[s] = (int)(sonar.raw_Distance+0.5d);
					display_temp(objt,dietemp,dist);			
				}	
			}
			/*if (Sensors.TMP){
				objt = TMP.readObjTempC();
				dietemp = TMP.readDieTempC();
				display_temp(objt,dietemp,(int)(sonar.raw_Distance+0.5d), ndevices_mlx+1);
			}*/
			while((millis()-start_time) <= 250);
		}
	}
}
/*********************************************************************/ 
void display_temp(double* obj, double *amb, int *sonar_dist)
{
	 char str_arr[64];
	for(int i=0;i<ndevices_mlx;i++)
	{
		char str[16];
		sprintf(str, "T%d,%.1f,%.1f,%i\n", i,obj[i],amb[i], sonar_dist[i]);
		strcat(str_arr,str);
	}
	Serial.send(str_arr);
	Serial.sendln();
}
/*********************************************************************/
uint8_t read_byte (byte* data)
{
	static int i = 1;
	bool serial_end = false;
	//Serial.send("read");
	while(!serial_end){
		if (Serial.available() > 0){
			inbyte = Serial.read();
			if (inbyte != 0x0D && inbyte != 0x0A ){
				data[i] = inbyte;
				//Serial.sendln(inbyte);
				i++;
			}
			else{
				inbyte = data[i-1];
				i=1;
				serial_end = true;
			}
		}
	}
	return inbyte;
}
/*********************************************************************/ 

