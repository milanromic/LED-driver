/* Copyright (C) 2014  Milan Romic */

#include "hardware_mega8.h"
#include "avr_modbus.h"

unsigned char rx[255];

unsigned int ModbusID, RXindex;

/* Main code */
int main(void)
{
	ModbusID = 22;
	
	init();
	
	SetREG(11, 0);
	SetREG(12, 0);
	SetREG(13, 0);
	SetREG(14, 0);
	
	while(1)
	{
		RXindex = GetRXindex();

		/* Modbus ID address 22 */
		if(RXindex > 7 && GetRXitem(0) == ModbusID)
		{
			BurnRXArray(rx);
			/* Check for function code */
			if(rx[1] != 8 && rx[1] != 16)
			{
				/* Check CRC of request */
				if(checkCRC(rx, RXindex) != 0)
				{
					/* Prepare data for reply */
					prepareOutputArray(rx);
				}
				/* Reset RX counter */
				SetRXindex(0);
			}
			else
			{
				/* If arrived Function Code 16 */
				if(rx[1] == 16 && RXindex >= (9 + rx[6]))
				{
					/* Check CRC of request */
					if(checkCRC(rx, RXindex) != 0)
					{
						PORTB |= 0b00000001;
						/* Prepare data for reply */
						prepareOutputArray(rx);
					}
					/* Reset RX counter */
					SetRXindex(0);
				}
			}
		}
		
		if(RXindex > 0 && GetRXtimeout() >= 600)
		{
			SetRXindex(0);
			SetRXtimeout(1000);
			
		}
/*		else
		{
			PORTB &= 0b11111110;
		}
*/		
		/* Update digital outputs */
		UpdateDigitalOutputs();
		/* Update digital inputs */
		UpdateDigitalInputs();
		/* Update LED outputs */
		SetRedDutyWish(GetREG(11));
		SetGreenDutyWish(GetREG(12));
		SetBlueDutyWish(GetREG(13));
		SetWhiteDutyWish(GetREG(14));
	}
}