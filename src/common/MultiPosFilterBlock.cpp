#include "MultiPosFilterBlock.h"
#include "../Operators/MultiPosFilterCursor.h"

MultiPosFilterBlock::MultiPosFilterBlock( ){
   init();
}

void MultiPosFilterBlock::init( ){
	startPos = 0;
	endPos = 0;
	currBlock = NULL;
	currBlockNum = 0;
	numValues = 0;
	isBufferSet = false;
	setCompleteSet(false);
	//setFilterFinished(false);
}

MultiPosFilterBlock::~MultiPosFilterBlock( ){
	//delete posFilterBlockVec;
	if(!isBufferSet)
	for(int i = 0; i<getNumBlocks();i++)
        delete posFilterBlockVec.at(i);
}

MultiPosFilterBlock* MultiPosFilterBlock::clone( ){
	//delete posFilterBlockVec;
	MultiPosFilterBlock* newBlock = new MultiPosFilterBlock();
	if(isCompleteSet ()){
		newBlock->setCompleteSet(true);
	}else if (!isNullSet()){
		for(int i = 0; i<getNumBlocks();i++)
			newBlock->addPosFilterBlock(posFilterBlockVec.at(i)->clone());	
		newBlock->setCurrBlock(0);
	}
	return newBlock;
}

void MultiPosFilterBlock::addPosFilterBlock( PosFilterBlock* posFilterBlock_ ){
	assert(posFilterBlock_ != NULL);
	if(startPos == 0){//add the first block
		startPos = posFilterBlock_->getStartPosition();	
		endPos = posFilterBlock_->getEndPosition();
		posFilterBlockVec.push_back(posFilterBlock_);
		numValues += posFilterBlock_->getNumValues();
		posFilterBlock_->parentMultiBlock = this;
		return;
	}

	if(posFilterBlock_->getEndPosition() < startPos){//insert a new block at beginning
        startPos = posFilterBlock_->getStartPosition();
		posFilterBlockVec.insert(posFilterBlockVec.begin(),posFilterBlock_);
		numValues += posFilterBlock_->getNumValues();
	}else if(posFilterBlock_->getEndPosition() > startPos && posFilterBlock_->getEndPosition() < endPos){
		//insert a new block at middle
		posFilterBlockVec.insert((posFilterBlockVec.begin()+currBlockNum),posFilterBlock_);
		numValues += posFilterBlock_->getNumValues();
	}else if(posFilterBlock_->getStartPosition() > endPos){//append a new block
		endPos = posFilterBlock_->getEndPosition();
		posFilterBlockVec.push_back(posFilterBlock_);
		numValues += posFilterBlock_->getNumValues();
	}
	posFilterBlock_->parentMultiBlock = this;
}

void MultiPosFilterBlock::addPosition(unsigned int pos){
	if(posFilterBlockVec.empty()){ //add the first block
		currBlock = new PosFilterBlock();
		currBlock->initEmptyBuffer(pos);
		addPosFilterBlock(currBlock);
		return;
	}

	if(pos < startPos){
		setCurrBlock(0); //Move to the first block
		if(!currBlock->addPosition(pos)){//Try to add the position in the first block
			/*if failed to add the poistion in the first block, create a new block*/
			currBlock = new PosFilterBlock();
			currBlock->initEmptyBuffer(pos);
			addPosFilterBlock(currBlock);
		}
	}else if(pos > startPos && pos < endPos){
		for(int i = 0; i<getNumBlocks();i++){
			setCurrBlock(i);
			if(pos < currBlock->getStartPosition()){
				if(currBlock->addPosition(pos))
					numValues++; //Try to add the position in the block
				else
					/*if failed to add the poistion, try to add it to previous block*/
					if(posFilterBlockVec.at(i-1)->addPosition(pos))
						numValues++;
					else{
						/*if failed to add the poistion in the block, create a new one*/
						currBlock = new PosFilterBlock();
						currBlock->initEmptyBuffer(pos);
						addPosFilterBlock(currBlock);
					}

			}else if(pos > currBlock->getStartPosition() && pos < currBlock->getEndPosition()){
				currBlock->addPosition(pos);//I think this situation is alway true
				numValues++; 
			}else continue;
			break;
		}
	}else if (pos > endPos){
		setCurrBlock((getNumBlocks()-1)); //Move to the last block
		if(currBlock->addPosition(pos)){//Try to add the position in the last block
			numValues++; 
			endPos = currBlock->getEndPosition();
		}else {
			/*if failed to add the poistion in the last block, create a new block*/
			currBlock = new PosFilterBlock();
			currBlock->initEmptyBuffer(pos);
			addPosFilterBlock(currBlock);
		}
	}
}

void MultiPosFilterBlock::addInt(unsigned int int_){
	assert(!posFilterBlockVec.empty());

	if(!currBlock->addInt(int_)){ //Block is full, create a new one
		currBlock = new PosFilterBlock();
		currBlock->initEmptyBuffer(endPos+1);
		currBlock->setCurrInt(0);
		currBlock->addInt(int_);//The first int, always true
		addPosFilterBlock(currBlock);		
	}
    endPos = currBlock->getEndPosition();
}

void MultiPosFilterBlock::optimize( ){
	
	int i = 0;
	bool prevFull;
	PosFilterBlock* currPosFilterBlock;
	PosFilterBlock* prevPosFilterBlock;
	while(true){		
		try{
			currPosFilterBlock = posFilterBlockVec.at(i);
		} catch(const exception& e) {
			e.what();
			break;
		}

		//01. remove all the preceding Zeros
		if(!removePrecedingZero(currPosFilterBlock)){//The whole block is zero,erase it directly
            posFilterBlockVec.erase(posFilterBlockVec.begin() + i);
			continue;
		}

		//02. try to merge the block into previous block
		if(i == 0){//Incase the first block, there is no previous block
			i++;
			startPos = currPosFilterBlock->getStartPosition();
			continue;
		}
		prevPosFilterBlock = posFilterBlockVec.at(i-1);
		currPosFilterBlock->setCurrInt(1);
		if(prevPosFilterBlock->addPosition(currPosFilterBlock->getNext())){
			//Try to add the start position to the previous block. If success, then try to add next position.
			prevFull = false;
			while(currPosFilterBlock != NULL){
				while(currPosFilterBlock->hasNext()){
					if(!(prevPosFilterBlock->addPosition(currPosFilterBlock->getNext()))){
						currPosFilterBlock->cutGetRightAtPos(currPosFilterBlock->getCurrPosition());
						prevFull = true;
						break;
					}
				}

				//run out of current block, delete the block and get next block;
				if(!prevFull){
					posFilterBlockVec.erase(posFilterBlockVec.begin() + i);
					break;
				}else{
					i++;
					break;
				}
			}
		}else
			i++;
	}
}

bool MultiPosFilterBlock::removePrecedingZero(PosFilterBlock* &posFilterBlock_){
	assert(posFilterBlock_ != NULL);
	unsigned int _currInt;
	posFilterBlock_->setCurrInt(0);
	while(posFilterBlock_->hasNextInt()){
		_currInt = posFilterBlock_->getNextInt();
		if( _currInt == 0)continue;
		else{	 
			posFilterBlock_->cutGetRightAtPos(posFilterBlock_->getCurrIntRealStartPos());
			return true;
		}
	}
	return false;//The whole block is zero
}


void MultiPosFilterBlock::resetBlock( ){
	for(int i = 0; i<getNumBlocks();i++)
		delete posFilterBlockVec.at(i);
	posFilterBlockVec.clear();
	init();
}

bool MultiPosFilterBlock::setCurrBlock( int i ){
	try{
		currBlock = posFilterBlockVec.at(i);
	} catch(const exception& e) {
		e.what();
		return false;
	}
	currBlockNum = i;
    currBlock->setCurrInt(1);
	return true;
}

bool MultiPosFilterBlock::hasNext( ){
	if(currBlock == NULL)return true;//Start from beginning
	else return (currBlock->getCurrPosition() < endPos);
}

unsigned int MultiPosFilterBlock::getNext( ){
	unsigned int retPos;
	unsigned int _currBlockNum;
	if (currBlock == NULL){
		if(setCurrBlock( 0 ) )
			retPos = currBlock->getNext(); //Start from beginning
	}else{
		retPos = currBlock->getNext();
		if (retPos == 0){ //if run out of a block,iterate to next block
			_currBlockNum = currBlockNum + 1;
			if(setCurrBlock(_currBlockNum))
				retPos = currBlock->getNext();
			else{
				retPos = 0;
				setCurrBlock(0);
			}//run out of all blocks
		}
	}
	return retPos;
}

PosFilterBlock* MultiPosFilterBlock::getNextBlock( ){
	unsigned int _currBlockNum;
	if (currBlock == NULL){
		if(setCurrBlock( 0 ) )
			return currBlock; //Start from the first block
	}else{
		_currBlockNum = currBlockNum + 1;
		if(setCurrBlock(_currBlockNum))
			return currBlock;
		else
			return NULL;//run out of all blocks
	}
}

PosFilterBlock* MultiPosFilterBlock::getFirstBlock( ){
	if(setCurrBlock( 0 ) )
		return currBlock; 
}

unsigned int MultiPosFilterBlock::getStartPosition() {
  return startPos;
}


unsigned int MultiPosFilterBlock::getEndPosition() {
   return endPos;
}

unsigned int MultiPosFilterBlock::getNumValues() {
   return numValues;
}
unsigned int MultiPosFilterBlock::getNumValuesR() {
  numValues = 0;
  for(int i = 0; i<getNumBlocks();i++)
	  numValues +=posFilterBlockVec.at(i)->getNumValuesR();
  return numValues;
}

PosFilterBlock* MultiPosFilterBlock::getCurrBlock() {
   return currBlock;
}

unsigned int MultiPosFilterBlock::getCurrPosition() {
   assert(currBlock != NULL);
   return currBlock->getCurrPosition();
}

unsigned int MultiPosFilterBlock::getNumBlocks() {
	return (posFilterBlockVec.empty()?0:posFilterBlockVec.size());
}

unsigned int MultiPosFilterBlock::getCurrStartPosition() {
   assert(currBlock != NULL);
   return currBlock->getCurrStartPosition();
}

MultiPosFilterCursor* MultiPosFilterBlock::getCursor(){
   MultiPosFilterCursor* newCursor = new MultiPosFilterCursor(this);
   return newCursor;
}

void MultiPosFilterBlock::setCurrStartPosition( ) {
/*Record the current position to the current start position.
The purpose is to continue the filtering at pervious break point
*/
	assert(currBlock != NULL);
	currBlock->setCurrStartPosition( );
}

void MultiPosFilterBlock::printBlocks( ) {
	cout<<"#startPos:" <<startPos<<", "
		<<"#endPos:" <<endPos<<", "
		<<"#numValues:" <<numValues<<endl;
  for(int i = 0; i<getNumBlocks();i++)
	posFilterBlockVec.at(i)->printBlock();
}

bool MultiPosFilterBlock::isNullSet( ){
  return posFilterBlockVec.empty();
}

bool MultiPosFilterBlock::isCompleteSet( ){
   return completeSet;
}

void MultiPosFilterBlock::setCompleteSet(bool flag_){
  completeSet=flag_;
}

void MultiPosFilterBlock::setTriple(RLETriple* triple_){
	assert(isNullSet());//Current multi position filter block must be empty!  
	unsigned int maxReps = currBlock->getMaxNumPos();
	unsigned int reps = triple_->reps;
	unsigned int startPos = triple_->startPos;
	int n = reps/maxReps;
	int mod = reps%maxReps;
	for (int i=0; i<n; i++){
		currBlock = new PosFilterBlock();
		currBlock->initEmptyBuffer(startPos);    
		currBlock->setRangePos(maxReps);
		startPos+=maxReps;
		addPosFilterBlock(currBlock);
	}
	if(mod>0){
		currBlock = new PosFilterBlock();
		currBlock->initEmptyBuffer(startPos);    
		currBlock->setRangePos(mod);
		addPosFilterBlock(currBlock);
	}
}