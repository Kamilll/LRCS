#include "BitmapDataSource.h"
#include "../Util/StringUtil.h"
#include "Decoder/LZDecoder.h"
#include "Decoder/BitmapDecoder.h"
#include "../Operators/PosOperator.h"

BitmapDataSource::BitmapDataSource(AM* am_, bool valSorted_, bool isROS, Decoder* decoder_) 
: DataSource(am_, isROS)
{
	valSorted = valSorted_;
	decoder = new LZDecoder(decoder_);    
	//decoder = new StringDecoder(true);
}

 
BitmapDataSource::~BitmapDataSource()
{

	if (decoder!=NULL) delete decoder;
	//if (currBlock!=NULL) delete currBlock;
}

//Get the position block on predicaiton
MultiPosFilterBlock* BitmapDataSource::getPosOnPred(){
	if(pred==NULL){
        matchedPredPos = new MultiPosFilterBlock ();
		matchedPredPos->setCompleteSet(true);
	}else{
		predChanged=false;//Reset predChanged

		ValPos* rhsvp = pred->getRHS();
		char* rhsval = (char*)rhsvp->value;
		ValPos* tempVP = rhsvp->clone();
		int valsize = pred->getRHS()->getSize();
		unsigned char* temp;
		temp = StringUtil::getSmallestLargerValue(rhsval,valsize);
		tempVP->set(temp);
        // The column is for sure not value sorted
		if(!getPosOnPredValueUnsorted((ROSAM*)am, rhsvp, tempVP))
			//If nothing find, matched predication position is a NULL set.
			matchedPredPos = new MultiPosFilterBlock ();        
	}
	return matchedPredPos;
}

//Get position on predition if the value is unsorted:check
bool BitmapDataSource::getPosOnPredValueUnsorted(ROSAM* am_,ValPos* rhsvp_, ValPos* tempVP_){
	byte* page=NULL;
	BitmapDecoder* bitmapDecoder = new BitmapDecoder();
	PosOperator* posOperator = new PosOperator();
	MultiPosFilterBlock* multiPosBlock;
	bool done = false;
	//ValPos* pair;
	
	assert(pred!=NULL);	
    char* rhsval = (char*)rhsvp_->value;
	char* temp = (char*)tempVP_->value;
	
	while(!done){
		//01. get a page from BDB
		switch	(pred->getPredType()) {
			case Predicate::OP_GREATER_THAN: 
				if (firstCall)
					page = (byte*)am_->getDbPageRange(temp);
				else
					page = (byte*)am_->getDbPageNext();
				break;
			case Predicate::OP_GREATER_THAN_OR_EQUAL: 
				if(firstCall)
					page = (byte*)am_->getDbPageRange(rhsval);
				else
					page = (byte*)am_->getDbPageNext();
				break;
			case Predicate::OP_EQUAL: 	
				if(firstCall)
           	      page = (byte*)am_->getDbPageSet(rhsval);
				else
				  page = (byte*)am_->getDbPageNextDup();
				break;
			case Predicate::OP_LESS_THAN:
                page = (byte*)am_->getDbPageNextRange(rhsval,firstCall);
				break;
			case Predicate::OP_LESS_THAN_OR_EQUAL:
                page = (byte*)am_->getDbPageNextRange(temp,firstCall);
				break;
			default:
				break;
		}

		if (page==NULL){
			done = true;
			break;}

		//02. Add OR operator
		if(!firstCall)posOperator->addWhereOp('|'); 
		
		//03. Decode the page
		bitmapDecoder->setBuffer(page);
		PosFilterBlock* posFilterBlock = NULL;

		//04. Check out the position block directly to position filters
		multiPosBlock = new MultiPosFilterBlock ();
		while(bitmapDecoder->hasNextBlock()){
			posFilterBlock = (PosFilterBlock*)bitmapDecoder->getNextBlock();
			multiPosBlock->addPosFilterBlock(posFilterBlock);			
		}

		posOperator->addPosBlock(multiPosBlock);
		firstCall = false;
    }
	posOperator->finishWhereOp();
    matchedPredPos = posOperator->getPosFilter();
	//cout<<matchedPredPos->getNumValuesR()<<endl;
	delete bitmapDecoder;
	delete posOperator;
	
	if(matchedPredPos == NULL)return false;
	else return true;	
}

// Finds the right block on the page
bool BitmapDataSource::skipToRightPosOnPage(unsigned int pos_) {
	if (decoder->skipToPos(pos_))
		return true;
	else
		return false;
}
