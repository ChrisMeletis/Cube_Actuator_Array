# Cube_Actuator_Array
I had a dream to create a small bedside table that would have small cubes in the front, each cube could be actuated in and out creating paterns. I set out to create the cube modules and connect them all toghether on a data bus. Cube positions travel using a slowed down neopixel library.

## Controller Main
Initially this would be an ESP system that would controll the actuator positions using some predefined tables. The goal was to provide a whole website where the user could connect and change the running pattern and create his own paterns stored on the microcotnroller.

## Peripheral Cube Tile
Consist of a Digispark Attiny USB that listens to the bus and sends the proper pwm signal to a miniature servo motor.

## Hardware
### Controller Main
 - Arduino Nano (ESP8266 NodeMCU)[Downsized due to project abandonment]
### Peripheral Cube
 - Digispark Attiny USB
 - Miuzel MF90 Micro Servo

## Current Project state
The project has entered development limbo sinse its too loud to be a Bed side table. I will share what has been accomplished so people can maybe at least benefit from this work.