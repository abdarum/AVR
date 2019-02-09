#ifndef F_CPU
#define F_CPU 1200000UL // or whatever may be your frequency
#endif

#define LED (1<<PB0)
#define KEY_PIN (1<<PB3)          
#define KEY_DOWN !(PINB & KEY_PIN) 
 
#include <avr/io.h>                    // adding header files
#include <util/delay.h>                // for _delay_ms()
 
uint8_t push_button(void);    

int main(void)
{
	DDRB |= LED;                // setting DDR of PORT B
	DDRB  &= ~KEY_PIN;          // set KEY_PIN like input 
	PORTB |=  KEY_PIN;          // pull-up to Vcc
	TCCR0A |= (1 << WGM01) | (1 << WGM00); // turn on FastPWM mode
	TCCR0A |= (1<<COM0A1); //Set OC0A at TOP and clear after compare

	//TCCR0B = (1<<CS00); //prescaler 1
	TCCR0B = (1<<CS01); //prescaler 8
	//TCCR0B = (1<<CS01)|(1<<CS00); //prescaler 64


	uint8_t level[] = {0, 10, 72, 255};   // 4 intensity level 
	uint8_t i = 0;                      
	

	OCR0A = level[i];               // set PWM duty 

	while(1)
	{
	
		if( push_button() )
		{
			i++;
			if( i >= 4) i = 0;

			OCR0A = level[i]; 
			_delay_ms(200);
		}
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

// References
// http://niki.art.pl/index.php/avr/54-attiny13-i-sprzetowy-pwm
