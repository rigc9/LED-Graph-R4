#include "Arduino_LED_Matrix.h"
ArduinoLEDMatrix matrix;

//Define if use analogread
#define AR
int analogPin = A0;

// These variables can be controlled by Serial commands
int DisplayPeriod = 500;
bool RtoL = false;
bool BarGraph = true;
float Gain = 1;
float Offset = 0;

const int max_bit = 14;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Available command: RtoL, BarGraph, Gain <value>, Offs <value>, DisP <value>");
  matrix.begin();

  #ifdef AR
  analogReadResolution(max_bit);
  #endif
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

unsigned long start = millis();
unsigned long average = 0;
unsigned int count = 0;

const int max_val = pow(2, max_bit) - 1;
void loop() {

  // value update (replace here for AnalogInput etc)
  #ifdef AR
    unsigned long value = analogRead(analogPin);    // アナログピンを読み取る
    delay(1); //sampling delay
  #else
    unsigned long value = random(1, max_val);
  #endif

  // update average value
  average = average + value;
  count++;

  // display
  if (millis() - start > DisplayPeriod) {
    // new value to plot
    int new_value = Gain * 8 * ((float)average/(float)count - Offset)/(max_val-1) ;
    //Serial.println(String(value) + " @ " + String(new_value)+ "  " +String((float)average/(float)count) + " , " +String(average)+"/" + String(count));
    // change to binary
    byte nv_bin;
    if (BarGraph) {
      nv_bin = pow(2, new_value) - 1;
    } else {
      nv_bin = 1 << (new_value - 1);
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
    average = 0;
    count = 0;
    start = millis();
  }
  


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
