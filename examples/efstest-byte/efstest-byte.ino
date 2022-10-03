#include <EepromFS.h>
#include <Wire.h>

EepromFS EFS(0x50, 32768);

void setup() {

  Serial.begin(9600);

  // EepromFS does not start Wire!
  Wire.begin();

  // give Wire and Serial a little time
  delay(1000);

  // mount or format
  if (!EFS.begin()) EFS.format(32);

  // allocate a file slot and call it dummy 
  // close it right away but remember the slot number
  uint8_t s=EFS.fopen("dummy", "w");
  EFS.fclose(s); 
  Serial.print("Slot number is "); 
  Serial.println(s);

  // how big is the slot
  Serial.print("Data slot size is :"); 
  Serial.println(EFS.size());

  // access it through the byte API
  for(int i=0; i<10; i++) EFS.putdata(s, i, i*i);
  EFS.rawflush();
  for(int i=0; i<10; i++) Serial.println(EFS.getdata(s, i));
}
  
  
void loop() {}
