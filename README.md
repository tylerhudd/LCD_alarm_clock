# LCD Alarm Clock

This project is for the UNLV CPE 310L microcontrolloers lab.  It is a clock, alarm clock, stopwatch, and timer implemented on the ATmega328P AVR microcontroller.

## Operation
The device has 6 states:
* *0* - display clock, alarm clock set time, and menu options
* *1* - set clock
* *2* - set alarm
* *3* - stopwatch
* *4* - timer
* *5* - sound/display alarm

**State 0 : Main screen**
* Show clock and alarm across the first row
* Show menu selection options on second row
  * Cycle through menu with increment and decrement buttons
  * Choose selection with enter button

**State 1 : Set clock**
* 1. Increment/decrement hours to set, then push enter
* 2. Increment/decrement mins to set, then push enter
* 3. Choose AM or PM then push enter
* 4. On enter, return to main screen

**State 2 : Set alarm**
* 1. Increment/decrement hours to set, then push enter
* 2. Increment/decrement mins to set, then push enter
* 3. Choose AM or PM then push enter
* 4. On enter, return to main screen

**State 3 : Stopwatch**
Buttons change to:
* 1. Start - count seconds and minutes until Start pushed again
* 2. Reset - reset stopwatch to 00:00
* 3. Return - return to main screen

**State 4 : Timer**
* 1. Increment/decrement minutes, then push enter
* 2. On enter, decrement from set time
* 3. Return button - return to main screen

**State 5 : Alarm**
* Signal alarm


## Components used with microcontroller
* **Pushbuttons**:
  * *Enter* - for confirming selection
  * *Increment* - for incrementing through menu, incrementing hours and minutes, etc.
  * *Decrement* - for decrementing through options.
* **LCD**: 16x2

## Authors

* **Tyler Huddleston** - *Initial work* - [tylerhudd](https://github.com/tylerhudd)
* **Georgi Genov** - *Initial work*

## Built With
* Atmel Studio
