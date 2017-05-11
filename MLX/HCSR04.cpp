/*
 * HC_SR04_.cpp
 *
 * Created: 26/01/2017 4:29:40 PM
 * Author: Ben Conway
 *
 * Version: 1
 *
 * Description:
 *  Connect the ultrasonic sensor to the Arduino as per the
 *  hardware connections below. Run the sketch and open a serial
 *  monitor. The distance read from the sensor will be displayed
 *  in centimeters and inches.
 * 
 * Hardware Connections:
 *		Muc | HC-SR04 
 *  -------------------
 *    5V    |   VCC     
 *    5     |   Trig     
 *    6     |   Echo     
 *    GND   |   GND
 *  
 * License:
 *  Public Domain
 */

#include "HCSR04.h"
#include "main.h"
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
//#include <avr/interrupt.h>
// Pins
const int TRIG_PIN = PIND5;
const int ECHO_PIN = PIND6;

// Anything over 400/500 cm (23200/29410 us pulse) is "out of range"
const unsigned long MAX_DIST = 23200;

//Define operations for setting pin ports
#define set_pin(port, pin) (port |= (1 << pin))
#define clr_pin(port, pin) (port &= ~(1 << pin))
#define out_pin(port, pin) (port |= (1 << pin))
#define in_pin(port, pin) (port &= ~(1 << pin))
#define _BV(bit) (1 << (bit))

  
void HC_SR04::init() {
  // The Trigger pin will tell the sensor to range find
  DDRD = 0x20;
  PORTD = 0x00;
  clr_pin(PORTD, TRIG_PIN);  
}
uint8_t HC_SR04::read(){
	float cm;
	float t1;
	float t2;
	float pulse_width;
	
	//set_micro(0);
	clr_pin(PORTD, TRIG_PIN);
	_delay_us(2);
	// Hold the trigger pin high for at least 10 us
	set_pin(PORTD, TRIG_PIN);
	_delay_us(20);
	clr_pin(PORTD, TRIG_PIN);

	// Wait for pulse on echo pin
	while ((PIND & _BV(ECHO_PIN)) == 0 );
	// Measure how long the echo pin was held high (pulse width)
	// Note: the micros() counter will overflow after ~70 min
	TCNT1 = 0;
	t1 = TCNT1 ;
	//cli();
	while ((PIND & _BV(ECHO_PIN)) != 0);
	//sei();
	t2 = TCNT1 ;
	
	pulse_width = (t2 - t1)* 0.5;
	
	if ( pulse_width > MAX_DIST ) {
		Serial.sendln("Out of range");
		pulse_width = MAX_DIST;
		return 1;
	}
	// Calculate distance in centimeters and inches. The constants
	// are found in the data-sheet, and calculated from the assumed speed 
	//of sound in air at sea level (~340 m/s).
	cm = pulse_width/ 58;
	raw_Distance = cm;
	return 0;
}

