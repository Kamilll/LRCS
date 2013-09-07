#include "LZDataSource.h"
#include "../Util/StringUtil.h"
#include "Decoder/LZDecoder.h"

LZDataSource::LZDataSource(AM* am_, bool valSorted_, bool isROS, Decoder* decoder_) 
: DataSource(am_, isROS)
{
	valSorted = valSorted_;
	decoder = new LZDecoder(decoder_);                                                                                                            
}

 
LZDataSource::~LZDataSource()
{

	if (decoder!=NULL) delete decoder;
	if (currBlock!=NULL) delete currBlock;
}

//Get the position block on predicaiton
MultiPosFilterBlock* LZDataSource::getPosOnPred(){
	matchedPredPos = new MultiPosFilterBlock ();
	if(pred==NULL)matchedPredPos->setCompleteSet(true);
	else{
		predChanged=false;//Reset predChanged

		ValPos* rhsvp = pred->getRHS();
		char* rhsval = (char*)rhsvp->value;
		ValPos* tempVP = rhsvp->clone();
		int valsize = pred->getRHS()->getSize();
		unsigned char* temp;
		temp = StringUtil::getSmallestLargerValue(rhsval,valsize);
		tempVP->set(temp);

		if (valSorted) {					
			getPosOnPredValueSorted(rhsvp, tempVP);
		}else {// The column is not value sorted
			getPosOnPredValueUnsorted((ROSAM*)am, rhsvp, tempVP);
		}	
	}
	return matchedPredPos;
}

//printColumn, print to stringstream instead. For testing purpose
void LZDataSource::printColumn() {
	using namespace std;
	int count=0;

	Block* blk=this->getNextValBlock(1);
	while (blk!=NULL) {
		while (blk->hasNext()) {
			ValPos* vp = blk->getNext();
			cout<<(char*)vp->value<<endl;
		}
		blk=this->getNextValBlock(0);
	}
}//printColumn, print to stringstream instead. For testing purpose

