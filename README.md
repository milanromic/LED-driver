# LED-driver
*Slave LED driver controlled by MODBUS RTU protocol*

### Hardware unit
MCU: Atmel AtMega8<br />
XTAL: 14.7456 MHz<br />
USART Baud Rate: 9600<br />
Communication Parameters: 8 Data, 1 Stop, No Parity<br />

ModbusID - address of unit

`void SetREG(unsigned int index, unsigned int val)` - change register value<br />
`unsigned int GetREG(unsigned int index)` - return value of register

`SetREG(11, 0);` - set RED light intensity (PWM intensity)<br />
`SetREG(12, 0);` - set GREEN light intensity (PWM intensity)<br />
`SetREG(13, 0);` - set BLUE light intensity (PWM intensity)<br />
`SetREG(14, 0);` - set WHITE light intensity (PWM intensity)<br />
*Range 0 - 200*

Set the output (acceleration / deceleration of intensity change is included)<br />
`SetRedDutyWish(GetREG(11));`<br />
`SetGreenDutyWish(GetREG(12));`<br />
`SetBlueDutyWish(GetREG(13));`<br />
`SetWhiteDutyWish(GetREG(14));`<br />
