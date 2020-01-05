#ifndef F_CPU
#define F_CPU 128000UL // or whatever may be your frequency
#endif

// PCINT example:        https://forbot.pl/forum/topic/4409-c-attiny13niedzialajace-przerwanie-pcint/
// ATTiny13 datasheet:   https://ww1.microchip.com/downloads/en/devicedoc/doc2535.pdf

// Ports settigs
// #define LED (1<<PB5)
#define FORCE_WDG_IN (1<<PB4)
#define CONTROL_POWER_OUT (1<<PB3)
#define WDG_INT_IN_PIN (1<<PB2)
#define ARD_TX_PIN (1<<PB1)
#define ARD_RX_PIN (1<<PB0)

#define WDG_TIME_PERIOD 1200 // in seconds
#define WDG_POWER_OFF_PERIOD 60 // in seconds


#include <avr/io.h>                    // adding header files
#include <util/delay.h>                // for _delay_ms()

void interuppt_setup(void);
void watchdog_setup(void);

volatile int16_t remaining_time_to_reset = WDG_TIME_PERIOD;

ISR(WDT_vect)
{
	remaining_time_to_reset--;
	MCUSR &= ~(1<<WDRF); // Disable reset of microcontroller
}

ISR(PCINT0_vect)
{
	remaining_time_to_reset = WDG_TIME_PERIOD;
}

int main(void)
{
	DDRB |= CONTROL_POWER_OUT;  // setting DDR of PORT B
	DDRB  &= ~WDG_INT_IN_PIN;   // set WDG_INT_IN_PIN like input 
	DDRB  &= ~FORCE_WDG_IN;     // set FORCE_WDG_IN like input 

	interuppt_setup();
	watchdog_setup();

	while(1)
	{
		if(remaining_time_to_reset<=0)
		{
 ////////////////////////////////////////////////////////////////////////////
			// reset procedure
		}
		
	 
	}


}

void interuppt_setup(void)
{
	/**** INT0 interrupt ****/
	// GIMSK |= (1<<INT0); // INT0 activate
	// MCUCR |= (1<<ISC00) | (1<<ISC01); //INT0 rising edge
	//MCUCR |= (1<<ISC01); //INT0 falling edge

	/**** PCINT interrupt ****/
	GIMSK |= (1<<PCIE);   // PCINT activate
	PCMSK |= (1<<PCINT2); //PCINT choose pin
	GIFR |=(1<<PCIF);     // clear PCIF flag

	sei();
}

void watchdog_setup(void)
{
	int i;
	// set prescaler to 1s and enable Watchdog Timer
	WDTCR =  (1<<WDTIE) | (1<<WDP1) | (1<<WDP2);
	// disable reset of microcontroller
	WDTCR &= ~(1<<WDE);
	MCUSR &= ~(1<<WDRF);
}
