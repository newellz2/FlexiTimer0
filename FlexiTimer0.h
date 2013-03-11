#ifndef FlexiTimer0_h
#define FlexiTimer0_h

#ifdef __AVR__
#include <avr/interrupt.h>
#else
#error FlexiTimer0 library only works on AVR architecture
#endif


namespace FlexiTimer0 {
	extern unsigned long time_units;
	extern void (*func)();
	extern void (*func2)();
	extern volatile unsigned long count;
	extern volatile char overflowing;
	extern volatile unsigned long count2;
	extern volatile char overflowing2;
	extern volatile unsigned int tcnt0;
	
	void set(unsigned long ms, void (*f)(),void (*f2)());
	void set(unsigned long units, double resolution, void (*f)(),void (*f2)() );
	void start();
	void stop();
	void _overflow();
	void _overflow2();
}

#endif
