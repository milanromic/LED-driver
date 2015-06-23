/* Copyright (C) 2014  Milan Romic */

#ifndef HARDWARE_MEGA8_H_
#define HARDWARE_MEGA8_H_

#define F_CPU 14745600UL
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef RXB8
#define RXB8 1
#endif

#ifndef TXB8
#define TXB8 0
#endif

#ifndef UPE
#define UPE 2
#endif

#ifndef DOR
#define DOR 3
#endif

#ifndef FE
#define FE 4
#endif

#ifndef UDRE
#define UDRE 5
#endif

#ifndef RXC
#define RXC 7
#endif

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

// USART Receiver buffer
#define RX_BUFFER_SIZE 64
unsigned char rx_buffer[RX_BUFFER_SIZE];

unsigned int rx_timeout;

#if RX_BUFFER_SIZE <= 256
unsigned char rx_wr_index,rx_rd_index,rx_counter;
#else
unsigned int rx_wr_index,rx_rd_index,rx_counter;
#endif

// This flag is set on USART Receiver buffer overflow
char rx_buffer_overflow;

unsigned char
red_duty,
red_duty_wish,
green_duty,
green_duty_wish,
blue_duty,
blue_duty_wish,
white_duty,
white_duty_wish;

unsigned int pwm_cnt, LED_accel;

void init();

unsigned int GetRXindex();
void SetRXindex(int index);

unsigned int GetRXtimeout();
void SetRXtimeout(int time);

unsigned char GetRXitem(int index);
void BurnRXArray(unsigned char * data);

char GetChar(void);
void PutChar(char c);

/* Set wish of each LED */
void SetRedDutyWish(unsigned int wish);
void SetGreenDutyWish(unsigned int wish);
void SetBlueDutyWish(unsigned int wish);
void SetWhiteDutyWish(unsigned int wish);

#endif