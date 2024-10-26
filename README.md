# LED-Graph-R4
Simple graphing code for LED matrix in Arduino Uno R4 Wifi
just like a bad oscilloscope.

Compiled on AruduinoIDE 2.3.3 with Arduino Uno Boards 1.2.2

## Demo
![LED BarGraph](https://github.com/user-attachments/assets/19ee3e28-9b5a-45d3-b837-ee8e1e045207)

## Variables:
1. int DisplayPeriod = 500;
  - update period
2. bool RtoL = false;
  - flow direction
3. bool BarGraph = true;
  - fill or not fill
4. float Gain = 1;
  - Gain for converting value to 8 bit value in the LED matrix
5. float Offset = 0;
  - Offset for converting value to 8 bit value in the LED matrix

- LED 8 bit value = Gain * (value - offset)

