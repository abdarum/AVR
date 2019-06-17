#ifndef F_CPU
#define F_CPU 128000UL // or whatever may be your frequency
#endif

// Ports settigs
#define LED (1<<PB5)
#define POT (1<<PB4)
#define NTC (1<<PB3)
#define HEATER_1 (1<<PB0)
#define HEATER_2 (1<<PB1)
#define HEATER_3 (1<<PB2)

#define MIN_TEMP 30
#define MAX_TEMP 59
#define NUMBER_OF_TEMP_VALUES 30

#define ADC_VAL_HYSTERESIS 3

#define ERROR_NO_TERMISTOR_ADC_VALUE 900

 
#include <avr/io.h>                    // adding header files
#include <util/delay.h>                // for _delay_ms()
 
//void pwm_setup(void);
//void pwm_set_duty(uint8_t);
//void pwm_update();
//uint8_t pwm_return_duty(void);
void adc_setup(void);
uint16_t adc_read (void);

void turn_on_heater(void);
void turn_off_heater(void);
void select_potentiometer_adc_mux(void);
void select_thermistor_adc_mux(void);
uint8_t get_idx_of_temp(void);
void check_temp_and_set_heater(void);

// 1 degree step from 30 to 59
uint16_t temp_values[] = {455, 445, 434, 423, 413, 403, 393, 383, 373, 
363, 354, 345, 336, 327, 318, 310, 301, 
293, 285, 277, 270, 262, 255, 248, 241, 
234, 228, 221, 215, 209};

int main(void)
{
	DDRB |= HEATER_1 | HEATER_2 | HEATER_3;// setting DDR of PORT B
	DDRB  &= ~POT;          // set KEY_PIN like input 
	DDRB  &= ~NTC;              // set LDR like input 
	turn_off_heater();

	//pwm_setup();
	adc_setup();
	
	//pwm_set_duty(0);



	//pwm_set_duty(100);  while(1){}

	while(1)
	{
		/*
		// LED on
		turn_on_heater();
		_delay_ms(500);                // wait 500 milliseconds

		//LED off
		turn_off_heater();
		_delay_ms(500);                // wait 500 milliseconds
		*/

		check_temp_and_set_heater();
	 
	}


}

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
void turn_on_heater(void)
{
	       PORTB |= HEATER_1;
	       PORTB |= HEATER_2;
	       PORTB |= HEATER_3;
}

void turn_off_heater(void)
{
	       PORTB &=~HEATER_1; 
	       PORTB &=~HEATER_2; 
	       PORTB &=~HEATER_3; 
}

void select_potentiometer_adc_mux(void)
{
	// Set the ADC input to PB4/ADC2
	ADMUX &= ~(1 << MUX0);
	ADMUX |= (1 << MUX1);
}

void select_thermistor_adc_mux(void)
{
	// Set the ADC input to PB3/ADC3
	ADMUX |= (1 << MUX0) | (1 << MUX1);
}

void adc_setup(void)
{
	// Turn on if needed only 8bit and read from ADCH
	//ADMUX |= (1 << ADLAR);
	// Set the ADC input to PB3/ADC3
	//ADMUX |= (1 << MUX0) | (1 << MUX1);
	// Set the ADC input to PB4/ADC2
	//ADMUX &= ~(1 << MUX0);
	//ADMUX |= (1 << MUX1);
	//select_potentiometer_adc_mux();

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

uint8_t get_idx_of_temp(void)
{
	select_potentiometer_adc_mux();
	return (adc_read()*(NUMBER_OF_TEMP_VALUES-1))/1023;
}

void check_temp_and_set_heater(void)
{
	select_thermistor_adc_mux();
	uint16_t thermistor_adc_val = adc_read();
	if (thermistor_adc_val > ERROR_NO_TERMISTOR_ADC_VALUE) 
	{
		turn_off_heater();
	} else
	{
		if (thermistor_adc_val > 
				(temp_values[get_idx_of_temp()]+ADC_VAL_HYSTERESIS))
		{
			turn_on_heater();
		} else if (thermistor_adc_val < 
				(temp_values[get_idx_of_temp()]-ADC_VAL_HYSTERESIS))
		{
			turn_off_heater();
		}
	}
}
