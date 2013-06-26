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
// Uncompressed Int Datasource
// Problems: migfer@mit.edu

#include "StringDataSource.h"

StringDataSource::StringDataSource(AM* am_, bool valSorted_, bool isROS) 
: DataSource(am_, isROS)
{
	valSorted = valSorted_;
	decoder = new StringDecoder(valSorted);
	
	posOutBlock=NULL;
	useWP=false;	                                                                                                              
}

 
StringDataSource::~StringDataSource()
{

	if (decoder!=NULL) delete decoder;
	if (currBlock!=NULL) delete currBlock;
	if (posOutBlock!=NULL) delete posOutBlock;
}


//Get postions directly from DB
bool StringDataSource::getPosBlockDB(ROSAM* am_) {
	byte* page=NULL;
	PosDecoder* posdecoder = new PosDecoder(false);
	Block* b;
	bool done = false;
	int currEndPos = 0;
	ValPos* pair;
	
	assert(pred!=NULL);	
	assert(pred->getRHS()->type == ValPos::STRINGTYPE);
	ValPos* rhsvp = pred->getRHS();
	char* rhsval = (char*)rhsvp->value;
	int valsize = pred->getRHS()->getSize();
	unsigned char* temp = StringUtil::getSmallestLargerValue(rhsval,valsize);
	ValPos* tempVP = new StringValPos (1, valsize);
	tempVP->set(temp);
	predChanged=false;
	
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


// Gets the next position block (bitstring of positions) from the operator
PosBlock* StringDataSource::getNextPosBlock(int colIndex_) {
	if(colIndex_ < 0)return NULL;
	if (valSorted && isROS) {	
		//Log::writeToLog("StringDataSource", 0, "getNextPosBlock(), called on valSorted col");				
		if (!posOutTripleOnPred->isNull()) return NULL;
		if (posOutBlock==NULL) posOutBlock=new PosRLEBlock();

		if (pred!=NULL) {
			// I need to find the first or last postion for a given value, depending on predicate
			// find position where binding on predicate occured
			StringDecoder* decoder=new StringDecoder(valSorted);
			unsigned int position=0;
			unsigned int minPos=1;
			unsigned int maxPos=getLastPosition();
			byte* page=NULL;
			assert(pred->getRHS()->type == ValPos::STRINGTYPE);
			//int rhsval = *(int*)pred->getRHS()->value;
			ValPos* rhsvp = pred->getRHS();
			char* rhsval = (char*)rhsvp->value;
			int valsize = pred->getRHS()->getSize();
			unsigned char* temp = StringUtil::getSmallestLargerValue(rhsval,valsize);
			ValPos* tempVP = new StringValPos (1, valsize);
			tempVP->set(temp);
			
			switch	(pred->getPredType()) {
				case Predicate::OP_GREATER_THAN:
					page=(byte*) skipToPageValue(temp);
					if (page!=NULL) {
						decoder->setBuffer(page);
						if (decoder->skipToBlockOnValue(tempVP))
							position=((MultiSBlock*) getDecodedBlock(decoder))->getPosition();
					}
					else return NULL;
					if (position==0) posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
					if (position==maxPos) return NULL;
					else
						posOutTripleOnPred->setTriple(NULL, position, maxPos-position+1);
					break;
				case Predicate::OP_GREATER_THAN_OR_EQUAL: 	
					page=(byte*) skipToPageValue(rhsval);
					if (page!=NULL) {
						decoder->setBuffer(page);
						if (decoder->skipToBlockOnValue(rhsvp))
							position=((MultiSBlock*) getDecodedBlock(decoder))->getPosition();
					}
					if (position==0) posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
					if (position==maxPos) return NULL;
					else posOutTripleOnPred->setTriple(NULL, position, maxPos-position+1);
					break;
				case Predicate::OP_EQUAL: 	
					page=(byte*) skipToPageValue(rhsval);
					if (page!=NULL) {
						decoder->setBuffer(page);
						if (decoder->skipToBlockOnValue(rhsvp))
							position=((MultiSBlock*) getDecodedBlock(decoder))->getPosition();
					}
					if (position==0) posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
					if (position==maxPos) return NULL;

					 unsigned int end;
					page=(byte*) skipToPageValue(temp);
					if (page!=NULL) {
						decoder->setBuffer(page);
						if (decoder->skipToBlockOnValue(tempVP))
							end=((MultiSBlock*) getDecodedBlock(decoder))->getPosition()-1;
					}
					else
						end=0;
					if (position==0) position=minPos;
					if (end==0) posOutTripleOnPred->setTriple(NULL, position, maxPos-position+1);
					else if (position==maxPos) return NULL;					
					else posOutTripleOnPred->setTriple(NULL, position, end-position+1);
					break;
				case Predicate::OP_LESS_THAN:
					if (position==minPos) return NULL;
					page=(byte*) skipToPageValue(rhsval);
					if (page!=NULL) {
						decoder->setBuffer(page);
						if (decoder->skipToBlockOnValue(rhsvp))
							position=((MultiSBlock*) getDecodedBlock(decoder))->getPosition();
					}
					else posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
					position--;
					if (position<=0) return NULL;
					else posOutTripleOnPred->setTriple(NULL, minPos, position-minPos+1);
					break;
				case Predicate::OP_LESS_THAN_OR_EQUAL:
					//zklee:should be reconsiderated.
					if (position==minPos) return NULL;
					page=(byte*) skipToPageValue(temp);
					if (page!=NULL) {
						decoder->setBuffer(page);
						if (decoder->skipToBlockOnValue(tempVP))
							position=((MultiSBlock*) getDecodedBlock(decoder))->getPosition();
					}
					else posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
					position--;
					if (position<=0) return NULL;
					else posOutTripleOnPred->setTriple(NULL, minPos, position-minPos+1);
					break;
					//break;
				default:
					return NULL;
					break;
			}
			delete decoder;
			delete temp;
			delete tempVP;
			((PosRLEBlock*) posOutBlock)->setTriple(posOutTripleOnPred);
			return posOutBlock;
		}				
		else {
			unsigned int minPos=1;
			unsigned int maxPos=getLastPosition();
			posOutTripleOnPred->setTriple(NULL, minPos, maxPos-minPos+1);
			((PosRLEBlock*) posOutBlock)->setTriple(posOutTripleOnPred);
			cout << "Returning posTriple" << endl;
			return posOutBlock;
		}
	}
	// The column is not value sorted
	else {
		if(currBlockWP->getPosBlockIter()!=NULL)return NULL;
		if( getPosBlockDB(am))
		return currBlockWP->getPosBlockIter();
	}
}

//printColumn, print to stringstream instead. For testing purpose
void StringDataSource::printColumn() {
	using namespace std;
	int count=0;
	//int total=0;
	//char prevVal="xxxxxxxxxx";

	Block* blk=this->getNextValBlock(1);
	while (blk!=NULL) {
		while (blk->hasNext()) {
			ValPos* vp = blk->getNext();
			//count++;
			//cout<<count<<endl;
			cout<<(char*)vp->value<<endl;
			/*char currVal= (char*)vp->value;
			if (prevVal!=currVal)  {
				if (count%10==0) ss  << endl << "Read uniques value: ";
				ss << currVal << ", ";
				count++;
				prevVal=currVal;
		}
		total++;*/
		}
		blk=this->getNextValBlock(0);
	}
	/*  ss << endl;
	 ss << "Total number of values read: " << total << endl;*/
}//printColumn, print to stringstream instead. For testing purpose

