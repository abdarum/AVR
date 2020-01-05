#ifndef F_CPU
#define F_CPU 128000UL // or whatever may be your frequency
#endif

// PCINT example:        https://forbot.pl/forum/topic/4409-c-attiny13niedzialajace-przerwanie-pcint/
// ATTiny13 datasheet:   https://ww1.microchip.com/downloads/en/devicedoc/doc2535.pdf

// Ports settigs
#define FORCE_WDG_IN (1<<PB4)
#define CONTROL_POWER_OUT (1<<PB3)
#define WDG_INT_IN_PIN (1<<PB2)
#define ARD_TX_PIN (1<<PB1)
#define ARD_RX_PIN (1<<PB0)

#define FORCE_WDG_DOWN !(PINB & FORCE_WDG_IN) 

#define WDG_TIME_PERIOD 1200 // in seconds
#define WDG_POWER_OFF_PERIOD 60 // in seconds


#include <avr/io.h>                    // adding header files
#include <util/delay.h>                // for _delay_ms()
#include <avr/interrupt.h>	       // for interuppts
#include <avr/wdt.h>                   // for wathdog timer

void interuppt_setup(void);
void watchdog_setup(void);
uint8_t watchdog_is_forced(void);    
void turn_on_output_power(void);
void turn_off_output_power(void);
void make_hard_reset_procedure(void);

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
	turn_on_output_power();

	interuppt_setup();
	watchdog_setup();


	while(1)
	{
		if(watchdog_is_forced()){
			turn_on_output_power();
		} 
		else
		{
			if(remaining_time_to_reset<=0)
			{
				make_hard_reset_procedure();
			}
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

uint8_t watchdog_is_forced(void)
{
	if(FORCE_WDG_DOWN)
	{
		_delay_ms(80);
		if(FORCE_WDG_DOWN) return 1;
	}
	return 0;
}

void turn_on_output_power(void)
{
	PORTB |= CONTROL_POWER_OUT;
}

void turn_off_output_power(void)
{
	PORTB &=~CONTROL_POWER_OUT; 
}

void make_hard_reset_procedure(void)
{
	cli();
	uint16_t i = 0;
	turn_off_output_power();
	for(i=0;i<WDG_POWER_OFF_PERIOD;i++)	
		_delay_ms(1000);
	turn_on_output_power();
	sei();
}
