/* Copyright (c) 2005, Regents of Massachusetts Institute of Technology, 
 * Brandeis University, Brown University, and University of Massachusetts 
 * Boston. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *   - Neither the name of Massachusetts Institute of Technology, 
 *     Brandeis University, Brown University, or University of 
 *     Massachusetts Boston nor the names of its contributors may be used 
 *     to endorse or promote products derived from this software without 
 *     specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// Problems: migfer@mit.edu

#include "PosDecoder.h"

PosDecoder::PosDecoder(bool valSorted_) : Decoder() {
	outBlock=new MultiBlock(valSorted_, true, true, ValPos::INTTYPE);
	outPair=outBlock->vp;
}
PosDecoder::PosDecoder(byte* buffer_, bool valSorted_) : Decoder(buffer_) { 
	outBlock=new MultiBlock(valSorted_, true, true, ValPos::INTTYPE);
	setBuffer(buffer_);
	outPair=outBlock->vp;
}
PosDecoder::PosDecoder(PosDecoder& decoder_) : Decoder(decoder_) { 
	reader=decoder_.reader;
	numIntsPtr=decoder_.numIntsPtr;
	startPos=decoder_.startPos;
	outBlock=new MultiBlock(*decoder_.outBlock);
	outPair=outBlock->vp;
} 

PosDecoder::~PosDecoder()
{
  delete outBlock;
}
	
void PosDecoder::setBuffer(byte* buffer_) { 
	Decoder::setBuffer(buffer_);
	if (buffer==NULL) throw new CodingException("PosDecoder: Error, buffer passed in is NULL");
	numIntsPtr= (unsigned int*)buffer_;
	startPos=1;
	currPos=1;
	reader.setBuffer(buffer_+sizeof(int), getPageLengthBits());
}

int PosDecoder::getValueSize(){ 
	return 8*sizeof(int);
}
int PosDecoder::getPageLengthBits() { 
	return 8*(*numIntsPtr)*sizeof(int);
}

bool PosDecoder::hasNextBlock() { 
	if (!initialized) return false;
	return (currPos-startPos<*numIntsPtr);
}

Block* PosDecoder::getNextBlock() {
  if (!initialized) return NULL; 
  int _numInts;
  byte* buffer;
  if (!(buffer=reader.getBuffer(_numInts))) return NULL;
  outBlock->setBuffer(currPos, _numInts, sizeof(int),buffer);
  currPos+=_numInts;
  return outBlock;
	
}

Block* PosDecoder::getNextBlockSingle() {
  if (!initialized) return NULL; 
  int value;
  if (!(reader.readInt(value))) return NULL;
  outBlock->setBuffer(currPos, 1, sizeof(int),(byte*)&value);;
  currPos++;
  return outBlock;
	
}
Block* PosDecoder::peekNextBlock() {
	if (!initialized) return NULL; 
	int value;
	int oldPos=reader.getCurrentIntPos();
	if (!reader.readInt(value)) return NULL;
	reader.skipToIntPos(oldPos);
	outPair->set(currPos, (byte*)&value);
	outBlock->currPos=-1;
	return outBlock;
	
}
bool PosDecoder::skipToPos(unsigned int blockPos_) { 
  if (blockPos_>=*numIntsPtr) return false;//NULL;
  if (reader.skipToIntPos(blockPos_)) {
    currPos=startPos+blockPos_;
    return true;
  }
  else {
    return false;//NULL;	
  }
}

bool PosDecoder::skipToBlockOnValue(int value_) {
  Log::writeToLog("PosDecoder", 0, "skipToBlockOnValue() called, val", value_);
  // currently scan down, however can do binary search when sorted in future
  reader.resetPos();
  int value;
  while (reader.readInt(value)) {
    currPos++;
    //int value=((MultiBlock*) peekNextBlock())->getValue();
    if (value!=value_) {
      if (outBlock->isValueSorted()) {
	if (value>value_) {
	  Log::writeToLog("PosDecoder", 0, "Did not find value, returning false, val", value_);
	  return false;
	}
	//getNextBlock();
      }
      //else {
      //getNextBlock();
      //}
    }
    else {
      Log::writeToLog("PosDecoder", 0, "skipToBlockOnValue() found value", value_);
      skipToPos(currPos-startPos -1);
      return true;
    }
  }
  Log::writeToLog("PosDecoder", 0, "Did not find value, returning false, val", value_);
  return false;
}

	
ValPos* PosDecoder::getStartVal() {
  delete utilityPair;
  utilityPair = new IntValPos(0,reader.readFirst());
  return utilityPair;
}
unsigned int PosDecoder::getStartPos() { 
	return startPos;
}
ValPos* PosDecoder::getEndVal() { 
  delete utilityPair;
  utilityPair = new IntValPos(0,reader.readLast());
  return utilityPair;
}
unsigned int PosDecoder::getEndPos() { 
	return startPos+(*numIntsPtr)-1;
}