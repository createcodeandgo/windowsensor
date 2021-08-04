# windowsensor
An ongoing project... 
A microcontroller monitors the status (open/closed) of my windows and sends it to a Raspberry Pi . The Pi keeps statistics about everything and publishes the current status over MQTT. The Arduinos connect to the Raspberry Pi over nRF24L01 modules.

### Components used:
- Arduino Pro Micro
- nRF24L01 module for every Arduino
- CNY70 distance sensor for every Arduino
- planned: a temperature sensor on some Arduinos

### Code
The arduino code is written with the Arduino IDE and the Raspberry Pi uses python scripts.

### Circuit design:

![all components](windowsensor_circuit.png "all components")

![simplified view](full_circuit.png "simplified view")