# patpatScreen

## This is a partial code for the PATPAT project.

## Used Hardware: 
- Raspberry Pi 4B
- 2.9inch Touch E-Paper E-Ink Display HAT for Raspberry Pi, 5-Points Capacitive Touch, 296×128, Black / White, SPI [product detail](https://www.waveshare.com/2.9inch-touch-e-paper-hat.htm) 


## Reference
- [Manual Book](https://www.waveshare.com/wiki/2.9inch_Touch_e-Paper_HAT_Manual#Raspberry_Pi)
- [code] (https://github.com/waveshareteam/Touch_e-Paper_HAT)

## What does it do?
- Get local weather and condition
- Get random memo
- Support multiple pet character icon
- Support touch interface


## Project Summary
As mentioned above, this is only the hardware section for the PATPAT project, which allows to plug and play with Raspberry Pi.


By executing the code, the E-paper will display the city name, time, date, temperature in C, and weather conditions. Below those information, the display will show a radome memo per mintue, and the pet charaction icon at the bottom.

Pet character icons are allowed to switch by touching the figure and will keep the same figure until the programe is disconnected or changed.

## How to run it:
- sudo make clean
- sudo make -j4
- make

easy, right?


## Future Plan
- Add pet animation
- re-organize the display formatting
- Synchronize pet characters with exisitng account
