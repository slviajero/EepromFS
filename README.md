# EepromFS

Filesystem like interface to an EEPROM module. Intended for the larger clock EEPROMS like the 4kB module in many real time clocks and the 32kB AT24C256 modules. 

The library was originally written for my IoT BASIC interpreter for microcontrollers (https://github.com/slviajero/tinybasic). 

The filesystem is extremely simple. It partitions the EEPROM into n slots of identical size. A file can be maximally as big as its slot. There is only one (root) directory. 

The interface to the filesystem is POSIX style. The following public methods are implemented:

	bool format(uint8_t s);
	uint8_t fopen(char* fn, char* m);
	uint8_t fclose(uint8_t f);
	uint8_t fclose(char* m);
	bool eof(uint8_t f);
	uint8_t fgetc(uint8_t f);
	bool fputc(uint8_t f, uint8_t ch);
	bool fflush(uint8_t f);
	void rewind(uint8_t f);
	uint8_t readdir();
	char* filename(uint8_t f);
	unsigned int filesize(uint8_t f);
	uint8_t remove(char* fn);
	uint8_t rename(char* ofn, char* nfn);
	int available(uint8_t f);
  
  In addition to this 
  
  uint8_t rawread(unsigned int a);
	void rawwrite(unsigned int a, uint8_t d);
	void rawflush();
  
  can be used for buffered access to individual memory cells. 
  
  The library is experimental right now. 
  
