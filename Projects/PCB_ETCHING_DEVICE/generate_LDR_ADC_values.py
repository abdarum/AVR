#! /bin/bash python
import math
import numpy

max_value_adc = 1023 #10 bit
series_resistor_value = 10000 #10k
default_NTC_resistance = 10000 #10k
nominal_temperature = 25
beta_coefficient_param = 3950


def calculate_resistance(temp_sensing):
    diff_inv_temp = 1/float(temp_sensing+273.15)\
            -1/float(nominal_temperature+273.15) 
    resist = float(default_NTC_resistance)*math.exp(\
            beta_coefficient_param*diff_inv_temp)
    return int(math.floor(resist))

def get_adc_val_from_devider(gnd_resist, vcc_resist):
    divisor = float(gnd_resist)/(vcc_resist+gnd_resist);
    return int(math.floor(divisor*max_value_adc))

def generate_table_of_values(min_temp, max_temp, step):
    tmp_list = list()
    for i in range(min_temp, max_temp, step):
        tmp_list.append(get_adc_val_from_devider(\
                calculate_resistance(i),series_resistor_value))
    print(len(tmp_list))
    return tmp_list

def generate_table_of_values_float_step(min_temp, max_temp, step):
    tmp_list = list()
    for i in numpy.arange(min_temp, max_temp, step):
        tmp_list.append(get_adc_val_from_devider(\
                calculate_resistance(i),series_resistor_value))
    print(len(tmp_list))
    return tmp_list

def print_list_as_c_array(list_of_elements, name_of_array = None, 
        type_of_array=None):
    string_of_array = ""
    if name_of_array:
        string_of_array += str(type_of_array)+' '+\
                str(name_of_array)+'[] = {'
    else:
        string_of_array += '{ '

    for i in range(len(list_of_elements)):
        string_of_array += str(list_of_elements[i])+', '
        if i%8==0 and i!=0 and i!=(len(list_of_elements)-1):
            string_of_array += '\n'
    string_of_array = string_of_array[:-2]
    string_of_array += '};\n'
    print(string_of_array)
    return string_of_array


tmp = generate_table_of_values_float_step(30,60,1)
print(tmp)
print_list_as_c_array(tmp, 'temp_values', 'uint16_t')

