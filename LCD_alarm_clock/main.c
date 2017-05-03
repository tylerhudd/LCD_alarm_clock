/*
 * LCD_alarm_clock.c
 *
 * Authors : Tyler Huddleston & Georgi Genov
 * For UNLV CPE 310L Microcontrollers Lab
 *
 * Specifications : Stopwatch, Alarm, Clock: use timer to create a clock, alarm, 
 *					stopwatch and display in LCD.
 */ 

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

// LCD pins
#define rs PD0
#define rw PD1
#define en PD2
#define data PORTB

// push button pins
#define enter PD0		// ENTER button
#define inc PD1			// INCREMENT button
#define dec PD2			// DECREMENT button

// function declarations
void lcd_init();						// clears LCD screen and sets position
void lcd_cmd(char cmd_out);				// sends a command across the LCD command bits
void lcd_data(char data_out);			// sends data across the LCD data register
void lcd_str(char *str);				// prints a string to the LCD screen
void displayMenuOption(int stateSel);	// display menu option on stateSelect change
void printClock(int CorA);				// print clock or alarm clock
void incHr(int CorA);					// increment clock or alarm hours
void decHr(int CorA);					// decrement clock or alarm hours
void incMin(int CorAorT, int set);		// increment clock or alarm mins
void decMin(int CorAorT);				// decrement clock or alarm mins
void toggleAMPM(int CorA);				// toggle am/pm
int setTime(int CorA, int setState);	// set clock or alarm time

// global variables
int state;
int hr;
int min;
int sec;
int ampm;
int alarmHr;
int alarmMin;
int alarmSec;
int alarmAmpm;
int timerMin;
int timerSec;


int main()
{
	int stateSelect = 0;
	int clockSetState = 0;
	int alarmSetState = 0;
	
	state = 0;
	hr = 12;
	min = 0;
	sec = 0;
	ampm = 0;
	alarmHr = 12;
	alarmMin = 0;
	alarmSec = 0;
	alarmAmpm = 0;
	timerMin = 0;
	timerSec = 0;
	
	
	DDRB=0XFF;			// PORT B: data register for LCD set to output
	DDRD=0XF8;			// PORT D: PD0:2 push button input, PD3:5 LCD commands output

	lcd_init();
	
	while(1)
	{
		/*
			STATE 0: Display, menu select - display clock and alarm set time, cycle through menu
			STATE 1: Set clock - set clock hours, minutes, and am/pm
			STATE 2: Set alarm - set alarm hours, minutes, and am/pm
			STATE 3: Stopwatch mode - counts minutes and seconds up
			STATE 4: Timer mode - set time, decrements minutes and seconds
			STATE 5: Alarm - display alarm message
		*/
		
		switch(state)
		{
			case 0 :	// display clock and alarm, menu select
				if((PIND & 0x01) == 0x01)	// if enter button pressed
				{
					state = stateSelect;
					switch(state)
					{
						case 1 :  // initial procedure for clock set
							lcd_cmd(0x01);	//clear screen
							lcd_str("Set hours");
							break;
						case 2 : // initial procedure for alarm set
							
							break;
						case 3 : // initial procedure for stopwatch
							
							break;
						case 4:	// initial procedure for timer
							
							break;
					}
				}
				if((PIND & 0x02) == 0x02)	// if increment button pressed
				{
					if(stateSelect == 4) stateSelect = 0;
					else stateSelect++;
					displayMenuOption(stateSelect);
				}
				if((PIND & 0x04) == 0x04)	// if decrement button pressed
				{
					if(stateSelect == 0) stateSelect = 4;
					else stateSelect--;
					displayMenuOption(stateSelect);
				}
				break;
			
			case 1 :	// set clock
				clockSetState = setTime(0, clockSetState);
				break;
			
			case 2:	// set alarm
				alarmSetState = setTime(1, alarmSetState);
				break;
				
			case 3:	// stopwatch
				break;
				
			case 4:	// timer
				break;
			
			case 5:	// sound the alarm
				break;
			
			if(alarmHr == hr && alarmMin == min && alarmAmpm == ampm) state = 5;
			
			_delay_ms(5);
		}
	}
}

void lcd_init()
{
	lcd_cmd(0X38);
	lcd_cmd(0X0E);
	lcd_cmd(0X01);
	lcd_cmd(0X80);
}

void lcd_cmd(char cmd_out)
{
	data=cmd_out;
	PORTD=(0<<rs)|(0<<rw)|(1<<en);
	_delay_ms(10);
	PORTD=(0<<rs)|(0<<rw)|(0<<en);
	_delay_ms(10);
}

void lcd_data(char data_out)
{
	data=data_out;
	PORTD=(1<<rs)|(0<<rw)|(1<<en);
	_delay_ms(10);
	PORTD=(1<<rs)|(0<<rw)|(0<<en);
	_delay_ms(10);
}

void lcd_str(char *str)
{
	unsigned int i=0;
	while(str[i]!='\0')
	{
		lcd_data(str[i]);
		i++;
	}
}

void displayMenuOption(int stateSel)
{
	// clear bottom row
	lcd_cmd(0xC0);
	for(int i=0; i < 16; i++) lcd_data(' ');
	lcd_cmd(0xC0);
	
	switch(stateSel)
	{
		case 0 :
			lcd_str("MCU Clock - GG & TH");
		break;
		case 1 :
			lcd_str("-> Clock Set");
		break;
		case 2 :
			lcd_str("-> Alarm Set");
		break;
		case 3 :
			lcd_str("-> Stopwatch");
		break;
		case 4 :
			lcd_str("-> Timer");
		break;
	}
}

void printClock(int CorA)
{
	char hrChar[2];
	char minChar[2];
	char ampmChar = 'A';
	
	switch(CorA)
	{
		case 0:	// for clock
			sprintf(hrChar, "%i", hr);
			sprintf(minChar, "%i", min);
			if (ampm == 1) ampmChar = 'P';
			break;
		case 1: // for alarm
			sprintf(hrChar, "%i", alarmHr);
			sprintf(minChar, "%i", alarmMin);
			if (alarmAmpm == 1) ampmChar = 'P';
			break;
	}
	
	lcd_str(hrChar);
	lcd_str(":");
	lcd_str(minChar);
	lcd_data(ampmChar);
	lcd_data('M');
}

void incHr(int CorA)
{
	switch(CorA)
	{
		case 0:	// for clock
			if(hr == 12) hr = 1;
			else hr++;
			break;
		case 1:	// for alarm
			if(alarmHr == 12) alarmHr = 1;
			else alarmHr++;
			break;
	}
}

void decHr(int CorA)
{
	switch(CorA)
	{
		case 0:	// for clock
			if(hr == 1) hr = 12;
			else hr--;
			break;
		case 1:	// for alarm
			if(alarmHr == 1) alarmHr = 12;
			else alarmHr--;
			break;
	}
}

void incMin(int CorAorT, int set)
{
	switch(CorAorT)
	{
		case 0: // for clock
			if(min == 59) min = 0;
			else min++;
			if(set == 0) hr++;
			break;
		case 1:	// for alarm
			if(alarmMin == 59) alarmMin = 0;
			else alarmMin ++;
			break;
		case 2:	// for timer
			if (timerMin < 60) timerMin++;	// 1 hour max for timer
			break;
	}
}

void decMin(int CorAorT)
{
	switch(CorAorT)
	{
		case 0:	// for clock
			if(min == 0) min = 59;
			else min --;
			break;
		case 1:	// for alarm
			if(alarmMin == 0) alarmMin = 59;
			else alarmMin --;
			break;
		case 2:	// for timer
			if (timerMin > 0) timerMin--;
			break;
	}
}

void toggleAMPM(int CorA)
{
	switch(CorA)
	{
		case 0:	// for clock
			if(ampm == 0) ampm = 1;
			else ampm = 0;
			break;
		case 1:	// for alarm
			if(alarmAmpm == 0) alarmAmpm = 1;
			else alarmAmpm = 0;
			break;
	}
}

int setTime(int CorA, int setState)
{
	lcd_cmd(0xC0);	// set position [2,1]
	printClock(CorA);
	if((PIND & 0x02) == 0x02)	// if increment button pressed
	{
		switch(setState)
		{
			case 0:	// set hours
				incHr(CorA);
				break;
			case 1:	// set mins
				incMin(CorA, 1);
				break;
			case 2: // set AM/PM
				toggleAMPM(CorA);
				break;
		}
	}
	if((PIND & 0x04) == 0x04)	// if decrement button pressed
	{
		switch(setState)
		{
			case 0:	// set hours
				decHr(CorA);
				break;
			case 1:	// set mins
				decMin(CorA);
				break;
			case 2: // set AM/PM
				toggleAMPM(CorA);
				break;
		}
	}
	if((PIND & 0x01) == 0x01)	// if enter button pressed
	{
		switch(setState)
		{
			case 0:
				lcd_cmd(0x01);
				lcd_str("Set minutes");
				setState ++;
				break;
			case 1:
				lcd_cmd(0x01);
				lcd_str("Set AM/PM");
				setState ++;
				break;
			case 2:
				state = 0;
				setState = 0;
				break;
		}
	}
	return setState;
}