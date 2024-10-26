#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;

// These variables can be controlled by Serial commands
int DisplayPeriod = 500;
bool RtoL = false;
bool BarGraph = true;
float Gain = 1;
float Offset = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Available command: RtoL, BarGraph, Gain <value>, Offs <value>, DisP <value>");
  matrix.begin();
}

void byte_to_bit_array(unsigned char byte_value, int bit_array[8]) {
  for (int i = 0; i < 8; i++) {
    bit_array[7 - i] = (byte_value >> i) & 1;
  }
}

unsigned long frame[] = {
  0x3184a444,
  0x42081100,
  0xa0040000
};

void loop() {
  // new value to plot
  int value = Gain * (random(1, 9) - Offset);

  // change to binary
  byte nv_bin;
  if (BarGraph) {
    nv_bin = pow(2, value) - 1;
  } else {
    nv_bin = 1 << (value - 1);
  }
  // decompose into array
  int nv[8];
  byte_to_bit_array(nv_bin, nv);

  // calculate frame
  unsigned long b1 = frame[0];
  unsigned long b2 = frame[1];
  unsigned long b3 = frame[2];

  if (RtoL) {
    frame[0] = (b1 << 1 & 0xffeffeff) | (b2 >> 31 & 1) | nv[0] << 20 | nv[1] << 8;
    frame[1] = (b2 << 1 & 0xeffeffef) | (b3 >> 31 & 1) | nv[2] << 28 | nv[3] << 16 | nv[4] << 4;
    frame[2] = (b3 << 1 & 0xfeffeffe) | nv[5] << 24 | nv[6] << 12 | nv[7] << 0;
  } else {
    frame[0] = (b1 >> 1 & 0x7ff7ff7f) | nv[0] << 31 | nv[1] << 19 | nv[2] << 7;
    frame[1] = (b2 >> 1 & 0xf7ff7ff7) | (b1 & 1) << 31 | nv[3] << 27 | nv[4] << 15 | nv[5] << 3;
    frame[2] = (b3 >> 1 & 0xff7ff7ff) | (b2 & 1) << 31 | nv[6] << 23 | nv[7] << 11;
  }

  // update
  matrix.loadFrame(frame);
  delay(DisplayPeriod);

  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');

    if (command.equalsIgnoreCase("RtoL")) {
      RtoL = !RtoL;
      Serial.println("RtoL is now: " + String(RtoL));
    } else if (command.equalsIgnoreCase("BarGraph")) {
      BarGraph = !BarGraph;
      Serial.println("BarGraph is now: " + String(BarGraph));
    } else if (command.startsWith("Gain ")) {
      Gain = command.substring(5).toFloat();
      Serial.println("Gain is now set to: " + String(Gain));
    } else if (command.startsWith("Offs ")) {
      Offset = command.substring(5).toFloat();
      Serial.println("Offset is now set to: " + String(Offset));
    } else if (command.startsWith("DisP ")) {
      DisplayPeriod = command.substring(5).toInt();
      Serial.println("DisplayPeriod is now set to: " + String(DisplayPeriod));
    } else {
      Serial.println("Unknown command");
    }
  }
}
