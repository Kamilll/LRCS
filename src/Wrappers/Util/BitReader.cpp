#include "BitReader.h"

BitReader::BitReader(byte* buffer_, int numBitsInBuffer_)
{
	Log::writeToLog("BitReader", 0, "Constructing buffer");
	buffer=buffer_;
	numBitsInBuffer=numBitsInBuffer_;
	initReader();	
}

BitReader::~BitReader()
{
}

bool BitReader::readBits(unsigned int& value, int numBits_) {
	if (numBits_==0) {
		value=0;
		return true;	
	}
	if (numBits_+numBitsRead>numBitsInBuffer)
		return false;

	byte numBitsAvail=32-currentBit;
	if (numBitsAvail==numBits_) {
		value=(0xFFFFFFFF>>currentBit)&(*currentInt);
		currentInt++;
		currentBit=0;	
	}
	else if (numBitsAvail>=numBits_) {
		value=(0xFFFFFFFF>>currentBit)&(*currentInt);
		value=value>>(numBitsAvail-numBits_);			
		currentBit+=numBits_;
	}
	else {
		// first part
		value=(0xFFFFFFFF>>currentBit)&(*currentInt);
		byte numBitsRemain=numBits_-numBitsAvail;
		value=value<<numBitsRemain;

		// second part
		currentInt++;
		value|=(*currentInt)>>(32-numBitsRemain);		
		currentBit=numBitsRemain;
	}
	numBitsRead+=numBits_;
	return true;
}

// Starts reading the buffer from the start
void BitReader::resetPos() {
	currentInt=(unsigned int*) buffer;
	currentBit=0;
	numBitsRead=0;	
}
	
// Start reading from a specified bit postion
bool BitReader::skipToBitPos(int bitPos_) {
	if (bitPos_<numBitsInBuffer) {
		currentInt=(unsigned int*) buffer;
		currentInt+=(bitPos_/32);
		currentBit=bitPos_%32;
		numBitsRead=bitPos_;
		return true;
	}
	else {
		return false;
	}	
}
	
// Get current bit position
int BitReader::getCurrentBitPosition() {
	return (8*((int)((byte*) currentInt-buffer))+currentBit);
}
	
// Get max bit
int BitReader::getNumBitsInBuffer() {
	return numBitsInBuffer;
}
	
void BitReader::setBuffer(byte* buffer_, int numBitsInBuffer_) {
	Log::writeToLog("BitReader", 0, "Constructing buffer");
	buffer=buffer_;
	numBitsInBuffer=numBitsInBuffer_;
	initReader();	
}

void BitReader::initReader() {
	if (buffer==NULL) {
		Log::writeToLog("BitReader", 2, "Error, bitreader cannot be initialized with a NULL buffer");
		throw new CodingException("BitReader: Passed a NULL buffer in initializer");	
	}
	resetPos();
}
