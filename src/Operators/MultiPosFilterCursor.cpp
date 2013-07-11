#include "MultiPosFilterCursor.h"

MultiPosFilterCursor::MultiPosFilterCursor(MultiPosFilterBlock* MPFB_){
	assert (!MPFB_->isCompleteSet());
	PosFilterBlock* _SPFB;
	_SPFB = MPFB_->getFirstBlock();
	while(_SPFB != NULL){
		posFilterBlockVec.push_back(_SPFB);
		_SPFB = MPFB_->getNextBlock();
	}

	startPos = MPFB_->getStartPosition();
	endPos = MPFB_->getEndPosition();

	//numValues = MPFB_->getNumValues();
	currBlockNum = 0;
	currBlock = NULL;
	currSPFC = NULL;
	filterFinished = false;
}

MultiPosFilterCursor::~MultiPosFilterCursor(){
   if(currSPFC != NULL){
	   delete currSPFC;
	   currSPFC = NULL;
   }
}

unsigned int MultiPosFilterCursor::getNext() {
	unsigned int retPos;
	unsigned int _currBlockNum;
	if (currSPFC == NULL){
		if(setCurrBlock( 0 ) ){//Start from beginning
			currSPFC = (Cursor*)currBlock->getCursor();
			retPos = currSPFC->getNext(); 
		}else retPos = 0;
	}else{
		retPos = currSPFC->getNext();
		if (retPos == 0){ //if run out of a block,iterate to next block
			_currBlockNum = currBlockNum + 1;
			delete currSPFC;
			if(setCurrBlock(_currBlockNum)){				
				currSPFC = (Cursor*)currBlock->getCursor();
				retPos = currSPFC->getNext();
			}else{
				retPos = 0;
				filterFinished = true;
				currSPFC = NULL;
			}//run out of all blocks
		}
	}
	return retPos;
}

bool MultiPosFilterCursor::hasNext() {
	return (getCurrPosition() < endPos);
}

void MultiPosFilterCursor::setCurrStartPosition() {
	if(currSPFC != NULL)currSPFC->setCurrStartPosition();
}

unsigned int MultiPosFilterCursor::getCurrStartPosition() {
	if(currSPFC != NULL)return currSPFC->getCurrStartPosition();
	else return startPos;
}

unsigned int MultiPosFilterCursor::getCurrPosition() {
	if(currSPFC != NULL)return currSPFC->getCurrPosition();
	else return startPos;
}

void MultiPosFilterCursor::resetCursor() {
	filterFinished = false;
	currBlockNum = 0;
	currBlock = NULL;
	currSPFC = NULL;
}

bool MultiPosFilterCursor::isFilterFinished(){
    return filterFinished;
}

bool MultiPosFilterCursor::setCurrBlock( int i ){
	try{
		currBlock = posFilterBlockVec.at(i);
	} catch(const exception& e) {
		e.what();
		return false;
	}
	currBlockNum = i;
	return true;
}