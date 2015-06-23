/* Copyright (C) 2014  Milan Romic */

#include "avr_modbus.h"
#include "avr_utils.h"
#include "hardware_mega8.h"


/* Get LOW byte of CRC */
unsigned char getCRClow(unsigned char * data, unsigned char length)
{
	unsigned char crc_low = generateCRC(data, length) & 0xFF;
	return crc_low;
}

/* Get HIGH byte of CRC */
unsigned char getCRChigh(unsigned char * data, unsigned char length)
{
	unsigned char crc_high = (int)(generateCRC(data, length) >> 8) & 0xFF;
	return crc_high;
}

/* Check CRC */
char checkCRC(unsigned char * data, unsigned char length)
{
	unsigned char Crc_HByte, Crc_LByte;
	unsigned int Crc;
	
	Crc=0xFFFF;
	for (int i = 0; i < length - 2; i++) {
		Crc = CRC16 (Crc, data[i] );
	}
	Crc_LByte = (Crc & 0x00FF); // Low byte calculation
	Crc_HByte = (Crc & 0xFF00) / 256; // High byte calculation
	
	if(data[length - 2] == Crc_LByte && data[length - 1] == Crc_HByte)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/* Generate CRC */
unsigned int generateCRC(unsigned char * data, unsigned char length)
{
	unsigned int crc = 0xFFFF;
	for (int i = 0; i < length; i++) {
		crc = CRC16 (crc, data[i] );
	}
	
	return crc;
}

/* CRC generating algorithm */
unsigned int CRC16(unsigned int crc, unsigned int data)
{
	const unsigned int Poly16=0xA001;
	unsigned int LSB, i;
	crc = ((crc^data) | 0xFF00) & (crc | 0x00FF);
	for (i=0; i<8; i++) {
		LSB=(crc & 0x0001);
		crc=crc/2;
		if (LSB)
		crc=crc^Poly16;
	}
	return(crc);
}

char checkException(unsigned char * data)
{
	/* Start register value */
	unsigned int start_register = combineBytes(data[2], data[3]);
	/* Quantity of registers value */
	unsigned int quantity = combineBytes(data[4], data[5]);
	
	/* Selection by function code */
	switch(data[1])
	{
		/**** Function Code 01 - Read Digital Output Status ****/
		case 1:
		if(start_register + quantity - 1 < 16)
		{
			return 0x00;
		}
		else
		{
			/* Illegal Data Address */
			return 0x02;
		}
		break;
		/**** Function Code 02 - Read Digital Input Status ****/
		case 2:
		if(start_register + quantity - 1 < 4)
		{
			return 0x00;
		}
		else
		{
			/* Illegal Data Address */
			return 0x02;
		}
		break;
		/**** Function Code 03 - Read Holding Registers ****/
		case 3:
		if(start_register + quantity - 1 < 20)
		{
			return 0x00;
		}
		else
		{
			/* Illegal Data Address */
			return 0x02;
		}
		break;
		/**** Function Code 06 - Preset Single Register ****/
		case 6:
		if(start_register < 20)
		{
			return 0x00;
		}
		else
		{
			/* Illegal Data Address */
			return 0x02;
		}
		break;
		/**** Function Code 16 - Preset Multiple Registers ****/
		case 16:
		if(start_register + quantity - 1 < 20)
		{
			return 0x00;
		}
		else
		{
			/* Illegal Data Address */
			return 0x02;
		}
		break;
	}
	return 0;
}

void prepareOutputArray(unsigned char * data)
{
	char exception = checkException(data);
	
	/* Temporary counters */
	int temp_cnt_bits = 0;
	int temp_cnt_reg = 0;
	int address_byte = 0;
	int start_byte = 0, length_byte = 0;
	
	if(exception == 0x00)
	{
		/* Exception doesn't exist */
		unsigned char reg_quantity;
		switch(data[1])
		{
			/**** Function Code 01 - Read Digital Output Status ****/
			case 1:
			tx_buffer_m[0] = data[0];
			tx_buffer_m[1] = data[1];
			/* Number of output registers related to requested registers */
			reg_quantity = 1 + ((data[5] - 1) / 8);
			tx_buffer_m[2] = reg_quantity;
			/* Temp register */
			temp_cnt_bits = data[3];
			temp_cnt_reg = (data[3] / 8);
			/* Reset registers */
			for(int i = 0; i < reg_quantity; i++)
			{
				tx_buffer_m[3 + i] = 0;
			}
			/* Populate register */
			for(int k = 0; k < data[5]; k++)
			{
				while(!(temp_cnt_bits < 8))
				{
					temp_cnt_bits -= 8;
					temp_cnt_reg++;
				}
				/* Fill output registers */
				if((OUT_COIL[temp_cnt_reg] & (1 << temp_cnt_bits)) != 0)
				{
					tx_buffer_m[3 + (k / 8)] += 1 << (((k / 8) * 8) + k);
				}
				temp_cnt_bits++;
			}
			/* Prepare CRC at the end */
			tx_buffer_m[reg_quantity + 3] = getCRClow(tx_buffer_m, reg_quantity + 3);
			tx_buffer_m[reg_quantity + 4] = getCRChigh(tx_buffer_m, reg_quantity + 3);
			/* Send array to output */
			for(int i = 0; i < reg_quantity + 5; i++)
			{
				PutChar(tx_buffer_m[i]);
			}
			break;

			/**** Function Code 02 - Read Digital Input Status ****/
			case 2:
			tx_buffer_m[0] = data[0];
			tx_buffer_m[1] = data[1];
			/* Number of output registers related to requested registers */
			reg_quantity = 1 + ((data[5] - 1) / 8);
			tx_buffer_m[2] = reg_quantity;
			/* Temp register */
			temp_cnt_bits = data[3];
			temp_cnt_reg = (data[3] / 8);
			/* Reset registers */
			for(int i = 0; i < reg_quantity; i++)
			{
				tx_buffer_m[3 + i] = 0;
			}
			/* Populate register */
			for(int k = 0; k < data[5]; k++)
			{
				while(!(temp_cnt_bits < 8))
				{
					temp_cnt_bits -= 8;
					temp_cnt_reg++;
				}
				/* Fill output registers */
				if((IN_COIL[temp_cnt_reg] & (1 << temp_cnt_bits)) != 0)
				{
					tx_buffer_m[3 + (k / 8)] += 1 << (((k / 8) * 8) + k);
				}
				temp_cnt_bits++;
			}
			/* Prepare CRC at the end */
			tx_buffer_m[reg_quantity + 3] = getCRClow(tx_buffer_m, reg_quantity + 3);
			tx_buffer_m[reg_quantity + 4] = getCRChigh(tx_buffer_m, reg_quantity + 3);
			/* Send array to output */
			for(int i = 0; i < reg_quantity + 5; i++)
			{
				PutChar(tx_buffer_m[i]);
			}
			break;
			
			/**** Function Code 03 - Read Holding Registers ****/
			case 3:
			tx_buffer_m[0] = data[0];
			tx_buffer_m[1] = data[1];
			/* Number of output registers related to requested registers */
			tx_buffer_m[2] = data[5] * 2;
			/* Temp register */
			temp_cnt_bits = 0;
			temp_cnt_reg = data[3];
			/* Populate register */
			for(int k = 0; k < data[5]; k++)
			{
				tx_buffer_m[3 + temp_cnt_bits] = getHIGHbyte(REG[data[3] + k]);
				temp_cnt_bits++;
				tx_buffer_m[3 + temp_cnt_bits] = getLOWbyte(REG[data[3] + k]);
				temp_cnt_bits++;
				
			}
			/* Prepare CRC at the end */
			tx_buffer_m[tx_buffer_m[2] + 3] = getCRClow(tx_buffer_m, tx_buffer_m[2] + 3);
			tx_buffer_m[tx_buffer_m[2] + 4] = getCRChigh(tx_buffer_m, tx_buffer_m[2] + 3);
			/* Send array to output */
			for(int i = 0; i < tx_buffer_m[2] + 5; i++)
			{
				PutChar(tx_buffer_m[i]);
			}
			break;
			
			/**** Function Code 06 - Preset Single Register ****/
			case 6:
			tx_buffer_m[0] = data[0];
			tx_buffer_m[1] = data[1];
			tx_buffer_m[2] = data[2];
			tx_buffer_m[3] = data[3];
			tx_buffer_m[4] = data[4];
			tx_buffer_m[5] = data[5];

			/* Store value in internal register */
			address_byte = combineBytes(data[2], data[3]);
			REG[address_byte] = combineBytes(data[4], data[5]);

			/* Prepare CRC at the end */
			tx_buffer_m[6] = getCRClow(tx_buffer_m, 6);
			tx_buffer_m[7] = getCRChigh(tx_buffer_m, 6);
			
			/* Send array to output */
			for(int i = 0; i < 8; i++)
			{
				PutChar(tx_buffer_m[i]);
			}
			break;
			
			/**** Function Code 16 - Preset Multiple Registers ****/
			case 16:
			tx_buffer_m[0] = data[0];
			tx_buffer_m[1] = data[1];
			tx_buffer_m[2] = data[2];
			tx_buffer_m[3] = data[3];
			tx_buffer_m[4] = data[4];
			tx_buffer_m[5] = data[5];

			/* Store value in internal registers */
			start_byte = combineBytes(data[2], data[3]);
			length_byte = combineBytes(data[4], data[5]);
			
			int incremental = 0;
			for(int j = start_byte; j < (long)(start_byte + length_byte); j++)
			{
				REG[j] = combineBytes(data[7 + incremental], data[8 + incremental]);
				incremental += 2;
			}

			/* Prepare CRC at the end */
			tx_buffer_m[6] = getCRClow(tx_buffer_m, 6);
			tx_buffer_m[7] = getCRChigh(tx_buffer_m, 6);
			
			/* Send array to output */
			for(int i = 0; i < 8; i++)
			{
				PutChar(tx_buffer_m[i]);
			}
			break;
		}
	}
	else
	{
		/* Exception exist */
		tx_buffer_m[0] = (data[1] + 0x80);
		tx_buffer_m[1] = exception;
		tx_buffer_m[2] = getCRClow(tx_buffer_m, 2);
		tx_buffer_m[3] = getCRChigh(tx_buffer_m, 2);
		
		for(int i = 0; i < 4; i++)
		{
			PutChar(tx_buffer_m[i]);
		}
	}
}

unsigned int GetREG(unsigned int index)
{
	return REG[index];
}

void SetREG(unsigned int index, unsigned int val)
{
	REG[index] = val;
}

void UpdateDigitalOutputs()
{
	/* RED: OUT_COIL[0] -> 0b00000001 */
	if(red_duty > 0)
		OUT_COIL[0] |= 0b00000001;
	else
		OUT_COIL[0] &= 0b11111110;
	/* GREEN: OUT_COIL[0] -> 0b00000010 */
	if(green_duty > 0)
		OUT_COIL[0] |= 0b00000010;
	else
		OUT_COIL[0] &= 0b11111101;
	/* BLUE: OUT_COIL[0] -> 0b00000100 */
	if(blue_duty > 0)
		OUT_COIL[0] |= 0b00000100;
	else
		OUT_COIL[0] &= 0b11111011;
	/* WHITE: OUT_COIL[0] -> 0b00001000 */
	if(white_duty > 0)
		OUT_COIL[0] |= 0b00001000;
	else
		OUT_COIL[0] &= 0b11110111;
	/* LED 1: IN_COIL[0] -> 0b00010000 */
	if((PINB & 0b00000001) == 0b00000001)
	{
		OUT_COIL[0] |= 0b00010000;
	}
	else
	{
		OUT_COIL[0] &= 0b11101111;
	}
	/* LED 2: IN_COIL[0] -> 0b00100000 */
	if((PIND & 0b10000000) == 0b10000000)
	{
		OUT_COIL[0] |= 0b00100000;
	}
	else
	{
		OUT_COIL[0] &= 0b11011111;
	}
}

void UpdateDigitalInputs()
{
	/* TASTER 1: IN_COIL[0] -> 0b00000001 */
	if((PINB & 0b00000010) == 0b00000010)
	{
		IN_COIL[0] |= 0b00000001;
	}
	else
	{
		IN_COIL[0] &= 0b11111110;
	}
	/* TASTER 1: IN_COIL[0] -> 0b00000010 */
	if((PINB & 0b00000100) == 0b00000100)
	{
		IN_COIL[0] |= 0b00000010;
	}
	else
	{
		IN_COIL[0] &= 0b11111101;
	}
	/* INPUT 1: IN_COIL[0] -> 0b00000100 */
	if((PIND & 0b00000100) == 0b00000100)
	{
		IN_COIL[0] |= 0b00000100;
	}
	else
	{
		IN_COIL[0] &= 0b11111011;
	}
	/* INPUT 1: IN_COIL[0] -> 0b00001000 */
	if((PIND & 0b00001000) == 0b00001000)
	{
		IN_COIL[0] |= 0b00001000;
	}
	else
	{
		IN_COIL[0] &= 0b11110111;
	}
}