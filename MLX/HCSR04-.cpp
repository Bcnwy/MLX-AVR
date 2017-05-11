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
 *  Arduino | HC-SR04 
 *  -------------------
 *    5V    |   VCC     
 *    7     |   Trig     
 *    8     |   Echo     
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
// Pins
const int TRIG_PIN = PIND5;
const int ECHO_PIN = PIND6;

// Anything over 400 cm (23200 us pulse) is "out of range"
const unsigned long MAX_DIST = 92800;

// Variables 


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
double HC_SR04::Get_distance(){
	double cm;
	unsigned long t1;
	unsigned long t2;
	unsigned long pulse_width;
	cli();
	
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
	
	t1 = TCNT1;
	while ((PIND & _BV(ECHO_PIN)) != 0);
	t2 = TCNT1;
	sei();
	pulse_width = (t2 - t1);

	// Calculate distance in centimeters and inches. The constants
	// are found in the data-sheet, and calculated from the assumed speed 
	//of sound in air at sea level (~340 m/s).
	cm = pulse_width / 58.0;
	// Print out results
	
	/*if ( pulse_width > MAX_DIST ) {
		Serial.sendln("Out of range");
		cm = 0;
	}*/
	return pulse_width;
}