/*
 * LCD_alarm_clock.c
 *
 * Authors : Tyler Huddleston & Georgi Genov
 * For UNLV CPE 310L Microcontrollers Lab
 *
 * Specifications : Stopwatch, Alarm, Clock: use timer to create a clock, alarm, 
 *					stopwatch and display in LCD.
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

// LCD pins
#define rs PD0
#define rw PD1
#define en PD2
#define data PORTB

// push button pins
#define enter PC0		// ENTER button
#define inc PC1			// INCREMENT button
#define dec PC2			// DECREMENT button

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
void incSec();							// increment clock seconds
void toggleAMPM(int CorA);				// toggle am/pm
int setTime(int CorA, int setState);	// set clock or alarm time
void printTimer();						// print the timer time
void stopwatch();

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
int runTimer;
int swSec;
int SWrun;

ISR(TIMER1_COMPA_vect)
{
	incSec();
}


int main()
{
	int stateSelect = 0;
	int clockSetState = 0;
	int alarmSetState = 0;
	int timerSetState = 0;
	
	state = 0;
	hr = 12;
	min = 0;
	sec = 0;
	ampm = 0;
	alarmHr = 11;
	alarmMin = 59;
	alarmSec = 0;
	alarmAmpm = 0;
	timerMin = 0;
	timerSec = 0;
	runTimer = 0;
	swSec = 0;
	SWrun = 0;
	
	sei();
	
	DDRB=0XFF;			// PORT B: data register for LCD set to output
	DDRC=0x00;			// PORT C: push button inputs
	DDRD=0XFF;			// PORT D: PD0:2 LCD commands output

	lcd_init();
	displayMenuOption(stateSelect);
	
	
	TCCR1B = 0x0C;	// compare value in OCR1A and 1024 prescaler
	TIMSK1 = 0x02;	// timer mask
	OCR1A = 36000;
	
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
		
		
		if(alarmHr == hr && alarmMin == min && alarmAmpm == ampm) state = 5;
		
		switch(state)
		{
			case 0 :	// display clock and alarm, menu select
				if((PINC & 0x01) == 0x01)	// if enter button pressed
				{
					state = stateSelect;
					switch(state)
					{
						case 1 :  // initial procedure for clock set
							lcd_cmd(0x01);	//clear screen
							lcd_str("Clock: set hour");
							break;
						case 2 : // initial procedure for alarm set
							lcd_cmd(0x01);	//clear screen
							lcd_str("Alarm: set hour");
							break;
						case 3 : // initial procedure for stopwatch
							lcd_cmd(0x01);
							lcd_str("Stopwatch");
							lcd_cmd(0xC0);
							SWrun=1;
							break;
						case 4:	// initial procedure for timer
							
							break;
					}
				}
				else if((PINC & 0x02) == 0x02)	// if increment button pressed
				{
					if(stateSelect == 4) stateSelect = 0;
					else stateSelect++;
					displayMenuOption(stateSelect);
				}
				else if((PINC & 0x04) == 0x04)	// if decrement button pressed
				{
					if(stateSelect == 0) stateSelect = 4;
					else stateSelect--;
					displayMenuOption(stateSelect);
				}
				else
				{
					lcd_cmd(0x80);
					printClock(0);
				}
				break;
			
			
			
			case 1 :	// set clock
				clockSetState = setTime(0, clockSetState);
				if (clockSetState == 3)
				{
					clockSetState = 0;
					state = 0;
					stateSelect = 0;
					lcd_cmd(0x01);
					printClock(0);
					displayMenuOption(stateSelect);
				}
				break;
			
			
			
			
			case 2:	// set alarm
				alarmSetState = setTime(1, alarmSetState);
					if (alarmSetState == 3)
					{
						alarmSetState = 0;
						state = 0;
						stateSelect = 0;
						lcd_cmd(0x01);
						printClock(0);
						displayMenuOption(stateSelect);
					}
				break;
				
				
			case 3:	// stopwatch
				stopwatch();
				if((PINC & 0x01) == 0x01 && timerMin > 0)	// if enter button pressed
				{
					state = 0;
					stateSelect = 0;
					lcd_cmd(0x01);
					printClock(0);
					displayMenuOption(stateSelect);
					swSec = 0;
					SWrun = 0;
				}
				break;
				
				
			case 4:	// timer
				if((PINC & 0x01) == 0x01 && timerMin > 0)	// if enter button pressed
				{
					runTimer = 1;
				}
				if(!runTimer)
				{
					if((PINC & 0x02) == 0x02)	// if increment button pressed
					{
						incMin(2, 0);
					}
					if((PINC & 0x04) == 0x04)	// if decrement button pressed
					{
						decMin(2);
					}
				}
				//printTimer();
				break;
			
			case 5:	// sound the alarm
				for(int i=0; i<3; i++)
				{
					lcd_cmd(0x01);
					_delay_ms(200);
					lcd_str("!!!!!!Alarm!!!!!!");
					lcd_cmd(0xC0);
					lcd_str("*!*!*!*!*!*!*!*!");
					_delay_ms(200);
					alarmHr = 11;
					alarmMin = 59;
				}
				state = 0;
				lcd_cmd(0x01);
				displayMenuOption(stateSelect);
				break;
				
			default: state = 0;
			
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
			lcd_str("MCU Clock-GG&TH");
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
			if(hr<10) sprintf(hrChar, "0%i", hr);
			else sprintf(hrChar, "%i", hr);
			
			if(min<10) sprintf(minChar, "0%i", min);
			else sprintf(minChar, "%i", min);
			
			if (ampm == 1) ampmChar = 'P';
			break;
		case 1: // for alarm
			if(alarmHr<10) sprintf(hrChar, "0%i", alarmHr);
			else sprintf(hrChar, "%i", alarmHr);
			
			if(alarmMin<10) sprintf(minChar, "0%i", alarmMin);
			else sprintf(minChar, "%i", alarmMin);
			
			if (alarmAmpm == 1) ampmChar = 'P';
			break;
	}
	
	lcd_data(hrChar[0]);
	lcd_data(hrChar[1]);
	lcd_str(":");
	lcd_str(minChar);
	lcd_data(ampmChar);
	lcd_data('M');
	// for(int i=0; i<7; i++) lcd_str(" ");
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
			if(min == 59)
			{
				min = 0;
				if(set == 0) incHr(0);
			}
			else min++;
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

void incSec()
{
	if(sec==59)
	{
		sec = 0;
		incMin(0,0);
	}
	else sec++;
	if(SWrun) swSec++;
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
	if((PINC & 0x02) == 0x02)	// if increment button pressed
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
	if((PINC & 0x04) == 0x04)	// if decrement button pressed
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
	if((PINC & 0x01) == 0x01)	// if enter button pressed
	{
		switch(setState)
		{
			case 0:
				lcd_cmd(0x01);
				lcd_cmd(0x80);
				if(!CorA) lcd_str("Clock: ");
				else lcd_str("Alarm: ");
				lcd_str("set mins");
				setState ++;
				break;
			case 1:
				lcd_cmd(0x01);
				lcd_cmd(0x80);
				if(!CorA) lcd_str("Clock: ");
				else lcd_str("Alarm: ");
				lcd_str("set AM/PM");
				setState ++;
				break;
			case 2:
				setState ++;
				break;
			default:
				state = 0;
				setState = 0;
				break;
		}
	}
	return setState;
}

void stopwatch(int swsec)
{
	char mins = '0';
	char secs[2];
	
	sprintf(mins, "%i", swsec/60);
	if(swsec%60<10) sprintf(secs, "0%i", swsec%60);
	else sprintf(secs, "%i", swsec%60);
	
	lcd_cmd(0xc0);
	lcd_data(mins);
	lcd_str(":");
	lcd_data(secs[0]);
	lcd_data(secs[1]);
}
