#include "StringWriter.h"

StringWriter::StringWriter(int stringSize_, int bfrSizeInBits_)
{
	buffer=new byte[bfrSizeInBits_/8];
	currBytePtr=buffer;
	memset(buffer, 0, (bfrSizeInBits_/8)); // to prevent rounding errors
	numBytesWritten=0;
	bufferSizeInBytes=bfrSizeInBits_/8;
	oldBufferSizeInBytes=bfrSizeInBits_/8;
	stringSize= stringSize_;
}

StringWriter::~StringWriter()
{
	
}

bool StringWriter::writeString(char* val_) {
	if ((numBytesWritten)>(bufferSizeInBytes-stringSize)) return false;
	memcpy(currBytePtr,val_,stringSize);
	currBytePtr+=stringSize;
	numBytesWritten+=stringSize;
	return true;
}

bool StringWriter::writeString(char* val_, int numBytes) {
	if ((numBytesWritten)>(bufferSizeInBytes-numBytes)) return false;
	memcpy(currBytePtr,val_,numBytes);
	currBytePtr+=numBytes;
	numBytesWritten+=numBytes;
	return true;
}

bool StringWriter::skipToBytePos(unsigned int pos_) {
  currBytePtr=buffer+(pos_);
  numBytesWritten=(pos_);
  return true;
}

bool StringWriter::skipToIntPos(unsigned int pos_) {
  //if (pos_>=3) return false; //NOT great code clean later
  assert(pos_<=5);
  currBytePtr=buffer+(pos_*sizeof(int));
  numBytesWritten=(pos_*sizeof(int));
  return true;
}

int StringWriter::getNumBytesWritten() {
  return numBytesWritten;
}

int StringWriter::getNumValuesWritten() {
  return numBytesWritten/stringSize;
}

int StringWriter::getBufferSize() {
	return 8*bufferSizeInBytes;
}

int StringWriter::getStringSize() {
	return stringSize;
}

byte* StringWriter::getBuffer() {
	return ((byte*) buffer);	
}

void StringWriter::resetBuffer() {
  currBytePtr=buffer;
  numBytesWritten=0;
}

bool StringWriter::reserveBufferSpace(int numInts) {
  assert(numInts>=0);
  bufferSizeInBytes -= (numInts*sizeof(int));
  if (numBytesWritten>=bufferSizeInBytes) {
    bufferSizeInBytes += (numInts*sizeof(int));
    return false;
  }
  return true;
}

void StringWriter::unReserveSpace() {
  bufferSizeInBytes=oldBufferSizeInBytes;
}

void StringWriter::unWriteString() {
  assert(numBytesWritten >= stringSize);
  currBytePtr-=stringSize;
  numBytesWritten-=stringSize;
}
