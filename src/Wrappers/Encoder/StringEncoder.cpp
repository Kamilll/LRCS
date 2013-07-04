#include "StringEncoder.h"

StringEncoder::StringEncoder(Operator* dataSrc_, int colIndex_, int stringSize, int bfrSizeInBits_) : Encoder(dataSrc_, colIndex_)
{
	if (dataSrc_==NULL) mode=Encoder::PUSH;
	else mode=Encoder::UNINIT;
	currBlock=NULL;
	currPair=NULL;
	init=true;
	writer=new StringWriter(stringSize, bfrSizeInBits_);
	buffer=writer->getBuffer();
	lengthPtr=(int*) buffer;
	startPosPtr=lengthPtr+1;
	ssizePtr=lengthPtr+2;
	writer->skipToIntPos(3);
}

StringEncoder::~StringEncoder()
{
	delete writer;
}

//DNA: this is ugly code fix later
void StringEncoder::resetPair() {
	currPair=NULL;
}

byte* StringEncoder::getPage() {
	Encoder::getPage();	
	if (mode==Encoder::PUSH) {
		init=true;
		writer->resetBuffer();
		return buffer;
	}
	//Log::writeToLog("StringEncoder", 0, "Called getPage()");
	while (true) {
		// get the next block
		if (((currBlock==NULL) || (!currBlock->hasNext())) && (currPair == NULL)) {			
			do {
				currBlock=dataSrc->getNextValBlock(colIndex);	
				if (currBlock==NULL) {
					if (init)	{
						Log::writeToLog("StringEncoder", 1, "Block was NULL, we dried DataSrc: returning NULL");
						return NULL; // signal we are really done
					}
					else {
						init=true;
						currPair=NULL;
						Log::writeToLog("StringEncoder", 1, "Block was NULL, we dried DataSrc: returning buffer, numInts=",*lengthPtr);
						return buffer;	// we dried up dataSrc
					}
				}
				Log::writeToLog("StringEncoder", 0, "Got new block");
			} while (!currBlock->hasNext());
		}

		/*		if (!currBlock->isPosContiguous())
		throw new UnexpectedException("StringEncoder: cannot encode a non contiguous column");*/

		// write initial value to a blank page
		if (init) {
			writer->resetBuffer();
			writer->skipToIntPos(3);
			if (currPair==NULL) currPair=(StringValPos*)currBlock->getNext();
			if (currPair==NULL) return NULL;		
			*startPosPtr=currPair->position;	
			*ssizePtr = writer->getStringSize();
			if (!writer->writeString(currPair->value)) throw new UnexpectedException("StringEncoder: Could not write initial value");
			*lengthPtr=1;
			init=false;
			currPair=NULL;
		}
		// append more values to same page
		else {
			if (currPair==NULL) currPair=(StringValPos*)currBlock->getNext();

			if (!writer->writeString(currPair->value)) {
				init=true;
				//Log::writeToLog("StringEncoder", 1, "Page full, returning page, length",*lengthPtr);
				return buffer;
			}
			else {
				//Log::writeToLog("StringEncoder", 0, "Wrote: value=", currPair->value);
				currPair=NULL;
				*lengthPtr=*lengthPtr+1;
			}
		}
	}
}
