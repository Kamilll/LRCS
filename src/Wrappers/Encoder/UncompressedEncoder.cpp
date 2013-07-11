#include "UncompressedEncoder.h"

UncompressedEncoder::UncompressedEncoder(Operator* dataSrc_, int colIndex_, int stringSize, int bfrSizeInBits_) : Encoder(dataSrc_, colIndex_, NULL)
{
  if (dataSrc_==NULL) mode=PUSH;
  currBlock=NULL;
  currVP=NULL;
  init=true;
  writer=new StringWriter(stringSize, bfrSizeInBits_);
  buffer=writer->getBuffer();
  lengthPtr=(int*) buffer;
  startPosPtr=lengthPtr+1;
  ssizePtr=lengthPtr+2;
  *ssizePtr = stringSize;
  writer->skipToIntPos(3);
  mode=UncompressedEncoder::UNINIT;
  //ssize = stringSize;
}

UncompressedEncoder::~UncompressedEncoder()
{
  delete writer;
}

//DNA: this is ugly code fix later
void UncompressedEncoder::resetPair() {
  currVP=NULL;
}

byte* UncompressedEncoder::getPage() {
  Encoder::getPage();	
  if (mode==Encoder::PUSH) {
    init=true;
    writer->resetBuffer();
    return buffer;
  }
  Log::writeToLog("UncompressedEncoder", 0, "Called getPage()");
  while (true) {
    // get the next block
    if (((currBlock==NULL) || (!currBlock->hasNext())) && (currVP == NULL)) {			
      do {
	currBlock=dataSrc->getNextValBlock(colIndex);	
	if (currBlock==NULL) {
	  if (init)	{
	    Log::writeToLog("UncompressedEncoder", 1, "Block was NULL, we dried DataSrc: returning NULL");
	    return NULL; // signal we are really done
	  }
	  else {
	    init=true;
	    currVP=NULL;
	    Log::writeToLog("UncompressedEncoder", 1, "Block was NULL, we dried DataSrc: returning buffer, numInts=",*lengthPtr);
	    return buffer;	// we dried up dataSrc
	  }
	}
	Log::writeToLog("UncompressedEncoder", 0, "Got new block");
      } while (!currBlock->hasNext());
    }
		
    /*		if (!currBlock->isPosContiguous())
		throw new UnexpectedException("UncompressedEncoder: cannot encode a non contiguous column");*/
		
    // write initial value to a blank page
    if (init) {
      writer->resetBuffer();
      writer->skipToIntPos(3);
      if (currVP==NULL) currVP=currBlock->getNext();
      if (currVP==NULL) return NULL;		
      *startPosPtr=currVP->position;	
      //*ssizePtr=ssize;
      if (!writer->writeString((char*)currVP->value)) throw new UnexpectedException("UncompressedEncoder: Could not write initial value");
      *lengthPtr=1;
      init=false;
      currVP=NULL;
    }
    // append more values to same page
    else {
      if (currVP==NULL) currVP=currBlock->getNext();
	
      if (!writer->writeString((char*)currVP->value)) {
	init=true;
	Log::writeToLog("UncompressedEncoder", 1, "Page full, returning page, length",*lengthPtr);
	return buffer;
      }
      else {
	//Log::writeToLog("UncompressedEncoder", 0, "Wrote: value=", currVP->value);
	currVP=NULL;
	*lengthPtr=*lengthPtr+1;
      }
    }
  }
}

bool UncompressedEncoder::writeVal(char* val_, unsigned int pos_)  {
  Encoder::writeVal(0,pos_);
  if (writer->writeString(val_)) {
    if (init) {
      init=false;
      *startPosPtr=pos_;	
      //*ssizePtr = ssize;	
      *lengthPtr=0;			
      writer->resetBuffer();
      if (!writer->skipToIntPos(3)) throw new UnexpectedException("IntEncoder: cannot write int, buffer to small to initialize");		
      if (!writer->writeString(val_)) throw new UnexpectedException("IntEncoder: cannot write int, buffer to small to initialize");		
      Log::writeToLog("IntEncoder", 0, "IntWriter PUSH mode, finished init");
    }
    *lengthPtr=*lengthPtr+1;
    return true;
  }
  else {
    return false;
  }	
}

//Return the exact buffer size
int UncompressedEncoder::getBufferSize()
{
	return (3*sizeof(int) + (*ssizePtr)*(*lengthPtr));
}
