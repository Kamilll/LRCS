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
	matchedPredPos = new MultiPosFilterBlock ();
	posOutTripleOnPred=new RLETriple();
}

DataSource::~DataSource() {	
	delete posOutTripleOnPred;
	delete matchedPredPos;
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
void DataSource::setPositionFilter(MultiPosFilterBlock* bitstringDataSource_, int colIndx_) {
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

Block* DataSource::getDecodedBlock() {
    return decoder->getNextBlock();
}

// Gets the next value block from the operator 
Block* DataSource::getNextValBlock(int colIndex_) {
//zklee: Each page is converted to a multiblock, no loop required anymore!
	if (colIndex_ < 0)return NULL;
	if(posFilter->isFilterFinished())return NULL;

	byte* page=getRightPage( );
	if (page==NULL) return NULL;
	decoder->setBuffer(page);
	unsigned int currStartPos = posFilter->getCurrStartPosition();
	skipToRightPosOnPage(currStartPos);
	currBlock=(MultiBlock*) getDecodedBlock();
	if (currBlock == NULL) return NULL;

	currBlock->filterWithPos(posFilter);

	return currBlock;
}

byte* DataSource::getRightPage() {
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

	if(posFilter != NULL)
	  if(posFilter->isCompleteSet())
		return getPage();//Sequential Reading
	  else if(posFilter->isNullSet())
		return NULL;
      else
	    return getPageOnPos();
	else
	  return getPage();
}

byte* DataSource::getPage() {
	return (byte*) getNextPageValue();
}

//zklee: This method could be deleted somehow!
byte* DataSource::getPageOnPred(bool valSorted_) {
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
	else {//Sequential Reading
	   return getPage();
	}
}

byte* DataSource::getPageOnPos() {
	unsigned int filterStart;
	unsigned int filterEnd;
	unsigned int pageEnd;

	assert(posFilter != NULL);
	byte* page;
	if (!filterFinished){
		filterStart = posFilter->getCurrStartPosition();
		filterEnd = posFilter->getEndPosition();
		am->initCursors();
		page=(byte*) skipToPagePosition(filterStart);
		decoder->setBuffer(page);
		pageEnd = decoder->getEndPos();
		if(pageEnd>filterEnd) filterFinished = true;
		return page;
	}else return NULL;
}

// Finds the right block on the page
bool DataSource::skipToRightPosOnPage(unsigned int pos_) {
	if (decoder->skipToPos(pos_))
		return true;
	else
		return false;
}

//Get the position block on predicaiton
MultiPosFilterBlock* DataSource::getPosOnPred(){
	if(pred==NULL)matchedPredPos->setCompleteSet(true);
	else{
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
			getPosOnPredValueSorted(rhsvp, tempVP);
		}else {// The column is not value sorted
			getPosOnPredValueUnsorted(am, rhsvp, tempVP);
		}	
	}
	return matchedPredPos;
}
//Get postion on predition if the value is sorted:Check
bool DataSource::getPosOnPredValueSorted(ValPos* rhsvp_, ValPos* tempVP_){

	if (!posOutTripleOnPred->isNull()) return false;
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
					position=((MultiBlock*) getDecodedBlock())->getPosition();
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
					position=((MultiBlock*) getDecodedBlock())->getPosition();
			}
			if (position==0) matchedPredPos->setCompleteSet(true);
			if (position==maxPos)return false;
			else posOutTripleOnPred->setTriple(NULL, position, maxPos-position+1);
			break;
		case Predicate::OP_EQUAL: 	
			page=(byte*) skipToPageValue(rhsval);
			if (page!=NULL) {
				decoder->setBuffer(page);
				if (decoder->skipToBlockOnValue(rhsvp_))
					position=((MultiBlock*) getDecodedBlock())->getPosition();
			}
			if (position==0)return false;
			unsigned int end;
			page=(byte*) skipToPageValue(temp);
			if (page!=NULL) {
				decoder->setBuffer(page);
				if (decoder->skipToBlockOnValue(tempVP_))
					end=((MultiBlock*) getDecodedBlock())->getPosition()-1;
			}
			else end=0;

			if (end==0) posOutTripleOnPred->setTriple(NULL, position, maxPos-position+1);			
			else posOutTripleOnPred->setTriple(NULL, position, end-position+1);
			break;
		case Predicate::OP_LESS_THAN:
			if (position==minPos) return false;
			page=(byte*) skipToPageValue(rhsval);
			if (page!=NULL) {
				decoder->setBuffer(page);
				if (decoder->skipToBlockOnValue(rhsvp_))
					position=((MultiBlock*) getDecodedBlock())->getPosition();
			}
			else ;
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
					position=((MultiBlock*) getDecodedBlock())->getPosition();
			}
			else matchedPredPos->setCompleteSet(true);
			position--;
			if (position<=0)return false;
			else posOutTripleOnPred->setTriple(NULL, minPos, position-minPos+1);
			break;
		default:
			 return false;
			break;
	}
	if(!matchedPredPos->isCompleteSet())
       matchedPredPos->setTriple(posOutTripleOnPred);
	return true;
}

//Get position on predition if the value is unsorted:check
bool DataSource::getPosOnPredValueUnsorted(ROSAM* am_,ValPos* rhsvp_, ValPos* tempVP_){
	byte* page=NULL;
	PosDecoder* posdecoder = new PosDecoder(false);
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
		MultiBlock* currBlockPos = (MultiBlock*)posdecoder->getNextBlock();

		//04. Check current position block to escape the loop
		if(currBlockPos == NULL){
			done = true;
			break;}

		//05. get the pairs one by one
		while(currBlockPos->hasNext()){
			pair=currBlockPos->getNext();	
			matchedPredPos->addPosition(*(int*)pair->value);
		}
		firstCall = false;
    }

	//06. Check whether matchedPredPos is empty or not
	if(matchedPredPos->isNullSet())return false;
	else return true;	
}