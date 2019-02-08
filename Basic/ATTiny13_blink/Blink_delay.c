/*=====================================================
|   TASK:  TOGGLING THE LED ATTACHED TO A PARTICULAR PORT
|   DIFFICULTY LEVEL: VERY BASIC
|   Author:  BISWARUP BANERJEE
|   Language:  EMBEDDED C
|   To Compile:  RUN THE SCRIPT AS MENTIONED BY THE AUTHOR
|
|   Date:  June 2017
|
================================================*/
#ifndef F_CPU
#define F_CPU 8000000UL // or whatever may be your frequency
#endif

#define LED PB4
 
#include <avr/io.h>                    // adding header files
#include <util/delay.h>                // for _delay_ms()
 
int main(void)
{
   DDRB = (1<<DDB4);                       // setting DDR of PORT B
   while(1)
   {
       // LED on
       PORTB |= (1<<LED);            // PB4 = High = LED attached on PB4 is ON
       _delay_ms(500);                // wait 500 milliseconds
 
       //LED off
       PORTB &=~(1<<LED);            // PB4 = Low = LED attached on PB4 is OFF
       _delay_ms(500);                // wait 500 milliseconds
   }
}
