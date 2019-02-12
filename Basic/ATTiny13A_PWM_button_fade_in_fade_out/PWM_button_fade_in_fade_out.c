#ifndef F_CPU
#define F_CPU 1200000UL // or whatever may be your frequency
#endif

// Ports settigs
#define LED (1<<PB0)
#define POT (1<<PB2)

#define KEY_PIN (1<<PB3)          
#define KEY_DOWN !(PINB & KEY_PIN) 

// Fade mode settings
#define FADE_MODE_FIXED_STEP 0
#define FADE_STEP_TIME 8

#define FADE_MODE_FIXED_FADE_TIME 1
#define FADE_WHOLE_TIME 2000

#define FADE_MODE FADE_MODE_FIXED_FADE_TIME

#define SUSTAIN_TIME 8000
#define SUSTAIN_EXTENSION_TIME 2000

// PWM final level
#define PWM_FINAL_VALUE 128

// Stages of working PWM
#define PWM_MODE_TURN_OFF 0
#define PWM_MODE_FADE_IN 1
#define PWM_MODE_TURN_ON 2
#define PWM_MODE_FADE_OUT 3

 
#include <avr/io.h>                    // adding header files
#include <util/delay.h>                // for _delay_ms()
#include <avr/interrupt.h>		// for interuppts
 
uint8_t push_button(void);    
void pwm_setup(void);
void pwm_set_duty(uint8_t);
void pwm_update();
uint8_t pwm_return_duty(void);
void adc_setup(void);
uint8_t adc_read (void);
void fade_in(uint8_t);
void fade_out(void);
void full_cycle_of_pwm(void);

volatile uint16_t main_counter = SUSTAIN_TIME;
uint8_t current_pwm_duty = 0;
volatile uint8_t current_status_of_pwm_cycle = PWM_MODE_TURN_OFF;

int main(void)
{
	DDRB |= LED;                // setting DDR of PORT B
	DDRB  &= ~KEY_PIN;          // set KEY_PIN like input 
	PORTB |=  KEY_PIN;          // pull-up to Vcc
	
	pwm_setup();
	adc_setup();
	
	uint8_t pwm_level = adc_read();
	pwm_update();

	while(1)
	{
		if(push_button())
		{
			full_cycle_of_pwm();
		}

	}
}


void fade_in(uint8_t final_value)
{
	uint16_t fade_step_time = FADE_WHOLE_TIME/(final_value - 
			current_pwm_duty); 
	while(current_pwm_duty<final_value)
	{
		if(FADE_MODE == FADE_MODE_FIXED_STEP)
			_delay_ms(FADE_STEP_TIME);	
		if(FADE_MODE == FADE_MODE_FIXED_FADE_TIME)
		{
			uint16_t i=0;
			for(i=0;i<fade_step_time;i++)	_delay_ms(1);
		}
		current_pwm_duty++;
		pwm_update();
	}
}

void fade_out(void)
{
	uint16_t fade_step_time = FADE_WHOLE_TIME/current_pwm_duty; 
	while(current_pwm_duty>0)
	{
		if(FADE_MODE == FADE_MODE_FIXED_STEP)
			_delay_ms(FADE_STEP_TIME);		
		if(FADE_MODE == FADE_MODE_FIXED_FADE_TIME)
		{
			uint16_t i=0;
			for(i=0;i<fade_step_time;i++)	_delay_ms(1);
		}
		current_pwm_duty--;
		pwm_update();
	}

}


void full_cycle_of_pwm(void)
{
	current_status_of_pwm_cycle = PWM_MODE_FADE_IN;
	fade_in(PWM_FINAL_VALUE);
	current_status_of_pwm_cycle = PWM_MODE_TURN_ON;
	main_counter = SUSTAIN_TIME;
	while(main_counter>0)
	{
		_delay_ms(1);
		main_counter--;
	}
	current_status_of_pwm_cycle = PWM_MODE_FADE_OUT;
	fade_out();
	current_status_of_pwm_cycle = PWM_MODE_TURN_OFF;
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
	

	//sei(); //enable global interuppts
}
	

void pwm_set_duty(uint8_t duty)
{
	OCR0A = 255 - duty;               // set PWM duty 
}

void pwm_update()
{
	pwm_set_duty(current_pwm_duty);
}

uint8_t pwm_return_duty(void)
{
	return OCR0A;
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
	// if you turn off mux, where you use input it won't work
	//DIDR0 |= (1<<ADC0D) | (1<<ADC2D) | (1<<ADC3D);
}

uint8_t adc_read (void)
{
	// Start the conversion
	ADCSRA |= (1 << ADSC);

	// Wait for it to finish - blocking
	while (ADCSRA & (1 << ADSC));

	return ADCH;
}

