/* Copyright (C) 2014  Milan Romic */

#ifndef AVR_UTILS_H_
#define AVR_UTILS_H_

unsigned char getLOWbyte(unsigned int input);
unsigned char getHIGHbyte(unsigned int input);
unsigned int combineBytes(unsigned char high, unsigned char low);

#endif