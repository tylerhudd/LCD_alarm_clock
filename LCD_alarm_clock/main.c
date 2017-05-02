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

int main()
{
	int state = 0;
	int stateSelect = 0;
	
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
			case 0 :		// display clock and alarm, menu select
				if((PIND & 0x01) == 0x01)	// if enter button pressed
				{
					state = stateSelect;
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