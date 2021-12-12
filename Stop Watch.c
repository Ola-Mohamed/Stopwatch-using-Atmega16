/*
 * MiniProject1.c
 *
 *  Created on: 5 May 2021
 *      Author: OlaMohamedAhmed
 *       Diploma 54
 */


#include<avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


//Global variable for the clock system
unsigned char tick=0;
unsigned char  clockSecond=0;
unsigned char  clockMinute=0;
unsigned char  clockHour=0;


void Timer1_CTC_Init(void){

	/* Configure the timer mode
		 * 1. Non PWM mode FOC1A = 1 & FOC1B = 1
		 * 2. CTC Mode WGM10 = 0 & WGM11 = 0 & WGM12 = 1 & WGM13 = 0
		 * 3. clock = CPU clock/1024 CS10 = 1 CS11 = 0 CS12 = 1
	 */


	TCCR1A = (1<<FOC1A);

	TCCR1B = (1<<WGM12)|(1<<CS10)|(1<<CS11);

	OCR1A = 1000;


	SREG|=(1<<7); //enable interrupt when compare

	TCNT1 =0;     // Set Timer initial value to 0

	TIMSK |= (1<<OCIE1A);// Enable Timer1 Compare Interrupt
}
//this ISR is called exactly at 1ms interval
ISR(TIMER1_COMPA_vect){


	clockSecond ++;
		if (clockSecond == 60){
			clockSecond = 0;
			clockMinute ++;
		}
		if (clockMinute == 60){
			clockSecond= 0;
			clockMinute = 0;
			clockHour ++;
		}
		if (clockHour == 12){
			clockSecond= 0;
			clockMinute = 0;
			clockHour = 0 ;
		}

	}

void INT0_Reset(void)
{
	DDRD &=~(1<<PD2);
	PORTD|=(1<<PD2);
    // internal pull up

	// interrupt 0
	MCUCR|=(1<<ISC01);	// Falling edge
	GICR |=(1<<INT0);
	SREG |=(1<<7);
}
ISR(INT0_vect)
{
	clockSecond = 0;
	clockMinute = 0;
	clockHour = 0 ;
}



void INT1_stop(){
	DDRD &= ~(1<<PD3);
		// Configure interrupt 1
	MCUCR|=(1<<ISC10)|(1<<ISC11);	// Raising edge
	GICR |=(1<<INT1);   //interrupt 1 enable
	SREG|=(1<<7);
}
ISR(INT1_vect)
{
	//this ISR to make timer1 off
	TCCR1B &= ~(1<<CS10)&~(1<<CS11)&~(1<<CS12);
}
void INT2_Resume(void)
{
	DDRB &=~(1<<PB2);
	PORTB |= (1<<PB2);   // internal pull up

	// interrupt 2
	MCUCR&=~(1<<ISC2);	// falling edge
	GICR |=(1<<INT2);
	SREG|=(1<<7);
}
ISR(INT2_vect)
{
	TCCR1B = (1<<WGM12)|(1<<CS10)|(1<<CS11);
}

int main(){
	DDRA = 0xff;	//configure the 7seg data pins to input
	PORTA = 0xff;
	DDRC = 0x0f;	//configure the 7seg data pins to output
	PORTC = 0x00;  // at beginning all LEDS off


	INT0_Reset();
	INT1_stop();
	INT2_Resume();
	Timer1_CTC_Init();


	while(1){

		PORTA = (1<<5);
			PORTC = clockSecond % 10;
			_delay_ms(5);
			PORTA = (1<<4);
			PORTC = clockSecond / 10;
			_delay_ms(5);
			PORTA = (1<<3);
			PORTC = clockMinute % 10;
			_delay_ms(5);
			PORTA = (1<<2);
			PORTC = clockMinute / 10;
			_delay_ms(5);
			PORTA = (1<<1);
			PORTC = clockHour % 10;
			_delay_ms(5);
			PORTA = (1<<0);
			PORTC = clockHour / 10;
			_delay_ms(5);

}

}
