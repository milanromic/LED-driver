/* Copyright (C) 2014  Milan Romic */

#include "hardware_mega8.h"

void init()
{
	    
	// Declare your local variables here

	// Input/Output Ports initialization
	// Port B initialization
	// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=Out
	// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=0
	PORTB=0x00;
	DDRB=0x01;

	// Port C initialization
	// Func6=In Func5=Out Func4=Out Func3=Out Func2=Out Func1=In Func0=In
	// State6=T State5=0 State4=0 State3=0 State2=0 State1=T State0=T
	PORTC=0x00;
	DDRC=0x3C;

	// Port D initialization
	// Func7=Out Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
	// State7=0 State6=T State5=T State4=T State3=T State2=T State1=T State0=T
	PORTD=0x00;
	DDRD=0x80;

	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: Timer 0 Stopped
	TCCR0=0x00;
	TCNT0=0x00;

	// Timer/Counter 1 initialization
	// Clock source: System Clock
	// Clock value: 2000.000 kHz
	// Mode: CTC top=OCR1A
	// OC1A output: Discon.
	// OC1B output: Discon.
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	// Timer1 Overflow Interrupt: Off
	// Input Capture Interrupt: Off
	// Compare A Match Interrupt: On
	// Compare B Match Interrupt: Off
	TCCR1A=0x00;
	TCCR1B=0x0A;
	TCNT1H=0x00;
	TCNT1L=0x00;
	ICR1H=0x00;
	ICR1L=0x00;
	OCR1AH=0x00;
	OCR1AL=0x63;
	OCR1BH=0x00;
	OCR1BL=0x00;

	// Timer/Counter 2 initialization
	// Clock source: System Clock
	// Clock value: Timer2 Stopped
	// Mode: Normal top=0xFF
	// OC2 output: Disconnected
	ASSR=0x00;
	TCCR2=0x00;
	TCNT2=0x00;
	OCR2=0x00;

	// External Interrupt(s) initialization
	// INT0: Off
	// INT1: Off
	MCUCR=0x00;

	// Timer(s)/Counter(s) Interrupt(s) initialization
	TIMSK=0x10;

	// USART initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART Receiver: On
	// USART Transmitter: On
	// USART Mode: Asynchronous
	// USART Baud Rate: 9600
	UCSRA=0x00;
	UCSRB=0xD8;
	UCSRC=0x86;
	UBRRH=0x00;
	UBRRL=0x5F; // XTAL = 14.7456 MHz
	//UBRRL=0x67; // XTAL = 16MHz

	// Analog Comparator initialization
	// Analog Comparator: Off
	// Analog Comparator Input Capture by Timer/Counter 1: Off
	ACSR=0x80;
	SFIOR=0x00;

	// ADC initialization
	// ADC disabled
	ADCSRA=0x00;

	// SPI initialization
	// SPI disabled
	SPCR=0x00;

	// TWI initialization
	// TWI disabled
	TWCR=0x00;

	// Global enable interrupts
	asm("sei");
	
	/* Startup values */
	red_duty = 0;
	red_duty_wish = 0;
	green_duty = 0;
	green_duty_wish = 0;
	blue_duty = 0;
	blue_duty_wish = 0;
	white_duty = 0;
	white_duty_wish = 0;
}

unsigned int GetRXindex()
{
	return rx_wr_index;
}

void SetRXindex(int index)
{
	rx_wr_index = index;
}

unsigned char GetRXitem(int index)
{
	return rx_buffer[index];
}

unsigned int GetRXtimeout()
{
	return rx_timeout;
}

void SetRXtimeout(int time)
{
	rx_timeout = time;
}

void BurnRXArray(unsigned char * data)
{
	for(int i = 0; i < RX_BUFFER_SIZE; i++)
	{
		data[i] = rx_buffer[i];
	}
}

ISR(USART_RXC_vect)
{
	char status,data;
	status=UCSRA;
	data=UDR;
	if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
	{
		rx_buffer[rx_wr_index++] = data;

		rx_timeout = 0;
		#if RX_BUFFER_SIZE == 256
		// special case for receiver buffer size=256
		if (++rx_counter == 0)
		{
			#else
			if (rx_wr_index == RX_BUFFER_SIZE) rx_wr_index=0;
			if (++rx_counter == RX_BUFFER_SIZE)
			{
				rx_counter=0;
				#endif
				rx_buffer_overflow=1;
			}
		}
	}

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART Receiver buffer
#define _ALTERNATE_GETCHAR_

char GetChar(void)
{
	char data;
	while (rx_counter==0);
	data=rx_buffer[rx_rd_index++];
	#if RX_BUFFER_SIZE != 256
	if (rx_rd_index == RX_BUFFER_SIZE) rx_rd_index=0;
	#endif
	asm("cli");
	--rx_counter;
	asm("sei");
	return data;
}

#endif

// USART Transmitter buffer
#define TX_BUFFER_SIZE 64
unsigned char tx_buffer[TX_BUFFER_SIZE];

#if TX_BUFFER_SIZE <= 256
unsigned char tx_wr_index,tx_rd_index,tx_counter;
#else
unsigned int tx_wr_index,tx_rd_index,tx_counter;
#endif

// USART Transmitter interrupt service routine
ISR(USART_TXC_vect)
{
	if (tx_counter)
	{
		--tx_counter;
		UDR=tx_buffer[tx_rd_index++];
		#if TX_BUFFER_SIZE != 256
		if (tx_rd_index == TX_BUFFER_SIZE) tx_rd_index=0;
		#endif
	}
}

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART Transmitter buffer
#define _ALTERNATE_PUTCHAR_

void PutChar(char c)
{
	while (tx_counter == TX_BUFFER_SIZE);
	asm("cli");
	if (tx_counter || ((UCSRA & DATA_REGISTER_EMPTY)==0))
	{
		tx_buffer[tx_wr_index++]=c;
		#if TX_BUFFER_SIZE != 256
		if (tx_wr_index == TX_BUFFER_SIZE) tx_wr_index=0;
		#endif
		++tx_counter;
	}
	else
	UDR=c;
	asm("sei");
}

#endif

// Timer1 output compare A interrupt service routine
ISR (TIMER1_COMPA_vect)
{
	/* Increment PWM counter */
	pwm_cnt++;
	/* Increment RX timeout */
	rx_timeout++;
	/* Increment Accelerator */
	LED_accel++;
	
	/* Set RED */
	if(pwm_cnt < red_duty)
	{
		PORTC |= 0b00000100;
	}
	else if(pwm_cnt >= red_duty)
	{
		PORTC &= 0b11111011;
	}
	
	/* Set GREEN */
	if(pwm_cnt < green_duty)
	{
		PORTC |= 0b00001000;
	}
	else if(pwm_cnt >= green_duty)
	{
		PORTC &= 0b11110111;
	}
	
	/* Set BLUE */
	if(pwm_cnt < blue_duty)
	{
		PORTC |= 0b00010000;
	}
	else if(pwm_cnt >= blue_duty)
	{
		PORTC &= 0b11101111;
	}
	
	/* Set WHITE */
	if(pwm_cnt < white_duty)
	{
		PORTC |= 0b00100000;
	}
	else if(pwm_cnt >= white_duty)
	{
		PORTC &= 0b11011111;
	}
	
	/* Reset PWM counter */
	if(pwm_cnt >= 200)
	{
		pwm_cnt = 0;
	}
	
	/* Acceleration algorithm */
	if(LED_accel >= 10)
	{
		/* Reset LED accelerator */
		LED_accel = 0;
		
		if(red_duty > red_duty_wish)
		{
			red_duty--;
		}
		else
		{
			red_duty++;
		}
		if(green_duty > green_duty_wish)
		{
			green_duty--;
		}
		else
		{
			green_duty++;
		}
		if(blue_duty > blue_duty_wish)
		{
			blue_duty--;
		}
		else
		{
			blue_duty++;
		}
		if(white_duty > white_duty_wish)
		{
			white_duty--;
		}
		else
		{
			white_duty++;
		}
	}
}
/* Set wish of Red LED */
void SetRedDutyWish(unsigned int wish)
{
	red_duty_wish = wish;
}
/* Set wish of Green LED */
void SetGreenDutyWish(unsigned int wish)
{
	green_duty_wish = wish;
}
/* Set wish of Blue LED */
void SetBlueDutyWish(unsigned int wish)
{
	blue_duty_wish = wish;
}
/* Set wish of White LED */
void SetWhiteDutyWish(unsigned int wish)
{
	white_duty_wish = wish;
}