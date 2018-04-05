/*Tenniel Takenaka-Fuller*/
/*CPE 301 Design Assignment 3*/

/*Library Functions*/
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>

/*Baud rate for ports*/
#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD -1

/*Global variables*/
volatile uint8_t ADCvalue; // Volatile if used with interrupts  

int main( void )
{
	
	//---------------------------------------------------------------
	// INITIALIZE ADC VALUES
	//---------------------------------------------------------------

	ADMUX = 0; // use ADC0
	ADMUX |= (1 << REFS0); // use AVcc as the reference
	ADMUX |= (1 << ADLAR); // Right adjust for 8 bit resolution
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 prescale for 16Mhz
	ADCSRA |= (1 << ADATE); // Set ADC Auto Trigger Enable
	ADCSRB = 0; // 0 for free running mode
	ADCSRA |= (1 << ADEN); // Enable the ADC
	ADCSRA |= (1 << ADIE); // Enable Interrupts
	ADCSRA |= (1 << ADSC); // Start the ADC conversion
	
	
	//---------------------------------------------------------------
	// INITIALIZE UART VALUES 
	//---------------------------------------------------------------
	
	/*Set baud rate */
	UBRR0H = ((MYUBRR) >> 8); //Shift to store the upper 8 bits
	UBRR0L = MYUBRR; //Store lower bits

	UCSR0B |= (1 << RXEN0) | (1 << TXEN0); // Enable receiver and transmitter
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); // Set frame: 8data, 1 stop, no parity bit
	
	
	//---------------------------------------------------------------
	// INITIALIZE TIMER INTERRUPT VALUES (Timer 1)
	//---------------------------------------------------------------
	TCCR1A = 0; //Normal mode
	TCCR1B = (1 << CS12) | (1 << CS10); //Prescaler is 1024
	
	TCNT1 = 49911; //Calculation to start the timer for overflow interrupt
	TIMSK1 = (1 << TOIE1); //Enable Timer 1 Overflow Interrupt
	
	sei(); // Enables the interrupts
	
	
	while(1)
	{
		; // Do nothing
	}
}


ISR(ADC_vect)
{
	ADCvalue = ADCH * 2; // Manipulate ADCH value to get temperature
}


ISR(TIMER1_OVF_vect) {
	TCNT1 = 49911; //Calculation to start the timer
	TIFR1 = (1 << TOV1); //Interrupt flag register gets overflow flag set when overflow occurs
	while(!(UCSR0A & (1 << UDRE0))); //If UDRE0 is 1, buffer is empty & ready to be written to
	UDR0 = ADCvalue; //Send the temperature value to the graph
}
