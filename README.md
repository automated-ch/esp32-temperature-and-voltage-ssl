# esp32 temperature measurement with ds18b20 including battery status
Meassure temperature with a ds18b20 tmperature sensor and send the data to a remote server. The identity of the remote server is verified by its root certificate and https is used to secure the communication withe remote server.

The battery voltage is also measured via ADC and sent to the server.


### measure temperature with a ds18B20 
To meassure the temperature with a ds18b20 sensor you need a pullup resistor with 4.7K Ohm.


References:
- https://community.blynk.cc/t/ds18b20-value-127-but-only-when-connecting-to-blynk-on-esp32/18336
- https://github.com/milesburton/Arduino-Temperature-Control-Library arduino Dallas temperature
- https://www.milesburton.com/Dallas_Temperature_Control_Library Dallas Library

### measure battery voltage (without calibration)
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


### meassure

For the voltage divider following resistors are used:

BAT (+) --- 51 K -- 5.1 K --- ADC PIN --- 20K --- GND (-)

![image](https://user-images.githubusercontent.com/48131740/152804452-82f5e47e-a08a-4abb-a92d-ce296ec25099.png)

