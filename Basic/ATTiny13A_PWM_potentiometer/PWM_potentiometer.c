#ifndef F_CPU
#define F_CPU 1200000UL // or whatever may be your frequency
#endif

#define LED (1<<PB0)
#define POT (1<<PB2)

#define KEY_PIN (1<<PB3)          
#define KEY_DOWN !(PINB & KEY_PIN) 
 
#include <avr/io.h>                    // adding header files
#include <util/delay.h>                // for _delay_ms()
 
uint8_t push_button(void);    
void pwm_setup(void);
void pwm_set_duty(uint8_t);
void adc_setup(void);
uint8_t adc_read (void);

int main(void)
{
	DDRB |= LED;                // setting DDR of PORT B
	DDRB  &= ~KEY_PIN;          // set KEY_PIN like input 
	PORTB |=  KEY_PIN;          // pull-up to Vcc
	
	pwm_setup();
	adc_setup();


	
	while(1)
	{
		pwm_set_duty(adc_read());
	}
}


uint8_t push_button(void)
{
	if(KEY_DOWN)
	{
		_delay_ms(80);
		if(KEY_DOWN) return 1;
	}
	return 0;
}

void pwm_setup(void)
{
	TCCR0A |= (1 << WGM01) | (1 << WGM00); // turn on FastPWM mode
	TCCR0A |= (1<<COM0A1)|(1<<COM0A0); //Clear OC0A at TOP and set after compare

	//TCCR0B = (1<<CS00); //prescaler 1
	TCCR0B = (1<<CS01); //prescaler 8
	//TCCR0B = (1<<CS01)|(1<<CS00); //prescaler 64
}
	

void pwm_set_duty(uint8_t duty)
{
	OCR0A = 255 - duty;               // set PWM duty 
}
void adc_setup(void)
{
	// Set the ADC input to PB2/ADC1
	ADMUX |= (1 << MUX0);
	ADMUX |= (1 << ADLAR);

	// Set the prescaler to clock/128 & enable ADC
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0);
	ADCSRA |= (1 << ADEN);

	// Turn off not necessery ADC's
	DIDR0 |= (1<<ADC0D) | (1<<ADC2D) | (1<<ADC3D);
}

uint8_t adc_read (void)
{
	// Start the conversion
	ADCSRA |= (1 << ADSC);

	// Wait for it to finish - blocking
	while (ADCSRA & (1 << ADSC));

	return ADCH;
}

// References
// https://adnbr.co.uk/articles/adc-and-pwm-basics
