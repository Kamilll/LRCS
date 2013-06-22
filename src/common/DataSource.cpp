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
#include "DataSource.h"
using namespace std;

DataSource::DataSource(AM* am_, bool isROS_) {
	init(am_, isROS_);
	//currBlockWP=NULL;
}

DataSource::DataSource(AM* am_, bool isROS_, bool valSorted_, Decoder* decoder_) {
	init(am_, isROS_);
	valSorted=valSorted_;
	decoder = decoder_;
	currBlock=NULL;
	posOutBlock=NULL;

	useWP=false;	
}

void DataSource::init(AM* am_, bool isROS_) {
	if (am_==NULL) 
	throw new UnexpectedException("DataSource::constructor Error, am is NULL");
	am=am_;
	
	if (!isROS_)
		posPrimaryIndex=false;
	else {
		posPrimaryIndex=true;
	}
	isROS=isROS_;

	am->initCursors();
	posFilterChanged=true;
	predChanged=true;
	predLookup=true;

	posFilterInit=true;
	posFilter=NULL;
	pred=NULL;

	getBlocksCurrPage=NULL;
	getBlocksCurrAM=NULL;
	getBlocksCurrDecoder=NULL;

	currPosBlock=NULL;
	havePosBlock=false;
	posFilterStart=0;
	posFilterEnd=0;
	posSkippedTo=0;	
	getNextNextTime = true;
	RLEAlways=false;
	matchedPred = new Type2Block(false);
	matchedPredPos = new PosType2Block();
	pba=NULL;
	posOutTripleOnPred=new RLETriple();
	currRLETriple=NULL;
	theRLEBlock=NULL;
	currBlockWP=new BlockWithPos();
}

DataSource::~DataSource() {	
	delete posOutTripleOnPred;
	delete currBlockWP;
	delete matchedPred;
	delete matchedPredPos;
	delete pba;
	delete currRLETriple;
	delete theRLEBlock;
}

// Changes the RHS binding for this datasource
void DataSource::changeRHSBinding(ValPos* rhs_) {
	if (pred==NULL) 
		throw new UnexpectedException("DataSource: Error, setting binding on NULL predicate");
	else { 
		pred->setRHS(rhs_);
		predChanged=true;
	}
}

// sets a new predicate for this datasource
void DataSource::setPredicate(Predicate* pred_) {
	predChanged=true;
	pred=pred_;
}		

// Sets a filter for postions
void DataSource::setPositionFilter(Operator* bitstringDataSource_, int colIndx_) {
	posFilterChanged=true;
	posFilter=bitstringDataSource_;
	posColIndex=colIndx_;
}


//getLastPosition returns the last position in the data source (column)
int DataSource::getLastPosition() {
	if (posPrimaryIndex)
		return *(int*)am->getLastIndexValuePrimary();
	else
		return *(int*)am->getLastIndexValueSecondary();
}

const void* DataSource::getNextPageValue() {
	if (posPrimaryIndex)
		return am->getNextPageSecondary();
	else
		return am->getNextPagePrimary();
}

const void* DataSource::skipToPageValue(char* key) {
	if (posPrimaryIndex)
		return am->skipToPageSecondary(key);
	else
		return am->skipToPagePrimary(key);
}

const void* DataSource::getNextPagePosition() {
	if (posPrimaryIndex)
		return am->getNextPagePrimary();
	else
		return am->getNextPageSecondary();
}

const void* DataSource::skipToPagePosition(int key) {
	if (posPrimaryIndex)
		return am->skipToPagePrimary((char*)&key);
	else
		return am->skipToPageSecondary((char*)&key);
}

// checks to make sure that for a given page,
// we have the right block within that page
bool DataSource::checkOutBlock~(bool& done_) {
	if (!posFilter->isNULL()) {
		currBlockWP->setBlock(currBlock);
		useWP=true;
		if(!currBlockWP->filterWithPos(posFilter))
			return false;		
	}

	return true;
}

// checks to make sure that for a given page,
// we have the right block within that page
bool DataSource::checkOutBlock(bool& done_) {
	if ((posFilter!=NULL)) {
		if (isROS) {
			unsigned int pos=currBlock->peekNext()->position;
			unsigned filterStart=posFilterStart;
			unsigned filterEnd=posFilterEnd;
			if (filterEnd==0)
				if (!getPosFilterStartEnd(filterStart, filterEnd, false))
				return false;  

			while (pos>filterEnd) {
				havePosBlock=false;
				if (!getPosFilterStartEnd(filterStart, filterEnd, true)) {
					done_=true;
					return false;
				}else skipToRightPosOnPage(decoder);
			} 


			currBlockWP->setBlock(currBlock);
			useWP=true;
			if (pba) {
				delete pba; 
				pba = NULL;
			}


			if (filterEnd > decoder->getEndPos()) {
				getNextNextTime=false;
				pba = currPosBlock->getIterator(pos, decoder->getEndPos());
				if (!pba->getEndPos())return false;
			}else {
				havePosBlock=false;
				getNextNextTime=true;
				pba = currPosBlock->getIterator(pos, filterEnd);
				if (!pba->getEndPos())return false;
			}
			currBlockWP->setPosBlockIter(pba);
		}
	}
	
	if (pred!=NULL) {
		if (isROS) {
			Block* b;
			if (posFilter==NULL) {
				currBlockWP->setBlock(currBlock);
				useWP=true;
				b = currBlock;
			}
			else
				b = currBlockWP;

			bool x = b->applyPredicate(matchedPred,pred);
			if (!x) return false;
			matchedPredPos->setBlock(matchedPred);
			delete pba;
			pba = matchedPredPos->getIterator();
			if (!pba->getEndPos())
				return false;
			currBlockWP->setPosBlockIter(pba);
			assert (currBlockWP->hasNext());
			return true;
		}
	}
	return true;
}

Block* DataSource::getDecodedBlock(Decoder* decoder) {
    return decoder->getNextBlock();
}

// Gets the next value block from the operator 
Block* DataSource::getNextValBlock~(int colIndex_) {
//zklee: Each page is converted to a multiblock, no loop required anymore!
	if (colIndex_ < 0)return NULL;

	byte* page=getRightPage~(valSorted, !valSorted);
	if (page==NULL) return NULL;
	decoder->setBuffer(page);			
	currBlock=(MultiBlock*) getDecodedBlock(decoder);
	if (currBlock == NULL) return NULL

	if(checkOutBlock(currBlock))
		return (useWP?currBlockWP:currBlock);
	else return NULL
}
// Gets the next value block from the operator 
Block* DataSource::getNextValBlock(int colIndex_) {
	if (colIndex_ < 0)return NULL;
	bool done=false;
	do {
		currBlock=(MultiBlock*) getDecodedBlock(decoder);
		if (currBlock==NULL) {
			while (!decoder->hasNextBlock()) {
				byte* page=getRightPage(decoder, valSorted, !valSorted);
				if (page==NULL) {		
					return NULL;
				}
				decoder->setBuffer(page);
				if (posFilter!=NULL) skipToRightPosOnPage(decoder);			
				currBlock=(MultiBlock*) getDecodedBlock(decoder);
				if (currBlock != NULL)
					break;
			}
		}
	} while (!checkOutBlock(done) && !done);

	if (done) return NULL;
	else {
		return (useWP?currBlockWP:currBlock);
	}
}

byte* DataSource::getRightPage~(Decoder* decoder_, bool valSorted_, bool posIndexPrimary_) {
	Log::writeToLog("DataSource", 0, "Called GetRightPage()");
	// if anything has changed we need to reset the indices
	if (posFilterChanged) {
		am->initCursors();
		posFilterChanged=false;
		posSkippedTo=0;
	}

	if (posFilter->isNULL())return getPage(decoder_);//Sequential Access
	else return getPageOnPos~(posFilter); //Index Access	
}

byte* DataSource::getRightPage(Decoder* decoder_, bool valSorted_, bool posIndexPrimary_) {
	Log::writeToLog("DataSource", 0, "Called GetRightPage()");
	// if anything has changed we need to reset the indices
	if (posFilterChanged || predChanged) {
		Log::writeToLog("Datasource", 0, "Filter or pred changed, initing cursors");
		am->initCursors();
		posFilterChanged=false;
		predChanged=false;
		predLookup=true;
		posSkippedTo=0;
	}

	if ((posFilter==NULL) && (pred==NULL)) {
		if (predChanged) predChanged=false;
		return getPage(decoder_);
	}
	else if ((posFilter==NULL) && (pred!=NULL)) {
		if (predLookup) {
			predLookup=false;
			Log::writeToLog("DataSource", 0, "GetRightPage() calling getPageOnPred()");
			byte* page=getPageOnPred(decoder_, valSorted_);
			if (page!=NULL) {
				decoder_->setBuffer(page);
			}
			return page;
		}
		else {
			Log::writeToLog("DataSource", 0, "GetRightPage() calling getPage()");
			return (byte*) getNextPageValue();
		}
	}
	else if ((posFilter!=NULL) && (pred==NULL)) {
		Log::writeToLog("DataSource", 0, "GetRightPage() calling getPageOnPos()");		
		return getPageOnPos(decoder_, posIndexPrimary_);
	}	
}

byte* DataSource::getPage(Decoder* decoder_) {
	return (byte*) getNextPageValue();
}


byte* DataSource::getPageOnPred(Decoder* decoder_, bool valSorted_) {
	if (valSorted_) {
		Log::writeToLog("DataSource", 0, "getPageOnPred(), valSorted");		

		byte* value;
		if (pred!=NULL) {
			predChanged=false; 	
		    ValPos* vp = pred->getRHS();
			value=(char*)vp->value;
		}
		byte* page=((byte*) skipToPageValue(value));
		return page;
	}
	else {
		 //return (byte*) getNextPageValue();
	}
}

byte* DataSource::getPageOnPos(Decoder* decoder_, bool posIndexPrimary_) {
	// Idea: simple, for each call to get a page we find the page containing this position
	unsigned int position;
	unsigned int filterEnd;
	
	if (isROS) {
	  if (!getPosFilterStartEnd(position, filterEnd, getNextNextTime))	
	    return NULL;	
	} else {
	  if (!getPosFilterStartEnd(position, filterEnd, true))	
	    return NULL;
	}
	  
	byte* page;
	// now to get the page
	// if on same position filter block, then just walk down index
	if (!getNextNextTime || posSkippedTo==position) {
		page=(byte*) getNextPagePosition();
	}
	else {
	  if (position != filterEnd) posSkippedTo=position;
		am->initCursors();
		page=(byte*) skipToPagePosition(position);
	}
	return page;	
}

byte* DataSource::getPageOnPos~(PosBitMap* posFilter_) {
	unsigned int filterStart;
	unsigned int filterEnd;
	unsigned int pageEnd;

	byte* page;
	if (!filterFinished){
		filterStart = posFilter_->getStartPos();
		filterEnd = posFilter_->getEndPos ();
		am->initCursors();
		page=(byte*) skipToPagePosition(filterStart);
		decoder->setBuffer(page);
		pageEnd = decoder->getEndPos ();
		if(pageEnd>filterEnd) filterFinished = true;
		else posFilter_->setCurrStartPos(pageEnd);
		return page;
	}else return NULL;
}

// Gets the range currently encoded in the position filter
bool DataSource::getPosFilterStartEnd(unsigned int& posFilterStart_, unsigned int& posFilterEnd_, bool getNextFilterRange_) {
	// Okay, do we have a range, are we being asked for next range, otherwise just return old range
	if (getNextFilterRange_ || (posFilterEnd==0)) {
		// check to see if we have a position block
	  if ((!havePosBlock) || (!currPosBlock->hasNext())) {
			getNextPosFilterBlock();
			havePosBlock=true;
			if (currPosBlock==NULL) {
			  havePosBlock=false;
			  return false;
			}
		}

		// now three cases for position blocks	
		// 1. Position contiguous
		// 2. Not Position contiguous
		if (posFilterCurrBlockPosContig) {
			// All we need is start and end
			posFilterStart=currPosBlock->getStartPos();
			posFilterEnd=currPosBlock->getStartPos()+currPosBlock->getSize()-1;		
			havePosBlock=false;
		}
		else {
			posFilterStart=currPosBlock->getStartPos();
			posFilterEnd=((PosType2Block*)currPosBlock)->getEndPos();
			}
	}
	posFilterStart_=posFilterStart;
	posFilterEnd_=posFilterEnd;
	return (currPosBlock!=NULL);
}

// Finds the right block on the page
bool DataSource::skipToRightPosOnPage~(unsigned int pos_) {
	if (decoder->skipToPos(pos_))
		return true;
	else
		return false;
}

// Finds the right block on the page
bool DataSource::skipToRightPosOnPage(Decoder* decoder_) {
	assert(decoder_!=NULL);
	unsigned int start=decoder_->getStartPos();	
	
	unsigned int filterStart;
	unsigned int filterEnd;
	if (!getPosFilterStartEnd(filterStart, filterEnd,false))
		return false;
	
	if (start>=filterStart) 
		return true;
	else  {
	  // assumes that positions are contiguous
		Log::writeToLog("Datasource", 0, "Skipping to loc", filterStart-start-1);
		if (decoder_->skipToPos(filterStart-start))
			return true;
		else
			return false;
	}
}

// Gets the next position block from the filter, used internally by
// getPosFilterStartEnd()
void DataSource::getNextPosFilterBlock() {	
	if (posFilter==NULL)
		currPosBlock=NULL;
	else {
		currPosBlock=posFilter->getNextPosBlock(posColIndex);
		if (currPosBlock==NULL) {
			Log::writeToLog("DataSource", 0, "GetNextPosFilterBlock() returned NULL");
			return; 
		}
		int psize = currPosBlock->getSize();
		if ((psize>1) && (psize==currPosBlock->getNumOcurrences()))
		  RLEAlways=true;
		posFilterPosSorted=currPosBlock->isPosSorted();
		posFilterCurrBlockPosSorted=currPosBlock->isBlockPosSorted();
		posFilterCurrBlockPosContig=currPosBlock->isPosContiguous();
		//cerr << "Got next posFilterBlock; sz = " << currPosBlock->getSize() << endl;
	}
} 

//Get the position block on predicaiton
PosBlock* DataSource::getPosOnPred(){
	predChanged=false;//Reset predChanged

	ValPos* rhsvp = pred->getRHS();
	char* rhsval = (char*)rhsvp->value;
	ValPos* tempVP = rhsvp->clone();
	int valsize = pred->getRHS()->getSize();
	unsigned char* temp;
	if (pred->getRHS()->type == ValPos::STRINGTYPE){
		temp = StringUtil::getSmallestLargerValue(rhsval,valsize);
		tempVP->set(temp);
	}else{//To be implemented
        tempVP->set(1);
	}
	
	if (valSorted) {			
		if (pred!=NULL) {		
			if (getPosOnPredValueSorted(rhsvp, tempVP))				
				 return posOutBlock;
			else return NULL;
		}else {//No predication, return all the positions
			unsigned int minPos=1;
			unsigned int maxPos=getLastPosition();
			posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
			((PosRLEBlock*) posOutBlock)->setTriple(posOutTripleOnPred);
			return posOutBlock;
		}
	}else {// The column is not value sorted
		if(currBlockWP->getPosBlockIter()!=NULL)return NULL;
		if( getPosOnPredValueUnsorted(am, rhsvp, tempVP))
		return currBlockWP->getPosBlockIter();
	}	
}

//Get postion on predition if the value is sorted
bool DataSource::getPosOnPredValueSorted(ValPos* rhsvp_, ValPos* tempVP_){

	if (!posOutTripleOnPred->isNull()) return false;
	if (posOutBlock==NULL) posOutBlock=new PosRLEBlock();
	byte* page = NULL;
	unsigned int position=0;
	unsigned int minPos=1;
	unsigned int maxPos=getLastPosition();
	
	assert((rhsvp_ != NULL) && (tempVP_!= NULL));
	char* rhsval = (char*)rhsvp_->value;
	char* temp = (char*)tempVP_->value;

	StringDecoder* decoder=new StringDecoder(true);
	switch	(pred->getPredType()) {
		case Predicate::OP_GREATER_THAN:
			page=(byte*) skipToPageValue(temp);
			if (page!=NULL) {
				decoder->setBuffer(page);
				if (decoder->skipToBlockOnValue(tempVP_))
					position=((MultiSBlock*) getDecodedBlock(decoder))->getPosition();
			}else return false;
			if (position==0) posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
			if (position==maxPos)return false;
			else
				posOutTripleOnPred->setTriple(NULL, position, maxPos-position+1);
			break;
		case Predicate::OP_GREATER_THAN_OR_EQUAL: 	
			page=(byte*) skipToPageValue(rhsval);
			if (page!=NULL) {
				decoder->setBuffer(page);
				if (decoder->skipToBlockOnValue(rhsvp_))
					position=((MultiSBlock*) getDecodedBlock(decoder))->getPosition();
			}
			if (position==0) posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
			if (position==maxPos)return false;
			else posOutTripleOnPred->setTriple(NULL, position, maxPos-position+1);
			break;
		case Predicate::OP_EQUAL: 	
			page=(byte*) skipToPageValue(rhsval);
			if (page!=NULL) {
				decoder->setBuffer(page);
				if (decoder->skipToBlockOnValue(rhsvp_))
					position=((MultiSBlock*) getDecodedBlock(decoder))->getPosition();
			}
			if (position==0) posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
			if (position==maxPos)return false;
			unsigned int end;
			page=(byte*) skipToPageValue(temp);
			if (page!=NULL) {
				decoder->setBuffer(page);
				if (decoder->skipToBlockOnValue(tempVP_))
					end=((MultiSBlock*) getDecodedBlock(decoder))->getPosition()-1;
			}
			else
				end=0;
			if (position==0) position=minPos;
			if (end==0) posOutTripleOnPred->setTriple(NULL, position, maxPos-position+1);
			else if (position==maxPos)return false;				
			else posOutTripleOnPred->setTriple(NULL, position, end-position+1);
			break;
		case Predicate::OP_LESS_THAN:
			if (position==minPos) return false;
			page=(byte*) skipToPageValue(rhsval);
			if (page!=NULL) {
				decoder->setBuffer(page);
				if (decoder->skipToBlockOnValue(rhsvp_))
					position=((MultiSBlock*) getDecodedBlock(decoder))->getPosition();
			}
			else posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
			position--;
			if (position<=0) return false;
			else posOutTripleOnPred->setTriple(NULL, minPos, position-minPos+1);
			break;
		case Predicate::OP_LESS_THAN_OR_EQUAL:
			//zklee:should be reconsiderated.
			if (position==minPos)return false;
			page=(byte*) skipToPageValue(temp);
			if (page!=NULL) {
				decoder->setBuffer(page);
				if (decoder->skipToBlockOnValue(tempVP_))
					position=((MultiSBlock*) getDecodedBlock(decoder))->getPosition();
			}
			else posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
			position--;
			if (position<=0)return false;
			else posOutTripleOnPred->setTriple(NULL, minPos, position-minPos+1);
			break;
		default:
			 return false;
			break;
	}	
    ((PosRLEBlock*) posOutBlock)->setTriple(posOutTripleOnPred);
	return true;
}

//Get postion on predition if the value is unsorted
bool DataSource::getPosOnPredValueUnsorted(ROSAM* am_,ValPos* rhsvp_, ValPos* tempVP_){
	byte* page=NULL;
	PosDecoder* posdecoder = new PosDecoder(false);
	Block* b;
	bool done = false;
	int currEndPos = 0;
	ValPos* pair;
	
	assert(pred!=NULL);	
    char* rhsval = (char*)rhsvp_->value;
	char* temp = (char*)tempVP_->value;
	
	while(!done){
		//01. get a page from BDB
		switch	(pred->getPredType()) {
			case Predicate::OP_GREATER_THAN: 
				if (firstCall)
					page = (byte*)am_->getDbSetRange(temp);
				else
					page = (byte*)am_->getDbNext();
				break;
			case Predicate::OP_GREATER_THAN_OR_EQUAL: 
				if(firstCall)
					page = (byte*)am_->getDbSetRange(rhsval);
				else
					page = (byte*)am_->getDbNext();
				break;
			case Predicate::OP_EQUAL: 	
           	    page = (byte*)am_->getDbSet(rhsval);
                done = true;  
				break;
			case Predicate::OP_LESS_THAN:
                page = (byte*)am_->getDbNextRange(rhsval,firstCall);
				break;
			case Predicate::OP_LESS_THAN_OR_EQUAL:
                page = (byte*)am_->getDbNextRange(temp,firstCall);
				break;
			default:
				break;
		}

		if (page==NULL){
			done = true;
			break;}

		//02. Decode the page
		posdecoder->setBuffer(page);

		//03. Get the blocks
		MultiBlock* currBlockPos = (MultiBlock*) getDecodedBlock(posdecoder);

		//04. Check current position block to escape the loop
		if(currBlockPos == NULL){
			done = true;
			break;}

		//05.Get the block
		currBlockWP->setBlock(currBlockPos);
		useWP=true;
		b = currBlockPos;

		//06. initialize the buffer to hold positions	
		if (firstCall){
			pair=b->getNext();	
			matchedPred->initEmptyBuffer(*(int*)pair->value, pair);
			matchedPred->addPosition(*(int*)pair->value);
		    currEndPos = *(int*)pair->value;}

		//07. get the pairs one by one
		while(b->hasNext()){
			pair=b->getNext();	
			matchedPred->addPosition(*(int*)pair->value);
			currEndPos = *(int*)pair->value;
		}
		firstCall = false;
    }

	//08. finish the buffer editing
	if(currEndPos > 0)
	matchedPred->finishBufferEditing(currEndPos);
	else
	return false;

	//09. get the position iterator
	matchedPredPos->setBlock(matchedPred);
	pba = matchedPredPos->getIterator();
	if (!pba->getEndPos()) return false;
	currBlockWP->setPosBlockIter(pba);
	assert (currBlockWP->hasNext());
	return true;	
}