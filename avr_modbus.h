/* Copyright (C) 2014  Milan Romic */

#ifndef AVR_MODBUS_H
#define AVR_MODBUS_H

unsigned char tx_buffer_m[64];

unsigned int
REG[20];

unsigned char
OUT_COIL[8],
IN_COIL[8];

/* Get LOW byte of CRC */
unsigned char getCRClow(unsigned char * data, unsigned char length);
/* Get HIGH byte of CRC */
unsigned char getCRChigh(unsigned char * data, unsigned char length);
/* Check CRC */
char checkCRC(unsigned char * data, unsigned char length);
/* Generate CRC */
unsigned int generateCRC(unsigned char * data, unsigned char length);
/* CRC generating algorithm */
unsigned int CRC16(unsigned int crc, unsigned int data);
/* Check if there is exception in request */
char checkException(unsigned char * data);
/* Prepare data to be send via TX */
void prepareOutputArray(unsigned char * data);
/* Get REG value */
unsigned int GetREG(unsigned int index);
/* Get REG value */
void SetREG(unsigned int index, unsigned int val);
/* Update digital outputs */
void UpdateDigitalOutputs();
/* Update digital inputs */
void UpdateDigitalInputs();
#endif
