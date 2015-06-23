# LED-driver
*Slave LED driver controlled by MODBUS RTU protocol*

### Hardware unit
MCU: Atmel AtMega8 with 14.7456 MHz

**ModbusID** - address of unit

**SetREG** - change register value

For example:

`SetREG(11, 0);` - set RED light intensity (PWM intensity)

`SetREG(12, 0);` - set GREEN light intensity (PWM intensity)

`SetREG(13, 0);` - set BLUE light intensity (PWM intensity)

`SetREG(14, 0);` - set WHITE light intensity (PWM intensity)

