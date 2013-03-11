/*
  FlexiTimer0.h - Using timer2 with a configurable resolution
  Wim Leers <work@wimleers.com>

  Based on MsTimer2
  Javier Valencia <javiervalencia80@gmail.com>

  History:
    16/Dec/2011 - Added Teensy/Teensy++ support (bperrybap)
		   note: teensy uses timer4 instead of timer2
    25/April/10 - Based on MsTimer2 V0.5 (from 29/May/09)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <FlexiTimer0.h>

unsigned long FlexiTimer0::time_units;
void (*FlexiTimer0::func)();
void (*FlexiTimer0::func2)();
volatile unsigned long FlexiTimer0::count;
volatile char FlexiTimer0::overflowing;
volatile unsigned int FlexiTimer0::tcnt0;

volatile unsigned long FlexiTimer0::count2;
volatile char FlexiTimer0::overflowing2;

void FlexiTimer0::set(unsigned long ms, void (*f)(), void (*f2)() ) {
    FlexiTimer0::set(ms, 0.001, f, f2);
}


/**
 * @param resolution
 *   0.001 implies a 1 ms (1/1000s = 0.001s = 1ms) resolution. Therefore,
 *   0.0005 implies a 0.5 ms (1/2000s) resolution. And so on.
 */
void FlexiTimer0::set(unsigned long units, double resolution, void (*f)(),void (*f2)()) {
	float prescaler = 0.0;
	
	if (units == 0)
		time_units = 1;
	else
		time_units = units;
		
	func = f;
	func2 = f2;

	TCCR0A = 0;
	TCCR0B = 0;


	// Set mode to CTC
	TCCR0A |= (1 << WGM01);
	ASSR &= ~(1<<AS2);

	//Remove Overflow
    TIMSK0  &= ~(1<<TOIE0);
	//Remove Comparator A
	TIMSK0 &= ~(1<<OCIE0A);
	//Remove Comparator B
	TIMSK0 &= ~(1<<OCIE0B);
	
	//Modify prescaler
	TCCR0B |= (1 << CS11) | (1 << CS10); 
	//TCCR0B |= (1<<CS22);
	//TCCR0B &= ~((1<<CS21) | (1<<CS20));
	prescaler = 64.0;

	// 250 CTC = 1ms
	OCR0A = 250;
	OCR0B = 250;


	
	tcnt0 = 256 - (int)((float)F_CPU * resolution / prescaler);
}

void FlexiTimer0::start() {
	count = 0;
	overflowing = 0;
	TCNT0 = tcnt0;
	OCR0A = tcnt0;
	OCR0B = tcnt0;
	TIMSK0 |= (1<<OCIE0A);
	TIMSK0 |= (1<<OCIE0B);
}

void FlexiTimer0::stop() {
	TIMSK0 &= ~(1<<OCIE0A);
}

void FlexiTimer0::_overflow() {
	count += 1;
	
	if (count >= time_units && !overflowing) {
		overflowing = 1;
		count = count - time_units; // subtract time_uints to catch missed overflows
					// set to 0 if you don't want this.
		(*func)();
		overflowing = 0;
	}
}

void FlexiTimer0::_overflow2() {
	count2 += 1;
	
	if (count2 >= time_units && !overflowing2) {
		overflowing2 = 1;
		count2 = count2 - time_units; // subtract time_uints to catch missed overflows
					// set to 0 if you don't want this.
		(*func2)();
		overflowing2 = 0;
	}
}

ISR(TIMER0_COMPA_vect) {
	TCNT0 = FlexiTimer0::tcnt0;
	FlexiTimer0::_overflow();
}

ISR(TIMER0_COMPB_vect) {
	TCNT0 = FlexiTimer0::tcnt0;
	FlexiTimer0::_overflow2();
}


