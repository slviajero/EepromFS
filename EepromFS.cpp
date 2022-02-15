#include "EepromFS.h"

// lazy constructor
EepromFS::EepromFS(uint8_t e, unsigned int sz) {
	eepromsize=sz;
	eepromaddr=e;
	pagenumber=-1;
	pagechanged=false;
};

// the raw sequential read method 
uint8_t EepromFS::rawread(unsigned int a){
	int p=a/EFS_PAGESIZE;
	if (p != pagenumber) {
		// load the page 
		// send the address
		Wire.beginTransmission(eepromaddr);
		unsigned int pa=p*EFS_PAGESIZE;
		Wire.write((int)pa/256);
		Wire.write((int)pa%256);
		Wire.endTransmission();
		// wait just a little for the EEPROM to process
		// and then get data
		delayMicroseconds(50);
		Wire.requestFrom((int)eepromaddr, (int)EFS_PAGESIZE);
		// yield during wait
		uint8_t dc=0;
		delayMicroseconds(50);
		while( !Wire.available() && dc++ < 1000) delay(0);
		// collect the date
		if (Wire.available() == EFS_PAGESIZE) {
			for(uint8_t i=0; i<EFS_PAGESIZE; i++) pagebuffer[i]=(uint8_t) Wire.read();
			pagenumber=p;
		} else {
			debug=1;
			return 0;
		}
	}
	return pagebuffer[a%EFS_PAGESIZE];
}

// the raw sequential write method 
void EepromFS::rawwrite(unsigned int a, uint8_t d){
	uint8_t b;
	int p=a/EFS_PAGESIZE;
	if (p != pagenumber) rawflush();
	b=rawread(a);
	if (d == b) return;
	pagebuffer[a%EFS_PAGESIZE]=d;
	pagechanged=true;
}

// the raw flush methods
void EepromFS::rawflush(){
	if (pagechanged) {
		unsigned int pa=pagenumber*EFS_PAGESIZE;
		Wire.beginTransmission(eepromaddr);
		Wire.write((int)pa/256);
		Wire.write((int)pa%256);
		for(uint8_t i=0; i<EFS_PAGESIZE; i++) Wire.write(pagebuffer[i]);
		Wire.endTransmission();
		delay(10); // the write delay according to the AT24x datasheet
		pagechanged=false;
	}
}


// begin wants a formated filesystem
uint8_t EepromFS::begin() { 
	slotsize=0;
	if (rawread(0) == 'E') {
		nslots=rawread(1);
		if (nslots>0) slotsize=(eepromsize-4)/nslots;
	} 
	if (slotsize>EFS_FILEHEADERSIZE) return nslots; else return 0;
}

// zero the entire eeprom 
bool EepromFS::format(uint8_t s) {
	if (s>0) nslots=s; else return false;
	for(uint8_t a=0; a<EFS_HEADERSIZE; a++) rawwrite(a, 0);
	rawwrite(0, 'E'); 
	rawwrite(1, s); 
	slotsize=(eepromsize-4)/nslots;
	for(uint8_t s=1; s<=nslots; s++) clearslotheader(s);
	if (rawread(0) == 'E') return true; else return false;
}

// open and create a file 
uint8_t EepromFS::fopen(char* fn, char* m) {
	if (*m == 'r') {
		if(ifile=findfile(fn)) {
		 	ifilesize=getsize(ifile);
		 	ifilepos=0;
		}
		return ifile;
	}
	if (*m == 'w' || *m == 'a') {
		if (ofile=findfile(fn)) {
			ofilesize=getsize(ofile);
		} else {
			ofile=findemptyslot();
			ofilesize=0;
			if (ofile != 0) {
				putfilename(ofile, fn);
				putsize(ofile, 0);
			}
		}
		if (*m == 'w') ofilepos=0;
		if (*m == 'a') ofilepos=ofilesize;
		return ofile;
	}
}

// close a file - and write the correct filesize 
uint8_t EepromFS::fclose (uint8_t f){
	if (ifile == f) {
		ifilepos=0;
		return ifilesize;
	}
	if (ofile == f) {
		ofilepos=0;
		putsize(f, ofilesize);
		rawflush();
		return ofilesize;
	}
	return 0;
}

uint8_t EepromFS::fclose (char* m){
	if (*m == 'r') return fclose(ifile); 
	if (*m == 'w' || *m == 'a') return fclose(ofile);
}

// end of file on read 
bool EepromFS::eof(uint8_t f){
	return available(f)<=0;
}

// get a character from the file 
uint8_t EepromFS::fgetc(uint8_t f) {
	if (ifilepos<ifilesize) {
		return getdata(ifile, ifilepos++);
	}
}

// write a character to the file
bool EepromFS::fputc(uint8_t f, uint8_t ch){
	if (ofilepos<slotsize-1) {
		putdata(ofile, ofilepos++, ch);
		ofilesize++;
		return true;
	} else 
		return false;
}	

// write a character to the file
bool EepromFS::fflush(uint8_t f){
	if (ofile) putsize(ofile, ofilesize);
	rawflush();
	return true;
}	

// rewind the file for read
void EepromFS::rewind(uint8_t f) {
	if (ifile == f) {
		ifilepos=0;
	}
	if (ofile == f) {
		ofilepos=0;
		ofilesize=0;
	}	
}

// increment the dirpointer and read 
uint8_t EepromFS::readdir(){
	while(++dirp <= nslots) {
		if (getfilename(dirp)) return dirp;
	}
	return 0;
}

// the filename
char* EepromFS::filename(uint8_t f){
	if (getfilename(f)) return fnbuffer; 
	return 0;
}

// the filesize
unsigned int EepromFS::filesize(uint8_t f){
	return getsize(f);
}

// remove a file 
uint8_t EepromFS::remove(char* fn){
	uint8_t file=findfile(fn);
	if (file != 0) {
		for(uint8_t i=0; i<EFS_FILENAMELENGTH; i++) fnbuffer[i]=0;
		putsize(file, 0);
		putfilename(file, fnbuffer);
		rawflush();
	}
	return file;
}

// rename 
uint8_t EepromFS::rename(char* ofn, char* nfn){
	uint8_t file=findfile(ofn);
	putfilename(file, nfn);
	rawflush();
	return file;
}


// find a file by name or find and empty slot
uint8_t EepromFS::findfile(char* fn) {
	for(uint8_t i=1; i<=nslots; i++) {
		if (getfilename(i)) {
			bool found = true;
			for(uint8_t j=0; j<EFS_FILENAMELENGTH && fn[j] !=0; j++) {
				if (fnbuffer[j] != fn[j] ) {found=false; break; }
			}
			if (found) return i;
		}
	}
	return 0;
}

// find a file by name or find and empty slot
uint8_t EepromFS::findemptyslot() {
	for(uint8_t i=1; i<=nslots; i++) {
		if (!getfilename(i)) return i;
	}
	return 0;
}

// find the slots address
unsigned int EepromFS::findslot(uint8_t s) {
	if (s > 0 && s <= nslots) {
		return EFS_HEADERSIZE + (s-1)*slotsize;
	} else {
		return 0;
	}
}

// get the filename into the local buffer and return the length
uint8_t EepromFS::getfilename(uint8_t s) {
	uint8_t i;
	unsigned int a=findslot(s);
	if (a == 0) return 0;
	for (i=0; i<EFS_FILENAMELENGTH; i++) if ( (fnbuffer[i]=rawread(a+i)) == 0) break;
	return i;
}

// put a filename into a slot and pad the data with 0
void EepromFS::putfilename(uint8_t s, char* fn) {
	uint8_t i;
	unsigned int a=findslot(s);
	if (a == 0) return;
	for(i=0; i<EFS_FILENAMELENGTH && fn[i]!=0; i++) rawwrite(a+i, (uint8_t) fn[i]);
	while(i<EFS_FILENAMELENGTH) rawwrite(a+(i++), 0);
}

// clear the header of a slot - needed for formating
void EepromFS::clearslotheader(uint8_t s) {
	unsigned int a=findslot(s);
	if (a == 0) return;
	for(uint8_t i=0; i<EFS_FILEHEADERSIZE; i++) rawwrite(a+i, 0);
}

// get the size bigendian
unsigned int EepromFS::getsize(uint8_t s) {
	unsigned int a=findslot(s);
	if (a == 0) return;
	return rawread(a+EFS_FILENAMELENGTH+1)+rawread(a+EFS_FILENAMELENGTH+2)*256;
}

// put the size bigendian
void EepromFS::putsize(uint8_t s, unsigned int sz) {
	unsigned int a=findslot(s);
	if (a == 0) return;
	rawwrite(a+EFS_FILENAMELENGTH+1, sz%256);
	rawwrite(a+EFS_FILENAMELENGTH+2, sz/256);
}

// access to one byte of data in the slot
uint8_t EepromFS::getdata(uint8_t s, unsigned int i) {
	unsigned int a=findslot(s);
	if (a == 0) return 0;
	if (i>=0 && i<slotsize) return rawread(a+EFS_FILEHEADERSIZE+1+i); else return 0;
}

// put one byte of data in a slot
void EepromFS::putdata(uint8_t s, unsigned int i, uint8_t d){
	int a=findslot(s);
	if (a == 0) return 0;
	if (i>=0 && i<slotsize) rawwrite(a+EFS_FILEHEADERSIZE+1+i, d);
}

// stream class like available, for output it returns the free bytes
int EepromFS::available(uint8_t f) {
	if (f == ifile) {
		return ifilesize-ifilepos;
	}
	if (f == ofile) {
		return slotsize-ofilesize;
	} 
	return 0;
}








