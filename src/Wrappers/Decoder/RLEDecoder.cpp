#include "RLEDecoder.h"

RLEDecoder::RLEDecoder(bool valSorted_, bool isPosColumn_):RLESuperDecoder() {
	valSorted=valSorted_;
	isPosColumn=isPosColumn_;
	result=new RLETriple();
	if(!isPosColumn)
	    result->value = new IntValPos();
	reader=NULL;
	block=NULL;
}

RLEDecoder::RLEDecoder(byte* buffer_, bool valSorted_, bool isPosColumn_):RLESuperDecoder(buffer_)
{
	valSorted=valSorted_;
	isPosColumn=isPosColumn_;
	result=new RLETriple();
	if(!isPosColumn)
	    result->value = new IntValPos();
	reader=NULL;
	block=NULL;
	setBuffer(buffer_);
}

RLEDecoder::~RLEDecoder()
{
   delete result;
   if(block != NULL )delete block;
   if(reader != NULL )delete reader;
}

/*
 *	+-----------------------------------+
 *	|valueSize(short)|pageLength(int)|startPosSize(byte)|repsSize(byte)
 *	+-----------------------------------+
 *	|Triple->
 *	+-----------------------------------+
 *  |Triple-->
 *	+-----------------------------------+
 */
void RLEDecoder::setBuffer(byte* buffer_) {
	Decoder::setBuffer(buffer_);
	if (buffer==NULL)
		throw new CodingException("RLEDecoder: Input to RLEDecoder constructor is NULL");
    int pageLength;
	if(!isPosColumn){
		data=(buffer+sizeof(short)+sizeof(int)+2);
		valueSize=*((short*) buffer);
		startPosSize=*(data-2);
		repsSize=*(data-1);
		pageLength=(this->getPageLengthBits());
	}else{
        data = buffer;
		valueSize = 0;
		startPosSize = 8*sizeof(int);
		repsSize=8*sizeof(int);
		pageLength=16*sizeof(int);
	}
	tripleSize=valueSize+startPosSize+repsSize;
	

	if (reader==NULL)
		reader=new BitReader(data, pageLength);
	else
		reader->setBuffer(data, pageLength);
}

bool RLEDecoder::hasNextTriple() {
	return (reader->getCurrentBitPosition()+tripleSize<=reader->getNumBitsInBuffer());	
}

RLETriple* RLEDecoder::getNextTriple() {
	// to guarantee atomicity in reading if there is a failure
	int oldBitPos=reader->getCurrentBitPosition();
	unsigned int value;
	if (!reader->readBits(value, valueSize)) {
		reader->skipToBitPos(oldBitPos);	
		Log::writeToLog("RLEDecoder", 0, "getNextTriple() Could not read value");
		return NULL;
	}	
			
	if (!reader->readBits(result->startPos, startPosSize)) {
		reader->skipToBitPos(oldBitPos);	
		Log::writeToLog("RLEDecoder", 0, "getNextTriple() Could not read startPos");
		return NULL;
	}
	if (!reader->readBits(result->reps, repsSize)) {
		reader->skipToBitPos(oldBitPos);	
		Log::writeToLog("RLEDecoder", 0, "getNextTriple() Could not read reps");
		return NULL;
	}
	if(!isPosColumn)
	  result->value->set(result->startPos, (byte*)&value);
	return result;
}
RLETriple* RLEDecoder::peekNextTriple() {
	int oldBitPos=reader->getCurrentBitPosition();
	RLETriple* retTrip=getNextTriple();
	reader->skipToBitPos(oldBitPos);
	return retTrip;
}

bool RLEDecoder::skipToTriple(unsigned int triplePos_) {
	if (triplePos_>=getSize()) return false;
	return reader->skipToBitPos(triplePos_*tripleSize);
}
RLETriple* RLEDecoder::getTriple(unsigned int triplePos_) {	
	if (triplePos_>=getSize()) return false;
	// to guarantee atomicity in reading if there is a failure
	int oldBitPos=reader->getCurrentBitPosition();
	reader->skipToBitPos(triplePos_*tripleSize);
		
	unsigned int value;
	if (!reader->readBits(value, valueSize)) {
		reader->skipToBitPos(oldBitPos);	
		Log::writeToLog("RLEDecoder", 0, "getTriple() Could not read value");
		return NULL;
	}	
	
	if (!reader->readBits(result->startPos, startPosSize)) {
		reader->skipToBitPos(oldBitPos);	
		Log::writeToLog("RLEDecoder", 0, "getTriple() Could not read startPos");
		return NULL;
	}
	
	if (!reader->readBits(result->reps, repsSize)) {
		reader->skipToBitPos(oldBitPos);	
		Log::writeToLog("RLEDecoder", 0, "getTriple() Could not read reps");
		return NULL;
	}
	
	
    if(!isPosColumn)
	   result->value->set(result->startPos, (byte*)&value);	  

	reader->skipToBitPos(oldBitPos);	
	return result;	
}

bool RLEDecoder::hasNextBlock() {
	if (!initialized) return false;
	return hasNextTriple();
}
Block* RLEDecoder::getNextBlock() {
	if (!initialized) throw new CodingException("RLEDecoder: Error, decoder not initialized with buffer");
	RLETriple* triple=getNextTriple();
	if (triple==NULL) return NULL;
	
	if (block==NULL) {
		block=new RLEBlock(triple,valSorted);	
	}
	else {
		block->setTriple(triple);	
	}
	return block;
}
Block* RLEDecoder::skipToBlock(unsigned int blockPos_) {
	if (!initialized) throw new CodingException("RLEDecoder: Error, decoder not initialized with buffer");
	if (!skipToTriple(blockPos_)) return NULL;
	RLETriple* triple=getNextTriple();
	if (triple==NULL) return NULL;
	
	if (block==NULL) {
		block=new RLEBlock(triple,valSorted);	
	}
	else {
		block->setTriple(triple);	
	}
	return block;
}
//consume the block wih position blockPos_
bool RLEDecoder::skipToPos(unsigned int blockPos_) {
  throw new CodingException("DNA 7/2/06: I don't think this method is correct but I don't think it matters since this method never gets called in runtime code. Thus, I'll place this here to see if it gets called.");
	if (!initialized) throw new CodingException("RLEDecoder: Error, decoder not initialized with buffer");
	if (!skipToTriple(blockPos_)) return false;
	RLETriple* triple=getNextTriple();
	if (triple==NULL) return false;
	
	return true;
}
bool RLEDecoder::skipToBlockOnValue(int value_) {
	//Log::writeToLog("RLEDecoder", 0, "skipToBlockOnValue() called, val", value_);
	// currently scan down, however can do binary search when sorted in future
	reader->resetPos();
	while (hasNextTriple()) {
		RLETriple* triple=peekNextTriple();
		if (*(triple->value)!=value_) {
			if (valSorted) {
				if (*(triple->value)>value_) {
					Log::writeToLog("RLEDecoder", 0, "Did not find value, returning false, val", value_);
					return false;
				}
				getNextBlock();
			}
			else {
				getNextBlock();
			}
		}
		else {
			Log::writeToLog("RLEDecoder", 0, "skipToBlockOnValue() found value", value_);
			return true;
		}
	}
	Log::writeToLog("RLEDecoder", 0, "Did not find value, returning false, val", value_);
	return false;
}
bool RLEDecoder::skipToBlockOnPos(unsigned int pos) {
	// currently scan down, however can do binary search when sorted in future
	reader->resetPos();
	RLETriple* triple = NULL;
	while (hasNextTriple()) {
	  triple=peekNextTriple();
	  if ((triple->startPos + triple->reps -1) < pos)
	    getNextBlock();
	  else
	    break;
	}
	if ((triple) && (triple->startPos<=pos) && (pos<=(triple->startPos+triple->reps-1))) 
	  return true;
	else
	  return false;
}


Block* RLEDecoder::getBlock(unsigned int blockPos_) {
	if (!initialized) throw new CodingException("RLEDecoder: Error, decoder not initialized with buffer");
	RLETriple* triple=getTriple(blockPos_);
	if (triple==NULL) return NULL;
	
	if (block==NULL) {
		block=new RLEBlock(triple,valSorted);	
	}
	else {
		block->setTriple(triple);	
	}
	return block;
}
	
ValPos* RLEDecoder::getStartVal() {
	if (!initialized) throw new CodingException("RLEDecoder: Error, decoder not initialized with buffer");
	int oldBitPos=reader->getCurrentBitPosition();
	unsigned int startInt; 
	reader->resetPos();
	if (!reader->readBits(startInt, valueSize)) {
		// throw exception
		throw new CodingException("RLEDecoder: Cannot read Start Int");
	}
	
	reader->skipToBitPos(oldBitPos);	
	Log::writeToLog("RLEDecoder", 0, "Start int=", startInt);
	
	delete utilityPair;
	utilityPair = new IntValPos(0,(int)startInt);
	return utilityPair;
}
unsigned int RLEDecoder::getStartPos() {
	if (!initialized) throw new CodingException("RLEDecoder: Error, decoder not initialized with buffer");
	int oldBitPos=reader->getCurrentBitPosition();
	unsigned int startPos;
	reader->skipToBitPos(valueSize);	
	
	if (!reader->readBits(startPos, startPosSize)) {
		// throw exception
		throw new CodingException("RLEDecoder: Cannot read Start Pos");
	}	
	reader->skipToBitPos(oldBitPos);	
	return startPos;	
}

ValPos* RLEDecoder::getEndVal() {
	if (!initialized) throw new CodingException("RLEDecoder: Error, decoder not initialized with buffer");
	int oldBitPos=reader->getCurrentBitPosition();
	unsigned int endInt;
	int endIntPos=reader->getNumBitsInBuffer()-tripleSize;	
	reader->skipToBitPos(endIntPos);	
	
	if (!reader->readBits(endInt, valueSize)) {
		// throw exception
		throw new CodingException("RLEDecoder: Cannot read End Int");
	}	
	reader->skipToBitPos(oldBitPos);
	delete utilityPair;
	utilityPair = new IntValPos(0,(int)endInt);// assuming encoding for all positive values		
	return utilityPair;	

}
unsigned int RLEDecoder::getEndPos() {
	if (!initialized) throw new CodingException("RLEDecoder: Error, decoder not initialized with buffer");
	int oldBitPos=reader->getCurrentBitPosition();
	unsigned int endPos;
	unsigned int endReps;
	
	int endPosPos=reader->getNumBitsInBuffer()-tripleSize+valueSize;	
	reader->skipToBitPos(endPosPos);	
	
	if (!reader->readBits(endPos, startPosSize)) {
		// throw exception
		throw new CodingException("RLEDecoder: Cannot read End StartPos");		
	}	
	if (!reader->readBits(endReps, repsSize)) {
		// throw exception
		throw new CodingException("RLEDecoder: Cannot read End Reps");
		
	}
	reader->skipToBitPos(oldBitPos);	
	return (endPos+endReps-1);		
}
int RLEDecoder::getPageLengthBits() {
	if (!initialized) throw new CodingException("RLEDecoder: Error, decoder not initialized with buffer");	
	return *((int*) (buffer+sizeof(short)));
}
unsigned int RLEDecoder::getSize() {
	if (!initialized) throw new CodingException("RLEDecoder: Error, decoder not initialized with buffer");
	return (getPageLengthBits()-8*(sizeof(short)+sizeof(int)+2))/tripleSize;
}
