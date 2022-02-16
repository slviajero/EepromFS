#include <EepromFS.h>
#include <Wire.h>

EepromFS EFS(0x50, 32768);

void debug() {
  Serial.print("Debug: "); Serial.println(EFS.debug);
}

void setup() {

  Serial.begin(9600);

  // EepromFS does not start Wire!
  Wire.begin();

  // give Wire and Serial a little time
  delay(1000);

  Serial.println("Filesystem test ");

  // initial format - begin doesn't do that 
  Serial.println("Formating 32 slots");
  long t=millis();
  EFS.format(32);
  t=millis()-t;
  Serial.print("Format time = "); Serial.println(t);
  Serial.print(16*32+6); Serial.println(" bytes updated");
  Serial.print("Update baudrate * 1000 = "); Serial.print((16*32+6)*8/t); Serial.println();

  // begin the filesystem - return value is the number of slots in the FS
  int s;
  if (s=EFS.begin()) {
    Serial.println("Filessystem mounted");
    Serial.print(s); Serial.println(" slots in fs");
  }

  // file io demo 
  int file1=EFS.fopen("huhu", "w");  
  Serial.println("File open for write:");
  Serial.print("Slot id delivered "); Serial.println(file1);

  Serial.println("Bytes in slot:");
  Serial.println(EFS.available(file1));

  char message[16]="hello world";
  Serial.println("Storing to file:");
  Serial.println(message);
  
  for(int i=0; i<16 && message[i]!=0; i++) EFS.fputc(file1, message[i]);

  Serial.println("Bytes left in slot:");
  Serial.println(EFS.available(file1));

  EFS.fclose(file1);

  Serial.println("Filesize now:");
  Serial.println(EFS.filesize(file1));

  file1=EFS.fopen("huhu", "r");
  Serial.println("File open for read:");
  Serial.print("Slot id delivered "); Serial.println(file1);
  
  Serial.println("Reading content:");
  while (!EFS.eof(file1)) Serial.write((char) EFS.fgetc(file1));
  Serial.println();

  Serial.println("Reading again:");
  EFS.rewind(file1);
  while (!EFS.eof(file1)) Serial.write((char) EFS.fgetc(file1));
  Serial.println();

  // directory access demo
  int file2=EFS.fopen("haha", "w");  
  Serial.println("Opened a second file for write");
  Serial.print("Slot id delivered "); Serial.println(file2);
  
  EFS.fclose(file2);

  EFS.dirp=0;
  while (uint8_t file=EFS.readdir()) {
          Serial.print("Slot "); 
          Serial.print(file);
          Serial.print(": ");
          Serial.print(EFS.filename(file)); 
          Serial.print(" "); 
          Serial.println(EFS.filesize(file));
   } 

   EFS.remove("huhu");
   Serial.println("Delete of file huhu");

   EFS.dirp=0;
   while (uint8_t file=EFS.readdir()) {
        Serial.print("Slot "); 
        Serial.print(file);
        Serial.print(": ");
        Serial.print(EFS.filename(file)); 
        Serial.print(" "); 
        Serial.println(EFS.filesize(file));
   }

  int file3=EFS.fopen("hihi", "w");  
  Serial.println("Opened a third file for write");
  Serial.print("Slot id delivered "); Serial.println(file3);

  EFS.fflush(file3);

  EFS.dirp=0;
  while (uint8_t file=EFS.readdir()) {
        Serial.print("Slot "); 
        Serial.print(file);
        Serial.print(": ");
        Serial.print(EFS.filename(file)); 
        Serial.print(" "); 
        Serial.println(EFS.filesize(file));
  }

  EFS.rename("hihi", "holla");
  Serial.println("Renamed a file");

  EFS.dirp=0;
  while (uint8_t file=EFS.readdir()) {
        Serial.print("Slot "); 
        Serial.print(file);
        Serial.print(": ");
        Serial.print(EFS.filename(file)); 
        Serial.print(" "); 
        Serial.println(EFS.filesize(file));
  }
 
  Serial.println("-----------------------------------------");
  
}



void loop() {
  // put your main code here, to run repeatedly:

}
