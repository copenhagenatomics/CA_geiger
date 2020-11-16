// Declaration of variables
const int numCounts = 60; // length of vector
float counts1[numCounts]; // vector for counting statistics
float counts2[numCounts];
int index;
unsigned long clicks1;
unsigned long max1;
unsigned long min1;
unsigned long sum1;
unsigned long clicks2;
unsigned long max2;
unsigned long min2;
unsigned long sum2;
float CPM1;
float CPM1_array[numCounts];
float avg1;
float CPM2;
float CPM2_array[numCounts];
float avg2;
bool state1;
bool state2;
unsigned long analog;
double voltage;
unsigned long passedtime;

//Change this according to board
#define softwareVersion "0.1"
#define _serialNumber "gcdv02007"
#define _productType "Geiger Counter double v0.2"
#define _mcuFamily "Arduino Nano Every"

#include "readEEPROM.h"

#define LOG_PERIOD 1000 // Time frame in ms for clicks


void impulse1() {
  clicks1++;
  state1 = !state1;
}


void impulse2() {
  clicks2++;
  state2 = !state2;
}


void setup() {

  // Clear variables
  clicks1 = 0;
  clicks2 = 0;
  state1 = LOW;
  state2 = LOW;
  sum1 = 0;
  sum2 = 0;
  CPM1 = 0;
  CPM2 = 0;
  passedtime = 0;
  index = 0;

  // Connect serial & pin configuration 
  Serial.begin(115200);
  
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), impulse1, CHANGE);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(3), impulse2, CHANGE);

  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, OUTPUT);
  
  printSerial();
  
  //clear countvector
  for (int i = 0; i < numCounts; i++) {
    counts1[i] = 0;
    counts2[i] = 0;
  }
}

void loop() {

  // Buzzer sound
  if (digitalRead(4) == LOW) {
    digitalWrite(8, LOW);
  }
  else if (digitalRead(5) == LOW) {
    digitalWrite(8, state1);
  }
  else if (digitalRead(6) == LOW) {
    digitalWrite(8, state2);
  }
  else if (digitalRead(7) == LOW) {
    digitalWrite(8, state1 or state2);
  }
  
  unsigned long currenttime = millis();

  if (currenttime - passedtime > LOG_PERIOD) {
    passedtime = currenttime;
    counts1[index] = clicks1/2; // Divide by two as Attachinterrupt uses CHANGE of state 
    counts2[index] = clicks2/2;

    // Voltage divider multiplier found by measuring
    analog = analogRead(A0);
    voltage = ((float)analog/1024*5*113);
    
    for (int j = 0; j < numCounts; j++) {
      sum1 = sum1 + counts1[j];
      sum2 = sum2 + counts2[j];
    }

    CPM1 = (float)sum1/(float)numCounts*60;
    CPM1_array[index] = CPM1;
    for (int k = 0; k < (numCounts - 1); k++) {
      if (CPM1_array[k+1] > CPM1_array[k]) {
        max1 = CPM1_array[k+1];
      }
    }

    for (int l = 0; l < (numCounts - 1); l++) {
      if (CPM1_array[l+1] < CPM1_array[l]) {
        min1 = CPM1_array[l+1];
      }
    }
    
    avg1 = ((float)max1 + (float)min1)/2;
    
    CPM2 = (float)sum2/(float)numCounts*60;
    CPM2_array[index] = CPM2;
    for(int k = 0; k < (numCounts - 1); k++) {
      if (CPM2_array[k+1] > CPM2_array[k]) {
        max2 = CPM2_array[k+1];
      }
    }

    for (int l = 0; l < (numCounts - 1); l++) {
      if (CPM2_array[l+1] < CPM2_array[l]) {
        min2 = CPM2_array[l+1];
      }
    }
    
    avg2 = ((float)max2 + (float)min2)/2;

    // Serial output
    Serial.print(voltage);
    Serial.print(",  ");
    Serial.print(counts1[index]);
    Serial.print(",  ");
    Serial.print(CPM1);
    Serial.print(",  ");
    Serial.print(avg1);
    Serial.print(",  ");
    Serial.print(counts2[index]);
    Serial.print(",  ");
    Serial.print(CPM2);
    Serial.print(",  ");
    Serial.print(avg2);
    Serial.println(",  ");
//    Serial.print(-10);
//    Serial.print(",  ");
//    Serial.print(-10);
//    Serial.print(",  ");
//    Serial.print(-10);
//    Serial.print(",  ");
//    Serial.print(-10);
//    Serial.println(",  ");
    
    sum1 = 0;
    sum2 = 0;
    clicks1 = 0;
    clicks2 = 0;
    index = index + 1;

    if (index >= numCounts) {
      index = 0;
    }      
  }
}
