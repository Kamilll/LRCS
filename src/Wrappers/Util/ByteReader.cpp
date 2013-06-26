#include "ByteReader.h"

ByteReader::ByteReader(byte* buffer_, int numBitsInBuffer_)
{
	buffer=buffer_;
	currPosPtr=buffer;
	numBytesInBuffer=numBitsInBuffer_/8;
	currPos=0;
}

ByteReader::~ByteReader()
{
}

bool ByteReader::readByte(byte& value) {
	if (currPos+1>numBytesInBuffer) return false;
	value=(byte) (*currPosPtr);
	currPosPtr++;
	currPos++;
	return true;
}
bool ByteReader::readShort(unsigned short& value) {
	if (currPos+2>numBytesInBuffer) return false;
	value=(unsigned short) (*((unsigned short*) currPosPtr));
	currPosPtr+=2;
	currPos+=2;
	return true;
}

bool ByteReader::readThree(unsigned int& value) {
	if (currPos+3>numBytesInBuffer) return false;
	unsigned short shrt;
	byte byt;
	readShort(shrt);
	readByte(byt);
	
	value=(unsigned int) ((shrt*0x100)+byt);
	return true;
}
bool ByteReader::readInt(unsigned int& value) {
	if (currPos+4>numBytesInBuffer) return false;
	value=(unsigned int) (*((unsigned int*) currPosPtr));
	currPosPtr+=4;
	currPos+=4;
	return true;
}

void ByteReader::resetPos() {
	currPosPtr=buffer;
	currPos=0;			
}
bool ByteReader::skipToIntPos(unsigned int intPos_) {
	return skipToBytePos(intPos_*sizeof(int));
}
bool ByteReader::skipToBytePos(unsigned int bytePos_) {
	if (bytePos_>=numBytesInBuffer) return false;
	currPos=bytePos_;
	currPosPtr=buffer+bytePos_;	
	return true;
}
int ByteReader::readFirst() {
	return buffer[0];
}
int ByteReader::readLast() {
	return buffer[numBytesInBuffer-1];
}

int ByteReader::getCurrentBytePos() {
	return currPos;
} 
void ByteReader::setCurrentBytePos(int currPos_) {
        currPos = currPos_;
}
byte* ByteReader::getCurrentPosPtr() {
        return currPosPtr;
}
void ByteReader::setCurrentPosPtr(byte* currPosPtr_) {
        currPosPtr = currPosPtr_;
}
int ByteReader::getMaxNumBytes() {
	return numBytesInBuffer;
}
void ByteReader::setBuffer(byte* buffer_, int numBitsInBuffer_) {
	buffer=buffer_;
	currPosPtr=buffer;
	numBytesInBuffer=numBitsInBuffer_/8;
	currPos=0;		
}

