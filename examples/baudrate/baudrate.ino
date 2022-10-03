#include <EepromFS.h>
#include <Wire.h>

EepromFS EFS(0x50, 32768);

void setup() {

  Serial.begin(9600);

  // EepromFS does not start Wire!
  Wire.begin();

  // give Wire and Serial a little time
  delay(1000);
  
  // begin the filesystem - return value is the number of slots in the FS
  int s;
  if (s=EFS.begin()) {
    Serial.println("Filessystem mounted");
    Serial.print(s); Serial.println(" slots in fs");

    int f=EFS.fopen("test", "w");
    for (int i=0; i<256; i++) EFS.fputc(i, f);
    EFS.fclose(f);

    f=EFS.fopen("test", "r");
    long t=millis();
    for (int i=0; i<256; i++) EFS.fgetc(f);
    t=millis()-t;
    EFS.fclose(f);

    Serial.print("Read baudrate : "); Serial.println(256*8/t*1000);
  } else {
    Serial.println("No filesystem found");
  }
}

void loop() {}
