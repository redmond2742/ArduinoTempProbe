/*
Resources
Code: OneWire latest Version: http://playground.arduino.cc/Learning/OneWire
Schematic: https://tushev.org/articles/arduino/10

Todo
-Check code to make sure all printlines work and temp sensor works
-Have input values determine max and min temps
-Print out motor results based on temp
-Show estimated time to change.
*/

#include <OneWire.h>

// DS18S20 Temperature chip i/o
OneWire ds(10);  // on pin 10

void setup(void) {
  // initialize inputs/outputs
  // start serial port
  Serial.begin(9600);
}

void loop(void) {
  //Setup Variables, Temp changed to floats for including decimals
  int HighByte, LowByte, TReading, SignBit, Whole, Fract;
  float Tc_100, Tf_100;
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];

  ds.reset_search();
  if ( !ds.search(addr)) {
      Serial.print("No more addresses.\n");
      ds.reset_search();
      return;
  }

  Serial.print("R=");
  for( i = 0; i < 8; i++) {
    Serial.print(addr[i], HEX);
    Serial.print(" ");
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n");
      return;
  }

  if ( addr[0] == 0x10) {
      Serial.print("Device is a DS18S20 family device.\n");
  }
  else if ( addr[0] == 0x28) {
      Serial.print("Device is a DS18B20 family device.\n");
  }
  else {
      Serial.print("Device family is not recognized: 0x");
      Serial.println(addr[0],HEX);
      return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end

  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.

 present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad
 /*
  Serial.print("P=");
  Serial.print(present,HEX);
  Serial.print(" "); */
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    //Serial.print(data[i], HEX);
    //Serial.print(" ");
  }
  /*
  Serial.print(" CRC=");
  Serial.print( OneWire::crc8( data, 8), HEX);
  Serial.println(); */
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  
  //Read Value for Temp Sensor
  Tc_100 = ((6 * TReading) + TReading / 4)/100.0;    // multiply by (100 * 0.0625) or 6.25 and then convert to F with C*9/5 +32 
  //Convert Celcius to Farhenheight
  Tf_100=(((Tc_100)*(9.0/5.0))+32);
  //Whole = Tf_100 / 100;  // separate off the whole and fractional portions
  //Fract = Tf_100 % 100;

  
//Whole = (Whole* 9/5) +32;
  Serial.print("C:");
  if (SignBit) // If its negative
  {
     Serial.print("-");
  }
  
  Serial.println(Tc_100);
  Serial.print("F:");
   if (SignBit) // If its negative
  {
     Serial.print("-");
  }
  Serial.println(Tf_100);
  
  /******PsudoCode to Keep Burner on or Off
  
Float User_Input_Value_Min, User_Input_Value_Max
Ask user input for temp.
 //If Tf < User_Input_Value_Min
 Max Flame Power (server motor calibrated)
 If Tf>User_Input_Value_Max
 No Flame Power (server motor calibrated)
  
}

