/* Copyright (C) 2014  Milan Romic */

#include "avr_utils.h"

/* Get LOW byte of int */
unsigned char getLOWbyte(unsigned int input)
{
	return input & 255;
}

/* Get HIGH byte of int */
unsigned char getHIGHbyte(unsigned int input)
{
	return (input >> 8) & 255;
}

/* Combine HIGH and LOW bytes */
unsigned int combineBytes(unsigned char high, unsigned char low)
{
	return (int) (high << 8) + low;
}