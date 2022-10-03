#include <EepromFS.h>
#include <Wire.h>

EepromFS EFS(0x50, 32768);

void formattest() {
  Serial.println("Formating 32 slots");
  long t=millis();
  EFS.format(32);
  t=millis()-t;
  Serial.print("Format time = "); Serial.println(t);
  Serial.print(16*32+6); Serial.println(" bytes updated");
  Serial.print("Update baudrate = "); Serial.print((16*32+6)*8/t); Serial.println("000");

 Serial.print("Error status: "); Serial.println(EFS.ferror);
}

void writetest(char* n, char* s) {
  uint8_t f;
  f=EFS.fopen(n, "w");
  if (f) for(int i=0; i<256 && s[i]!=0; i++) EFS.fputc(s[i], f);
  else Serial.println("Write error");

  EFS.fflush(f);

  Serial.println("Filesize now:");
  Serial.println(EFS.filesize(f));

  Serial.println("Bytes left in slot:");
  Serial.println(EFS.available(f));
  
  EFS.fclose(f);

  Serial.print("Error status: "); Serial.println(EFS.ferror);
}

void readtest(char *n) {
  uint8_t f;
  f=EFS.fopen(n, "r");

  Serial.println("Bytes in file:");  
  Serial.println(EFS.available(f));

  long t=millis();
  if (f) for(int i=0; i<256 && ! EFS.eof(f); i++) Serial.write(EFS.fgetc(f));
  else Serial.println("Read error");
  t=millis()-t;
  Serial.print("Read baud rate: "); Serial.println(11*8/t);
  
  EFS.fclose(f);

  Serial.print("Error status: "); Serial.println(EFS.ferror);
}

void dirtest() {
  EFS.dirp=0;
  while (uint8_t f=EFS.readdir()) {
          Serial.print("Slot "); 
          Serial.print(f);
          Serial.print(": ");
          Serial.print(EFS.filename(f)); 
          Serial.print(" "); 
          Serial.println(EFS.filesize(f));
   } 

   Serial.print("Error status: "); Serial.println(EFS.ferror);
}

void setup() {

  Serial.begin(9600);

  // EepromFS does not start Wire!
  Wire.begin();

  // give Wire and Serial a little time
  delay(1000);

  
  Serial.println("Filesystem test ");

  // initial format - begin doesn't do that 
  Serial.println("-----------------------------------------");
  formattest();
  
  // begin the filesystem - return value is the number of slots in the FS
  Serial.println("-----------------------------------------");
  int s;
  if (s=EFS.begin()) {
    Serial.println("Filessystem mounted");
    Serial.print(s); Serial.println(" slots in fs");
  }
  dirtest();

  Serial.println("-----------------------------------------");
  Serial.println("Write test 1 - file1");
  writetest("file1", "hello world");
  readtest("file1");

  Serial.println("-----------------------------------------");
  Serial.println("Write test 2 - file1");
  writetest("file1", "second write test");
  readtest("file1");
 
  Serial.println("-----------------------------------------");
  Serial.println("Write test 3 - file2");
  writetest("file2", "another file");
  Serial.println("Directory now:");
  dirtest();
  Serial.println("Deleting file2");
  EFS.remove("file2");
  Serial.println("Directory now:");
  dirtest();
  
  Serial.println("-----------------------------------------");

  int file3=EFS.fopen("file3", "w");  
  Serial.println("Opened a third file for write");
  Serial.print("Slot id delivered "); Serial.println(file3);

  for(int i=0; i<26; i++) {
    EFS.fputc(65+i, file3); 
    EFS.fputc(10, file3);
  }
  
  EFS.fclose(file3);

  dirtest();

  int file1=EFS.fopen("file3", "r");

  for (int i=0; !EFS.eof(file1); i++) {
    char ch=EFS.fgetc(file1);
    Serial.write(ch);
  }

  EFS.fclose(file1);

  Serial.println("-----------------------------------------");

  EFS.rename("file3", "file2");
  Serial.println("Renamed a file");
  dirtest();
  Serial.println("-----------------------------------------");
}

void loop() {
  // put your main code here, to run repeatedly:

}
