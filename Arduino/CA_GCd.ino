// Declaration of variables
const int ArraySize = 20; // length of vector
float Counts1[ArraySize]; // vector for counting statistics
float Counts2[ArraySize];
int Index;
unsigned long Signal1;
unsigned long Sum1;
unsigned long Signal2;
unsigned long Sum2;
float CPM1[ArraySize];
float CPM1avg;
float CPM2[ArraySize];
float CPM2avg;
float CPMavg;
float dose;
int range;
bool State1;
bool State2;
unsigned long AnalogVal;
double Voltage;
unsigned long PassedTime1;
unsigned long PassedTime2;

//Change this according to board
#define softwareVersion "1.0"
#define _serialNumber "GCdv010n001"
#define _productType "GeigerBox"
#define _mcuFamily "Arduino Nano Every"

#include "readEEPROM.h"

#define OUT_PERIOD 1000 // Timeintervall between serial data output
#define LOG_PERIOD 3000 // Integration time for counts ms

void impulse1() { // 1 Click is 1 change of signal flank
  Signal1++;
  State1 = !State1;
}

void impulse2() {
  Signal2++;
  State2 = !State2;
}

void setup() {

  // Clear variables
  Signal1 = 0;
  Signal2 = 0;
  State1 = LOW;
  State2 = LOW;
  Sum1 = 0;
  Sum2 = 0;
  CPM1avg = 0;
  CPM2avg = 0;
  PassedTime1 = 0;
  PassedTime2 = 0;
  Index = 0;

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
  for (int i = 0; i < ArraySize; i++) {
    Counts1[i] = 0;
    Counts2[i] = 0;
    CPM1[i] = 0;
    CPM2[i] = 0;
  }
}

void loop() {

  // Buzzer sound
  if (digitalRead(4) == LOW) {
    digitalWrite(8, LOW);
  }
  else if (digitalRead(5) == LOW) {
    digitalWrite(8, State1);
  }
  else if (digitalRead(6) == LOW) {
    digitalWrite(8, State2);
  }
  else if (digitalRead(7) == LOW) {
    digitalWrite(8, State1 or State2);
  }

  // Timer
  unsigned long CurrentTime1 = millis();
  unsigned long CurrentTime2 = millis();

  // Data output via serial port
  if (CurrentTime1 - PassedTime1 > OUT_PERIOD){
    PassedTime1 = CurrentTime1;
    // Voltage divider multiplier found by measuring
    AnalogVal = analogRead(A0);
    Voltage = ((float)AnalogVal/1024*5*100);    
    Serial.print(Voltage);
    Serial.print(",  ");
    Serial.print(CPM1avg);
    Serial.print(",  ");
    Serial.print(CPM2avg);
    Serial.print(",  ");
    Serial.print(CPMavg);
//    Serial.print(",  ");
//    Serial.print(range);
    Serial.print(",  ");
    Serial.print(dose);
    Serial.println(",  ");
  }

  // Data analysis
  if (CurrentTime2 - PassedTime2 > LOG_PERIOD) {
    PassedTime2 = CurrentTime2;
    Counts1[Index] = Signal1/2; // Divide by two as Attachinterrupt uses CHANGE of state 
    Counts2[Index] = Signal2/2;
    CPM1[Index] = Counts1[Index]*20;
    CPM2[Index] = Counts2[Index]*20;

    // Set back state to avoid buzzer noise 
    State1 = LOW;
    State2 = LOW;
    
    for (int j = 0; j < ArraySize; j++) {
      Sum1 = Sum1 + CPM1[j];
      Sum2 = Sum2 + CPM2[j];
    }

    CPM1avg = (float)Sum1/(float)ArraySize;  
    CPM2avg = (float)Sum2/(float)ArraySize;
    CPMavg = (CPM1avg + CPM2avg)/2;

    // Calculate dose rate in µSv/h from CPM with adapted conversion factors    
    if (CPMavg <= 54){
      dose = CPMavg/181.567;
      range = 1;
      }
    else if (CPMavg <= 142 && CPMavg > 54){
      dose = (CPMavg - 54)/124.385 + 0.3;
      range = 2;
      }
    else if (CPMavg <= 379 && CPMavg > 142){
      dose = (CPMavg - 142)/118.518 + 1;
      range = 3;
      }
    else if (CPMavg <= 1199 && CPMavg > 379){
      dose = (CPMavg - 379)/117.266 + 3;
      range = 4;
      }
    else if (CPMavg <= 3284 && CPMavg > 1199){
      dose = (CPMavg - 1199)/104.246 + 10;
      range = 5;
      }      
    else if (CPMavg <= 9169 && CPMavg > 3284){
      dose = (CPMavg - 3284)/84.067 + 30;
      range = 6;
      }
    else if (CPMavg <= 19409 && CPMavg > 9169){
      dose = (CPMavg - 9169)/51.200 + 100;
      range = 7;
      }
    else if (CPMavg <= 31829 && CPMavg > 19409){
      dose = (CPMavg - 19409)/17.743 + 300;
      range = 8;
      }
    else if (CPMavg > 31829){
      dose = (CPMavg - 31829)/17.743 + 1000;
      range = 9;
      }

    // Set back values to zero for new loop
    Sum1 = 0;
    Sum2 = 0;
    Signal1 = 0;
    Signal2 = 0;
    Index = Index + 1;

    if(Index >= ArraySize) {
      Index = 0;
    }      
  }
   
}
