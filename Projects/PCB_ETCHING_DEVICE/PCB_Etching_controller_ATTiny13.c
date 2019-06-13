#ifndef F_CPU
#define F_CPU 1200000UL // or whatever may be your frequency
#endif

// Ports settigs
#define LED (1<<PB5)
#define POT (1<<PB4)
#define NTC (1<<PB3)
#define HEATER_1 (1<<PB2)
#define HEATER_2 (1<<PB1)
#define HEATER_3 (1<<PB0)

 
#include <avr/io.h>                    // adding header files
#include <util/delay.h>                // for _delay_ms()
#include <avr/interrupt.h>	       // for interuppts
#include <avr/wdt.h>                   // for wathdog timer
 
void pwm_setup(void);
void pwm_set_duty(uint8_t);
void pwm_update();
uint8_t pwm_return_duty(void);
void adc_setup(void);
uint16_t adc_read (void);
void interuppt_setup(void);
void watchdog_and_mean_setup(void);

// 1 degree step from 30 to 59
uint16_t temp_values[] = {455, 445, 434, 423, 413, 403, 393, 383, 373, 
363, 354, 345, 336, 327, 318, 310, 301, 
293, 285, 277, 270, 262, 255, 248, 241, 
234, 228, 221, 215, 209};


/*
ISR(INT0_vect)
{
	_delay_ms(10);
	if(KEY_UP)
	{
		if(current_status_of_pwm_cycle==PWM_MODE_TURN_OFF)
			current_status_of_pwm_cycle=PWM_MODE_TRIGGER;
		if(current_status_of_pwm_cycle==PWM_MODE_TURN_ON)
			if(main_counter<SUSTAIN_TIME-SUSTAIN_EXTENSION_TIME)
				main_counter += SUSTAIN_EXTENSION_TIME;
			else
				main_counter = SUSTAIN_TIME;
	}
}

ISR(WDT_vect)
{
	light_values[light_values_idx] = adc_read();
	if(light_values_idx<2*TIME_OF_MEAN)
		light_values_idx++;
	else
		light_values_idx = 0;
	MCUSR &= ~(1<<WDRF); // Disable reset of microcontroller
}
*/

int main(void)
{
	DDRB |= HEATER_1 | HEATER_2 | HEATER_3;// setting DDR of PORT B
	DDRB  &= ~POT;          // set KEY_PIN like input 
	DDRB  &= ~NTC;              // set LDR like input 
	PORTB &= ~HEATER_1; 
	PORTB &= ~HEATER_2; 
	PORTB &= ~HEATER_3; 

	//pwm_setup();
	//adc_setup();
	//interuppt_setup();
	//watchdog_and_mean_setup();
	
	//pwm_set_duty(0);



	//pwm_set_duty(100);  while(1){}

	while(1)
	{

	}


}

/*
void watchdog_and_mean_setup(void)
{
	int i;
	// set prescaler to 0.5s and enable Watchdog Timer
	WDTCR =  (1<<WDTIE) | (1<<WDP0) | (1<<WDP2);
	// disable reset of microcontroller
	WDTCR &= ~(1<<WDE);
	MCUSR &= ~(1<<WDRF);
	for(i=0;i<2*TIME_OF_MEAN;i++)
		light_values[i] = 0;
}
*/

/*
void interuppt_setup(void)
{
	GIMSK |= (1<<INT0);
	MCUCR |= (1<<ISC00) | (1<<ISC01); //INT0 rising edge
	//MCUCR |= (1<<ISC01); //INT0 falling edge

	sei();
}
*/

/*
void pwm_setup(void)
{
	TCCR0A |= (1 << WGM01) | (1 << WGM00); // turn on FastPWM mode
	TCCR0A |= (1<<COM0A1)|(1<<COM0A0); //Clear OC0A at TOP and set after compare

	//TCCR0B = (1<<CS00); //prescaler 1
	TCCR0B = (1<<CS01); //prescaler 8
	//TCCR0B = (1<<CS01)|(1<<CS00); //prescaler 64
	

}
*/

/*
void pwm_set_duty(uint8_t duty)
{
	OCR0A = 255 - duty;               // set PWM duty 
}
*/

/*
void pwm_update()
{
	pwm_set_duty(current_pwm_duty);
}
*/

/*
uint8_t pwm_return_duty(void)
{
	return OCR0A;
}
*/

/*
void adc_setup(void)
{
	// Set the ADC input to PB2/ADC1
	ADMUX |= (1 << MUX0) | (1 << MUX1);
	//ADMUX |= (1 << ADLAR);

	// Set the prescaler to clock/128 & enable ADC
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0);
	ADCSRA |= (1 << ADEN);

	// Turn off not necessery ADC's
	// if you turn off mux, where you use input it won't work
	//DIDR0 |= (1<<ADC0D) | (1<<ADC2D) | (1<<ADC3D);
}
*/

/*
uint16_t adc_read (void)
{
	// Start the conversion
	ADCSRA |= (1 << ADSC);

	// Wait for it to finish - blocking
	while (ADCSRA & (1 << ADSC));

	return ADC;
}
*/
