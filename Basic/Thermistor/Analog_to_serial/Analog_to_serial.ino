#include "Thermistor.h"
#include <stdint.h>
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  unsigned int therm_resist = get_resistance(10000, sensorValue);
  float temp = get_temperature_from_resistance(therm_resist,
      10000, 25, 3950, -0.88);
  Serial.print("ADC: ");
  Serial.print(sensorValue);
  Serial.print("; Res: ");
  Serial.print(therm_resist);
  Serial.print("; Temp: ");
  Serial.print(temp);
  Serial.println("*C");
}
