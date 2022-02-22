#include <EepromFS.h>
#include <Wire.h>

EepromFS EFS(0x50, 32768);

void debug() {
  Serial.print("Debug: "); Serial.println(EFS.debug);
}

void dump() {
  for(int i=0; i<99; i++) {
    if (i%8 == 0) Serial.println();
    Serial.print((int) EFS.rawread(i)); Serial.write(" "); 
  }
  
  Serial.println();
}

void setup() {

  Serial.begin(9600);

  // EepromFS does not start wire.
  Wire.begin();
  delay(1000);
  Serial.println("Basic EEPROM test ");

  // write three pages
  for (int i=0; i<60; i++) EFS.rawwrite(i, i);

  // always flush when using raw to flush the last 
  // page
  EFS.rawflush();

  // eeprom function test
  dump();

  // read three pages
  for (int i=0; i<60; i++) {
     Serial.print(i); Serial.print(" : "); Serial.println(EFS.rawread(i));
  }

  // check if there is an error state, should output 0
  debug();
}

void loop() {
  // put your main code here, to run repeatedly:

}
