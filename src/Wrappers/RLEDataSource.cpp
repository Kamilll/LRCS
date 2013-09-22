#include "RLEDataSource.h"
#include "../Util/StringUtil.h"
#include "Decoder/LZDecoder.h"
#include "Decoder/RLEDecoder.h"
#include "../Operators/PosOperator.h"

RLEDataSource::RLEDataSource(AM* am_, bool valSorted_, bool isROS, Decoder* decoder_) 
: DataSource(am_, isROS)
{
	valSorted = valSorted_;
	decoder = new LZDecoder(decoder_);    
	//decoder = new StringDecoder(true);
}

 
RLEDataSource::~RLEDataSource()
{

	if (decoder!=NULL) delete decoder;
	//if (currBlock!=NULL) delete currBlock;
}

//Get the position block on predicaiton
MultiPosFilterBlock* RLEDataSource::getPosOnPred(){
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
bool RLEDataSource::getPosOnPredValueUnsorted(ROSAM* am_,ValPos* rhsvp_, ValPos* tempVP_){
	byte* page=NULL;
	RLEDecoder* _RLEDecoder = new RLEDecoder(false, true);

	bool done = false;
	
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
		
		//02. Decode the page
		_RLEDecoder->setBuffer(page);

		//03. Check out the position block directly to position filters
		if(matchedPredPos == NULL)
		   matchedPredPos = new MultiPosFilterBlock ();
		RLEBlock* posFilterBlock = (RLEBlock*)_RLEDecoder->getNextBlock();
		matchedPredPos->addRLEBlock(posFilterBlock);			

		firstCall = false;
    }

	delete _RLEDecoder;
	
	if(matchedPredPos == NULL)return false;
	else return true;	
}

// Finds the right block on the page
/*bool RLEDataSource::skipToRightPosOnPage(unsigned int pos_) {
	if (decoder->skipToPos(pos_))
		return true;
	else
		return false;
}*/
