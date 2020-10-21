/*
 * EXAM.c
 *
 * Created: 20.11.2017 12:58:09
 * Author : MSI
 *	Wiktoria Kos
 */ 

#define F_CPU 16E6
#include <avr/io.h> //necessary libraries
#include <stdio.h>

#include <util/delay.h>
#include "i2cmaster.h" 
#include "lcd.h"


typedef struct     //new data type
{
	int minutes;
	int seconds;
}time_t;	//name of new data type


time_t timer;//global variables
int counter=0; 
int flag=0;

void oven_mode();	//function prototypes
void timer_decrease();
void timer_increase();
void print_time();
void oven_on_off();
void binary_pattern();


int main(void)
{
	DDRD = 0xFF; //I/O board:PD4…7 as outputs, for LEDs
	DDRC = 0xF0; //I/O board PC0…3 as inputs, for buttons
	PORTC = 0x3F; // Enable internal pull at PC0..3 inputs
	PORTD = 0x00; // Set output LEDs to off

	timer.minutes=0;//initial values
	timer.seconds=0;

	lcd_init();
	
	while(1)
	{
		if (PINC == 0b00111110) //1st button
		{
			oven_mode();
			_delay_ms(100);
			binary_pattern();
		}
		
		_delay_ms(50);
		
		if (PINC == 0b00111101) //2nd button
		timer_decrease();
		_delay_ms(50);
		
		if (PINC == 0b00111011) //3rd button
		timer_increase();
		_delay_ms(50);
		
		if (PINC == 0b00110111) //4th button
		oven_on_off();
		_delay_ms(50);
	}

}

void print_time(void)//function definition: printing time value
{
	lcd_clear();
	lcd_gotoxy(2,7);//writing in the middle of row
	
	printf("%02d:%02d", timer.minutes,timer.seconds);
	
	lcd_gotoxy(1,counter);//printing in 1st row counter's column
	printf("%d",counter);
}

void oven_mode(void) //function definition: selection of oven mode (1-7) - 1st button press
{
	counter=counter+1;
	if (counter>7)
	counter=0;
	
	print_time();
}

void timer_decrease(void)//function definition: decreasing timer - 2nd button
{
	timer.minutes=timer.minutes-1;
	if (timer.minutes<0)//cannot go below 0
	timer.minutes=0;
	print_time();
}

void timer_increase(void)////function definition: increasing timer - 3nd button
{
	timer.minutes=timer.minutes+1;
	if (timer.minutes>100)//cannot go above 100
	timer.minutes=100;
	print_time();
}

void oven_on_off(void)//function definition: turning on/off the oven ; timer is counting down - 4th button
{
	if (flag) //flag- to define whether oven should be on or off ; 1-ON/0-OFF
		flag=flag-1;
	else 
		flag=flag+1;
	
	if (flag==0)//oven is OFF - everything gets initial values - 0 
	{
		lcd_clear();
		timer.minutes=0;
		timer.seconds=0;
		counter=0;
		PORTD = 0b00000000;//switch off all LEDs
		
	}
	
	
	if (flag==1)//oven ON
	{
		PORTD = 0b10000000;//switch on LED 4
		while ((timer.seconds>0 || timer.minutes>0)&&flag==1)//starting countdown if possible
		{
			
				timer.seconds=timer.seconds-1;
				if (timer.seconds<0)
				{
					timer.seconds=60;
					timer.minutes=timer.minutes-1;
					if(timer.minutes<0)
					timer.minutes=0;
				}
			print_time();
			_delay_ms(900); //due to lcd.clear we take smaller value 900ms to get close to life result 
							//1000ms should be to get real life timer (1s) ; but delay is not so precise tool
			if (PINC == 0b00110111) //4th button
			flag=0;
		}
		if((timer.seconds==0 && timer.minutes==0)||flag==0)//if countdown is finished oven is OFF
		{
			lcd_clear();
			timer.minutes=0;
			timer.seconds=0;
			counter=0;
			PORTD = 0b00000000;//switch off all LEDs
			flag=0;
		}
	}
	
}

void binary_pattern()
{
	switch(counter)
	{
		case 1:
		PORTD = 0b00010000;//turning on particular LEDs to get binary pattern-0001
		break;
		case 2:
		PORTD = 0b00100000;//turning on particular LEDs to get binary pattern-0010
		break;
		case 3:
		PORTD = 0b00110000;//turning on particular LEDs to get binary pattern-0011
		break;
		case 4:
		PORTD = 0b01000000;//turning on particular LEDs to get binary pattern-0100
		break;
		case 5:
		PORTD = 0b01010000;//turning on particular LEDs to get binary pattern-0101
		break;
		case 6:
		PORTD = 0b01100000;//turning on particular LEDs to get binary pattern-0110
		break;
		case 7:
		PORTD = 0b01110000;//turning on particular LEDs to get binary pattern-0111
		break;
	}
	
	
}