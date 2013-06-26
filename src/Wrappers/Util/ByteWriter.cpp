#include "ByteWriter.h"

ByteWriter::ByteWriter(int bfrSizeInBits_)
{
	buffer=new byte[bfrSizeInBits_/8];
	currBytePtr=buffer;
	memset(buffer, 0, (bfrSizeInBits_/8)); 
	numBytesWritten=0;
	bufferSizeInBytes=bfrSizeInBits_/8;
}

ByteWriter::~ByteWriter()
{
}
bool ByteWriter::writeByte(byte val_) {
	if (numBytesWritten+1>bufferSizeInBytes) return false;
	*((short*) currBytePtr)=val_;
	currBytePtr+=1;
	numBytesWritten+=1;
	return true;	
}
bool ByteWriter::writeShort(unsigned short val_) {
	if (numBytesWritten+2>bufferSizeInBytes) return false;
	*((unsigned short*) currBytePtr)=val_;
	currBytePtr+=2;
	numBytesWritten+=2;
	return true;
}
bool ByteWriter::writeThree(unsigned int val_) {
	bool success=true;
	
	unsigned short valUp=(0xFFFF&(val_>>8));
	byte valLow=0xFF&(val_);
	
	byte* oldBytePtr=currBytePtr;
	int oldNumBytesWritten=numBytesWritten;
	
	
	success&=writeShort(valUp);
	success&=writeByte(valLow);
	
	if (!success) {
		currBytePtr=oldBytePtr;
		numBytesWritten=oldNumBytesWritten;
		return false;
	}
	return true;
}
bool ByteWriter::writeInt(unsigned int val_) {
	if (numBytesWritten+4>bufferSizeInBytes) return false;
	*((unsigned int*) currBytePtr)=val_;
	currBytePtr+=4;
	numBytesWritten+=4;
	return true;
}

bool ByteWriter::skipToBytePos(unsigned int pos_) {
	if (pos_>=bufferSizeInBytes) return false;
	currBytePtr=buffer+pos_;
	numBytesWritten=pos_;
	return true;
}

int ByteWriter::getNumBytesWritten() {
	return numBytesWritten;
}

int ByteWriter::getBufferSize() {
	return 8*bufferSizeInBytes;
}

byte* ByteWriter::getBuffer() {
	return buffer;	
}

void ByteWriter::resetBuffer() {
	currBytePtr=buffer;
	numBytesWritten=0;
}
