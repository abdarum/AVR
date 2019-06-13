unsigned int get_resistance(unsigned int resisistance_resistor, int adc_value){
  // change to uint16_t if needed
  float out_resistance = 0;
  out_resistance = resisistance_resistor/ (1023-adc_value);
  out_resistance *= adc_value;
  return (int)out_resistance;
}

float get_temperature_from_resistance(int resistance, 
    int default_resistance, int nominal_temperature,
    int beta_param, float offset_temp){
  // source: https://learn.adafruit.com/thermistor/using-a-thermistor
  // resistance of thermistor:
  // resistance
  // resistance at 25 degrees C:
  // default_resistance e.g. 10000      
  // temp. for nominal resistance (almost always 25 C):
  // nominal_temperature e.g. 25   
  // The beta coefficient of the thermistor (usually 3000-4000):
  // beta_param e.g. 3950
  float temp;
  temp = (float)resistance / (float)default_resistance;     // (R/Ro)
  temp = log(temp);                  // ln(R/Ro)
  temp /= (float)beta_param;                   // 1/B * ln(R/Ro)
  temp += 1.0 / ((float)nominal_temperature + 273.15); // + (1/To)
  temp = 1.0 / temp;                 // Invert
  temp -= 273.15;                         // convert to C
  temp += offset_temp;
  return temp;
}

int map_value_from_ADC(int min_val, int max_val, int adc_val){
//  int 
  return 0;
}
