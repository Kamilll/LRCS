#include "StringEncoder.h"
#include "PosEncoder.h"

StringEncoder::StringEncoder(Operator* dataSrc_, int colIndex_,PosEncoder* posEncoder_,int stringSize, int bfrSizeInBits_) : Encoder(dataSrc_, colIndex_, posEncoder_)
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

	while (true) {
		// get the next block
		if (((currBlock==NULL) || (!currBlock->hasNext())) && (currPair == NULL)) {			
			do {
				currBlock=dataSrc->getNextValBlock(colIndex);	
				if (currBlock==NULL) {
					if (init)	{
						return NULL; // signal we are really done
					}
					else {
						init=true;
						currPair=NULL;
						if(posEncoder != NULL)posEncoder->purgeMap2Queue();
						return buffer;	// we dried up dataSrc
					}
				}
			} while (!currBlock->hasNext());
		}

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
			//currPair=NULL;	
		}
		// append more values to same page
		else {
			if (currPair==NULL) currPair=(StringValPos*)currBlock->getNext();

			if (!writer->writeString(currPair->value)) {
				init=true;
				return buffer;
			}
			else {
				//currPair=NULL;
				*lengthPtr=*lengthPtr+1;
			}
		}
		if(posEncoder != NULL)posEncoder->addValPos(currPair);
		currPair=NULL;
	}

	if(posEncoder != NULL)posEncoder->purgeMap2Queue();
}

/*byte* StringEncoder::getEncodedPosPage(byte** posValue_, unsigned int** posPageSize_){
	assert(posEncoder != NULL);
	//*posValue_ = new byte[*ssizePtr];
	byte* _posValue = NULL;
	byte* _page = posEncoder->getPageAndValue(&_posValue);
	*posValue_ = _posValue;
	if(_page != NULL){
		unsigned int pageSize = BLOCK_SIZE*(*(unsigned int*)_page);
		*posPageSize_ = &pageSize;
		return _page;
	}else return NULL;
}*/

//Return the exact buffer size
int StringEncoder::getBufferSize()
{
	return (3*sizeof(int) + (*ssizePtr)*(*lengthPtr));
}