# esp32-temperature-ds18b20
Meassure temperature with a ds18b20 tmperature sensor and send the data to a remote server. The identity of the remote server is verified by ist root certificate and https is used to secure the communication.


### meassure battery voltage (without calibration)
Since LiPo batteries (usually) supply a voltage between ~4.2 and ~3.2 volts it is necessary to convert the voltage to match the ADC input voltage range. To meassure the battery voltage via ADC you will need a voltage divider. 

#### choose attenuation
The 0dB attenuation showed the most accurate results for the uesed esp32 device in the lower voltage range, so I choose this attenuation. 

Your esp32 might behave differently. So some testing will be necessary.

attenuation | input voltage range
----------------------------- | ----------------------------- 
analogSetAttenuation(ADC_0db); |  input voltage between 0V and 1.1V
analogSetAttenuation(ADC_2_5db); |  input voltage between 0V and 1.5V
analogSetAttenuation(ADC_6db); |  input voltage between 0V and 2.2V
analogSetAttenuation(ADC_11db); |  input voltage between 0V and 3.3V 


References and further reading:
- https://www.esp32.com/viewtopic.php?t=1045 ADC input voltage
- https://www.esp32.com/viewtopic.php?t=2919 voltage mismatch ADC reading
- http://esp-idf.readthedocs.io/en/latest/api-reference/peripherals/adc.html ESP32 ADC API reference
