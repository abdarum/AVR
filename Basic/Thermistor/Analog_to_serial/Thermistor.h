#include "Thermistor.c"
unsigned int get_resistance(unsigned int resisistance_resistor, int adc_value);
float get_temperature_from_resistance(int resistance, int default_resistance,
    int nominal_temperature, int beta_param, float offset_temp);
float map_value_from_ADC(int min_val, int max_val, int adc_val);
