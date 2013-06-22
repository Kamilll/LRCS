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

#include "StringDecoder.h"

StringDecoder::StringDecoder(bool valSorted_) : Decoder() {  
	outMultiBlock=new MultiSBlock(valSorted_, true, true);
	//outBlock=new SBasicBlock(valSorted_, true, true);
	valSorted = valSorted_;
	//outPair=outBlock->pair;
}
StringDecoder::StringDecoder(byte* buffer_, bool valSorted_) : Decoder(buffer_) { 
	outMultiBlock=new MultiSBlock(valSorted_, true, true);
	//outBlock=new SBasicBlock(valSorted_, true, true);
	valSorted = valSorted_;
	setBuffer(buffer_);
	//outPair=outBlock->pair;
}
StringDecoder::StringDecoder(StringDecoder& decoder_) : Decoder(decoder_) { 
	reader=decoder_.reader;
	numStringsPtr=decoder_.numStringsPtr;
	startPosPtr=decoder_.startPosPtr;
	ssizePtr=decoder_.ssizePtr;
	//outBlock=new SBasicBlock(*decoder_.outBlock);
	outMultiBlock=new MultiSBlock(*decoder_.outMultiBlock);
	//outPair=outBlock->pair;
} 

StringDecoder::~StringDecoder()
{

}

void StringDecoder::setBuffer(byte* buffer_) { 
	Decoder::setBuffer(buffer_);
	if (buffer==NULL) throw new CodingException("StringDecoder: Error, buffer passed in is NULL");
	numStringsPtr=(unsigned int*) buffer_;
	startPosPtr=(unsigned int*) (buffer_+sizeof(int));
	ssizePtr=(unsigned int*) (buffer_+(2*sizeof(int)));
	currPos=*startPosPtr;
	Log::writeToLog("StringDecoder", 1, "setBuffer(): buffer has numStrings", *numStringsPtr);
	Log::writeToLog("StringDecoder", 1, "setBuffer(): buffer has startPos", *startPosPtr);
	Log::writeToLog("StringDecoder", 1, "setBuffer(): buffer has ssize", *ssizePtr);

	reader.setStringSize(*ssizePtr);
	reader.setBuffer(buffer_+3*sizeof(int), getPageLengthBits()-8*3*sizeof(int));	

}

int StringDecoder::getValueSize(){ 
	return 8*(*ssizePtr);
}
int StringDecoder::getPageLengthBits() { 
	return (8*((((unsigned int) *numStringsPtr) * (*ssizePtr)) + (3*sizeof(int))));
}

bool StringDecoder::hasNextBlock() { 
	if (!initialized) return false;
	return (currPos-*startPosPtr<*numStringsPtr);
}

Block* StringDecoder::getNextBlock() {
	if (!initialized) return NULL; 
	int numVals;
	byte* buffer;
	if (!(buffer=reader.getBuffer(numVals))) return NULL;
	outMultiBlock->setBuffer(currPos, numVals,*ssizePtr,buffer);
	currPos+=numVals;
	return outMultiBlock;
}

Block* StringDecoder::getNextBlockSingle() {
	if (!initialized) return NULL; 
	char* value = new char[*ssizePtr];
	if (!(reader.readString(value))) return NULL;
	outMultiBlock->setBuffer(currPos, 1, (byte*)&value, *ssizePtr);
	currPos++;
	return outMultiBlock;
}

/*SBlock* StringDecoder::getNextSBlockSingle() {
	if (!initialized) return NULL; 
	char* value =new char[*ssizePtr];
	if (!(reader.readString(value))) return NULL;
	outBlock->setValue(value, currPos);
	currPos++;
	return outBlock;

}*/
Block* StringDecoder::peekNextBlock() {
	if (!initialized) return NULL; 
	char* value = new char(*ssizePtr);
	int oldPos=reader.getCurrentPos();
	if (!(reader.readString(value))) return NULL;
	reader.skipToStringPos(oldPos);
	outMultiBlock->setBuffer(currPos, 1, (byte*)&value, *ssizePtr);
	//outPair->setValue(currPos, (byte*)&value);
	outMultiBlock->currPos=-1;

	return outMultiBlock;
}
bool StringDecoder::skipToPos(unsigned int blockPos_) { 
	if (blockPos_>=*numStringsPtr) return false;//NULL;
	if (reader.skipToStringPos(blockPos_)) {
		currPos=*startPosPtr+blockPos_;
		return true;
	}
	else {
		return false;//NULL;	
	}
}

bool StringDecoder::skipToBlockOnValue(ValPos* rhs_) {
	//throw new UnimplementedException("Not done yet");
/*Zklee: This method need re-implemented considering other situations,
	 like non sorted blocks*/
	//Log::writeToLog("StringDecoder", 0, "skipToBlockOnValue() called, val", value_);
		
	reader.resetPos();
	char* value = new char[*ssizePtr];
		
	while (reader.readString(value)) {
		currPos++;
		//int value=((MultiBlock*) peekNextBlock())->getValue();
		ValPos* lhs = new StringValPos(1,*ssizePtr);
		lhs->set(value);
		
		if (*lhs!=rhs_) {
			if (outMultiBlock->isValueSorted()) {
				
				if (*lhs>rhs_) {
					//Log::writeToLog("StringDecoder", 0, "Did not find value, returning false, val", value_);
					return false;
				}
				//getNextBlock();
			}
			//else {
			//getNextBlock();
			//}
		}
		else {
			//Log::writeToLog("StringDecoder", 0, "skipToBlockOnValue() found value", value_);
			skipToPos(currPos-*startPosPtr -1);
			return true;
		}
	}
	//Log::writeToLog("StringDecoder", 0, "Did not find value, returning false, val", value_);
	return false;
}


ValPos* StringDecoder::getStartVal() {
	delete utilityPair;
	utilityPair = reader.readFirst();
	return utilityPair;
}
unsigned int StringDecoder::getStartPos() { 
	return *startPosPtr;
}
ValPos* StringDecoder::getEndVal() { 
	//	return reader.readLast();
	delete utilityPair;
	utilityPair = reader.readLast();
	return utilityPair;
}
unsigned int StringDecoder::getEndPos() { 
	return (*startPosPtr)+(*numStringsPtr)-1;
}
