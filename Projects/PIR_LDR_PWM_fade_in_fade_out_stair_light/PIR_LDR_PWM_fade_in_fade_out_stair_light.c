#ifndef F_CPU
#define F_CPU 1200000UL // or whatever may be your frequency
#endif

// Ports settigs
#define LED (1<<PB0)
#define POT (1<<PB2)
#define RELAY (1<<PB4)
#define LDR (1<<PB3)

#define KEY_PIN (1<<PB1)          
#define KEY_DOWN !(PINB & KEY_PIN) 
#define KEY_UP (PINB & KEY_PIN)

// Fade mode settings
#define DELAY_BEFORE_FADE_IN 500

#define FADE_MODE_FIXED_STEP 0
#define FADE_STEP_TIME 8

#define FADE_MODE_FIXED_FADE_TIME_STEP 1
#define FADE_WHOLE_TIME_STEP FADE_WHOLE_TIME/PWM_FINAL_VALUE


#define FADE_MODE_FIXED_FADE_TIME 2
#define FADE_WHOLE_TIME 2000

#define FADE_MODE FADE_MODE_FIXED_FADE_TIME_STEP

#define SUSTAIN_TIME 12000
#define SUSTAIN_EXTENSION_TIME 8000

// LDR values 59, 40
#define LDR_THRESHOLD 40
// LDR_MODE: 1 - Greather or equal LDR turn on light
// 	     0 - Lower or equal LDR turn on light
#define LDR_MODE 0


// PWM final level
#define PWM_FINAL_VALUE 100


// Stages of working PWM
#define PWM_MODE_TURN_OFF 0
#define PWM_MODE_TRIGGER 1
#define PWM_MODE_FADE_IN 2
#define PWM_MODE_TURN_ON 3
#define PWM_MODE_FADE_OUT 4

#define TIME_OF_MEAN 5

 
#include <avr/io.h>                    // adding header files
#include <util/delay.h>                // for _delay_ms()
#include <avr/interrupt.h>	       // for interuppts
#include <avr/wdt.h>                   // for wathdog timer
 
uint8_t push_button(void);    
void pwm_setup(void);
void pwm_set_duty(uint8_t);
void pwm_update();
uint8_t pwm_return_duty(void);
void adc_setup(void);
uint16_t adc_read (void);
void interuppt_setup(void);
void fade_in(uint8_t);
void fade_out(void);
void full_cycle_of_pwm(void);
uint8_t light_brghtness_comparator(uint16_t);
void watchdog_and_mean_setup(void);
void calculate_light_mean(void);

volatile uint16_t main_counter = SUSTAIN_TIME;
uint8_t current_pwm_duty = 0;
volatile uint8_t current_status_of_pwm_cycle = PWM_MODE_TURN_OFF;
volatile uint16_t light_values[2*TIME_OF_MEAN];
volatile uint16_t mean_light_value = 0;
volatile uint8_t light_values_idx = 0;

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

int main(void)
{
	DDRB |= LED | RELAY;        // setting DDR of PORT B
	DDRB  &= ~KEY_PIN;          // set KEY_PIN like input 
	DDRB  &= ~LDR;              // set LDR like input 
	PORTB |=  KEY_PIN;          // pull-up to Vcc
	PORTB &= ~RELAY; 

	pwm_setup();
	adc_setup();
	interuppt_setup();
	watchdog_and_mean_setup();
	
	pwm_set_duty(0);



	//pwm_set_duty(100);  while(1){}

	while(1)
	{
		if(current_status_of_pwm_cycle==PWM_MODE_TRIGGER)
		{
			calculate_light_mean();
			if(light_brghtness_comparator(mean_light_value))
			{
				PORTB |= RELAY;
				full_cycle_of_pwm();
				PORTB &= ~RELAY; 
			}
		}
	}


}

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
void interuppt_setup(void)
{
	GIMSK |= (1<<INT0);
	MCUCR |= (1<<ISC00) | (1<<ISC01); //INT0 rising edge
	//MCUCR |= (1<<ISC01); //INT0 falling edge

	sei();
}

void fade_in(uint8_t final_value)
{
	uint16_t fade_step_time = FADE_WHOLE_TIME/(final_value - 
			current_pwm_duty); 
	while(current_pwm_duty<final_value)
	{
		if(FADE_MODE == FADE_MODE_FIXED_STEP)
			_delay_ms(FADE_STEP_TIME);	
		if(FADE_MODE == FADE_MODE_FIXED_FADE_TIME_STEP)
			_delay_ms(FADE_WHOLE_TIME_STEP);		
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
		if(FADE_MODE == FADE_MODE_FIXED_FADE_TIME_STEP)
			_delay_ms(FADE_WHOLE_TIME_STEP);		
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
	_delay_ms(DELAY_BEFORE_FADE_IN);
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
	ADMUX |= (1 << MUX0) | (1 << MUX1);
	//ADMUX |= (1 << ADLAR);

	// Set the prescaler to clock/128 & enable ADC
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0);
	ADCSRA |= (1 << ADEN);

	// Turn off not necessery ADC's
	// if you turn off mux, where you use input it won't work
	//DIDR0 |= (1<<ADC0D) | (1<<ADC2D) | (1<<ADC3D);
}

uint16_t adc_read (void)
{
	// Start the conversion
	ADCSRA |= (1 << ADSC);

	// Wait for it to finish - blocking
	while (ADCSRA & (1 << ADSC));

	return ADC;
}

uint8_t light_brghtness_comparator(uint16_t ldr_value)
{
	if(LDR_MODE)
	{
		if(ldr_value >= LDR_THRESHOLD)
			return 1;
	}
	else
	{
		if(ldr_value <= LDR_THRESHOLD)
			return 1;
	}
	return 0;

}

void calculate_light_mean(void)
{
	uint16_t tmp=0;
	int i;
	for(i=0;i<2*TIME_OF_MEAN;i++)
	{
		if(65535-tmp<light_values[i])
			tmp = 65535;
		else
			tmp += light_values[i];
	}
	mean_light_value = tmp/(2*TIME_OF_MEAN);
}
