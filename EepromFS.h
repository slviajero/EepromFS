/*
	Library for a basic ""slot based" file system.

	A storage array of a given size is divided into n equal 
	sized slots, each one with a file name. The slot length 
	is the maximum file size. 

	The filesystem can have one open file for read and one open
	file for write/append.

	Filenames are 12 bytes long. 2 bytes length and 2 bytes reserved.
	(default fileheader of 16 bytes)

	Everything is stores as uint8_8.

*/ 

#include <Arduino.h>
#include <Wire.h>

#define EFS_HEADERSIZE 4
#define EFS_FILENAMELENGTH 12
#define EFS_FILEHEADERSIZE EFS_FILENAMELENGTH+2+2
#define EFS_PAGESIZE 16

//typedef signed char uint8_t;

class EepromFS {
public:
/* 
	constructor 
*/
	EepromFS(uint8_t e, unsigned int sz);

/*
	begin method - main purpose is connection to the
	Wire type eeprom
*/
	uint8_t begin();

/*
	formatting the filesystem with s slots
*/

	bool format(uint8_t s);

/*
	POSIX style interface to the filesystem	
*/
	uint8_t fopen(char* fn, char* m);
	uint8_t fclose(uint8_t f);
	uint8_t fclose(char* m);
	bool eof(uint8_t f);
	uint8_t fgetc(uint8_t f);
	bool fputc(uint8_t f, uint8_t ch);
	bool fflush(uint8_t f);
	void rewind(uint8_t f);

/*
	handling the directory
*/
	uint8_t readdir();
	char* filename(uint8_t f);
	unsigned int filesize(uint8_t f);
	uint8_t remove(char* fn);
	uint8_t rename(char* ofn, char* nfn);

/*
	a few more methods
*/
	int available(uint8_t f);

	// directory pointer
	uint8_t dirp;

	// debug flag and function
	uint8_t debug;

	// raw read and write methods - ought to be private, somehow
	uint8_t rawread(unsigned int a);
	void rawwrite(unsigned int a, uint8_t d);
	void rawflush();

private:
	// the eeprom address
	uint8_t eepromaddr;
	unsigned int eepromsize;


	// headers and files
	char fnbuffer[EFS_FILENAMELENGTH];
	uint8_t pagebuffer[EFS_PAGESIZE];


	// the slot number of the input and output file
	uint8_t ifile;
	uint8_t ofile;

	// the file position
	unsigned int ofilepos;
	unsigned int ifilepos;

	// the actual file size of the open file
	unsigned int ofilesize;
	unsigned int ifilesize;
	
	// finding files and storage space
	uint8_t findfile(char* fn);
	uint8_t findemptyslot();
	unsigned int findslot(uint8_t s);
	void clearslotheader(uint8_t s);
	uint8_t getfilename(uint8_t s);
	void putfilename(uint8_t s, char* fn);
	unsigned int getsize(uint8_t s);
	void putsize(uint8_t s, unsigned int sz);
	uint8_t getdata(uint8_t s, unsigned int i);
	void putdata(uint8_t s, unsigned int i, uint8_t d);
	
	// number of slot
	uint8_t nslots;
	unsigned int slotsize;

	// paging mechanisms
	int  pagenumber;
	bool pagechanged;
};