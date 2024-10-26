# LED-Graph-R4
Simple graphing code for LED matrix in Arduino Uno R4

## Variables:
- int DisplayPeriod = 500;
update period

- bool RtoL = false;
flow direction

- bool BarGraph = true;
fill or not fill

- float Gain = 1;
Gain for converting value to 8 bit value in the LED matrix

- float Offset = 0;
Offset for converting value to 8 bit value in the LED matrix

LED 8 bit value = Gain * (value - offset)
