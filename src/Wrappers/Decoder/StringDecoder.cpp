#include "StringDecoder.h"

StringDecoder::StringDecoder(bool valSorted_) : Decoder() {  
	outMultiBlock=new MultiBlock(valSorted_, true, true);
	valSorted = valSorted_;
}
StringDecoder::StringDecoder(byte* buffer_, bool valSorted_) : Decoder(buffer_) { 
	outMultiBlock=new MultiBlock(valSorted_, true, true);
	valSorted = valSorted_;
	setBuffer(buffer_);
}
StringDecoder::StringDecoder(StringDecoder& decoder_) : Decoder(decoder_) { 
	reader=decoder_.reader;
	numStringsPtr=decoder_.numStringsPtr;
	startPosPtr=decoder_.startPosPtr;
	ssizePtr=decoder_.ssizePtr;
	outMultiBlock=new MultiBlock(*decoder_.outMultiBlock);
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
    reader.setStringSize(*ssizePtr);
	reader.setBuffer(buffer_+3*sizeof(int), getPageLengthBits()-8*3*sizeof(int));	

}

int StringDecoder::getValueSize(){ 
	return 8*(*ssizePtr);
}

unsigned int StringDecoder::getCurrPosition(){ 
	return currPos;
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
	//currPos+=numVals;
	return outMultiBlock;
}

Block* StringDecoder::getNextBlockSingle() {
	if (!initialized) return NULL; 
	char* value = new char[*ssizePtr];
	if (!(reader.readString(value))) return NULL;
	outMultiBlock->setBuffer(currPos, 1,*ssizePtr,(byte*)&value);
	currPos++;
	return outMultiBlock;
}

Block* StringDecoder::peekNextBlock() {
	if (!initialized) return NULL; 
	char* value = new char(*ssizePtr);
	int oldPos=reader.getCurrentPos();
	if (!(reader.readString(value))) return NULL;
	reader.skipToStringPos(oldPos);
	outMultiBlock->setBuffer(currPos, 1, *ssizePtr,(byte*)&value);
	return outMultiBlock;
}
bool StringDecoder::skipToPos(unsigned int blockPos_) { 
	int posInBlock = blockPos_-*startPosPtr+1;
	if (posInBlock>*numStringsPtr) return false;//NULL;
	if (reader.skipToStringPos(posInBlock)) {
		currPos=blockPos_;
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
		ValPos* lhs = new StringValPos(*ssizePtr);
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
