# PWM on LED fade in and fade after PIR sensor high value 

PWM with changing duty of PWM to fade in and fade out. Maximum PWM duty following is set by constant PWM\_FINAL\_VALUE

After PB1 is high: if LED is turn off it will start fade in, light for a moment and fade out. If LED is during this process time of high state will be increased.


## Pinout:

PB0 - LED out

PB1 - Push button pulled down

PB4 - Relay will turn on 12V power supply 
