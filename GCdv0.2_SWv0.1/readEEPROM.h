
#include <EEPROM.h>

#ifndef _serialNumber
  #define _serialNumber "NA"
#endif
#ifndef _productType
  #define _productType "NA"
#endif
#ifndef _mcuFamily
  #define _mcuFamily "NA"
#endif

int address = 0;
int Length = 100;

String compileDate = __DATE__  " " __TIME__;

void printSerial()
{
  uint8_t localValue[Length]; // constructs byte array length of string

  for (int i = address; i <= Length + 1; i++) 
  { 
    localValue[i] =  EEPROM.read(i);
  }

  String eepromString = String((char*)localValue);
  int pos1 = eepromString.indexOf(';');
  int pos2 = eepromString.indexOf(';', pos1+1);
  int pos3 = eepromString.indexOf(';', pos2+1);
  int pos4 = eepromString.indexOf(';', pos3+1);
  int pos5 = eepromString.indexOf(';', pos4+1);

  String serialNumber = pos1 > 0?eepromString.substring(0, pos1):_serialNumber;
  String productType = pos2 > pos1?eepromString.substring(pos1+1, pos2):_productType;
  String pcbVersion = eepromString.substring(pos2+1, pos3);
  String eepromBurnDate = eepromString.substring(pos3+1, pos4);
  String mcuFamily = pos5 > pos4?eepromString.substring(pos4+1, pos5):_mcuFamily;
  
  Serial.println("");
  Serial.print("Serial Number: ");
  Serial.println(serialNumber);
  Serial.print("Product Type: ");
  Serial.println(productType);
  Serial.print("Software Version: ");
  Serial.println(softwareVersion);
  Serial.print("Software Compile Date: ");
  Serial.println(compileDate);
  Serial.print("MCU Family: ");
  Serial.println(mcuFamily);
  Serial.print("PCB version: ");
  Serial.println(pcbVersion);
}
