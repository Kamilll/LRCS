#include "MultiBlock.h"

MultiBlock::MultiBlock(bool valSorted_, bool posContig_, bool posSorted_) : Block()
{  
	init(valSorted_, posContig_, posSorted_); 
	setType(ValPos::STRINGTYPE);
}

MultiBlock::MultiBlock(bool valSorted_, bool posContig_, bool posSorted_,short type_) : Block()
{
	init(valSorted_, posContig_, posSorted);
	setType(type_);
}


MultiBlock::MultiBlock(bool valSorted_, bool posContig_, bool posSorted_, ValPos* vp_) : Block()
{
	init(valSorted_, posContig_, posSorted);
	setType(vp_);
}

void MultiBlock::init(bool valSorted_, bool posContig_, bool posSorted_) {
	valSorted=valSorted_ ;
	posContig=posContig_;
	posSorted=posSorted_;
	valsize = 0;
	currPos=-1;
	buffer = NULL;
	buffer_size = 0;
	startPos=-1;
	numValues=0;
	bufferdirect = false;
	vp = NULL;
	utilityVP = NULL;
}

MultiBlock::MultiBlock(const MultiBlock& block_) : Block(block_)
{
	vp=block_.vp->clone();
	utilityVP=block_.utilityVP->clone();
	valSorted=block_.valSorted;
	posContig=block_.posContig;
	posSorted=block_.posSorted;
	buffer=NULL;
	buffer_size=0;
	setBuffer(block_.startPos, block_.numValues,block_.valsize,block_.buffer);
}

Block* MultiBlock::clone(Block& block_)
{
	Block* newBlock = new MultiBlock((MultiBlock&)block_);
	return newBlock;
}

MultiBlock::~MultiBlock()
{
	delete vp;
	delete utilityVP;
	if (buffer && !bufferdirect)
		delete[] buffer;
}

void MultiBlock::resetBlock() {
	currPos=-1;
	currPosPtr=buffer;
	vp->set(startPos, currPosPtr);
}
void MultiBlock::setBuffer(int startpos_, int numValues_, short valsize_, byte* buffer_) {
	currPos=-1;
	startPos=startpos_;
	valsize = valsize_;
	int sz = numValues_*valsize;
	if (buffer_size != sz) {
		if (buffer)
			delete[] buffer;
		buffer_size=sz;
		buffer=new byte[sz];
	}
	memcpy(buffer, buffer_, numValues_*valsize);
	currPosPtr=buffer;
	numValues=numValues_;
	vp->set(startpos_, currPosPtr, valsize);
	bufferdirect = false; 
}

// Assumes buffer is being provided to us externally--we just set a pointer to it
void MultiBlock::setBufferDirect(int startpos_, int numValues_,short valsize_, byte* buffer_) {
	currPos = -1;
	startPos = startpos_;
	valsize = valsize_;	
	buffer = buffer_;
	buffer_size = numValues_ * valsize;
	currPosPtr = buffer;
	numValues = numValues_;
	vp->set(startPos, currPosPtr, valsize);
	bufferdirect = true;
}

byte* MultiBlock::getBuffer() {
	return buffer;
}

// Iterator access to block
bool MultiBlock::hasNext() {
	return (currPos<(int)(numValues));
}

ValPos* MultiBlock::getNext() {
	if (currPos<0) {
		currPos=1;	
		currPosPtr+=valsize;
		return vp;	
	}
	else if (currPos<(numValues)) {
		vp->set(currPos+startPos, currPosPtr);
		currPosPtr+=valsize;
		currPos++;
		return vp;
	}
	else
		return NULL;
}

ValPos* MultiBlock::peekNext() {
	if (currPos<0) {
		utilityVP->set(startPos, buffer);
		return utilityVP;
	}
	else if (hasNext()) {
		utilityVP->set(currPos+startPos, currPosPtr);
		return utilityVP;
	}
	else
		return NULL;
}

unsigned int MultiBlock::getPosition() {
	if (vp->position==0) 
		throw new CodingException("MultiBlock: cannot get position of empty multi block");	
	return vp->position;
}
unsigned int MultiBlock::getLastPosition() {
	return getPairAtLoc(getSize()-1)->position;
}
unsigned int MultiBlock::getEndPosition() {
	return getPairAtLoc(getSize()-1)->position;
}

// Zero indexed, gets the pair at this pos_
ValPos* MultiBlock::getPairAtLoc(unsigned int loc_) {
	assert (loc_<(unsigned int)numValues);
	utilityVP->set(loc_+startPos, buffer + (loc_*valsize));
	return utilityVP;
}

//Like getPairAtLoc except set the regular pair variable rather than 
//the utilityPair variable so can be called by getNext().
ValPos* MultiBlock::getPairAtLocNotUtility(unsigned int loc_) {
	assert (loc_<(unsigned int)numValues);
	vp->set(loc_+startPos, buffer + (loc_*valsize));
	return vp;
}

bool MultiBlock::skipToLoc(unsigned int loc_) {
	if ((loc_<(unsigned int)numValues) && (loc_>=0)) {
		currPosPtr=buffer+(valsize*loc_);
		currPos=loc_;
		return true;
	}    
	else
		return false;
}

void MultiBlock::cutOffEnd(int endPos) {
	if (endPos < startPos+numValues)
		numValues=(endPos-startPos)+1;
}

int MultiBlock::getCurrLoc() {
	if (currPos == -1)
		return 0;
	else
		return currPos-1;
}

// return size of block		
int MultiBlock::getSize() {
	return numValues;	
}
//return buffer size in bits
int MultiBlock::getSizeInBits() {
	return (numValues)*valsize*8;	
}
ValPos* MultiBlock::getStartPair() {
	if (buffer==NULL) return NULL;
	else return getPairAtLoc(0);
}

bool MultiBlock::isOneValue() {
	if (numValues == 1)
		return true;
	else
		return false;
}
bool MultiBlock::isValueSorted() {
	return valSorted;	
}

bool MultiBlock::isPosContiguous() {
	return posContig;	
}
bool MultiBlock::isPosSorted() {
	return posSorted;	
}
bool MultiBlock::isBlockValueSorted() {
	return false;	
}
bool MultiBlock::isBlockPosSorted() {
	return true;
}

bool MultiBlock::isSplittable() {
	return numValues > 1;
}

// FIXME make the two blocks share memory
Block* MultiBlock::split(int npositions) {
	assert(npositions < numValues && npositions > 0); // sanity-check the split
	cerr << "want to retain " << npositions << "/" << numValues << endl;

	// Create the leftover block
	byte* leftover_buf = new byte[valsize*(numValues - npositions)];
	memcpy(leftover_buf, &(buffer[npositions*valsize]), valsize*(numValues - npositions));
	MultiBlock* leftover = new MultiBlock(true, true, true);
	leftover->setBuffer(startPos + npositions, 
	                    (numValues - npositions), valsize, leftover_buf);
	delete [] leftover_buf;

	// Modify this block to reflect the split
	byte* newbuf = new byte[valsize*npositions];
	memcpy(newbuf, buffer, npositions);
	setBuffer(startPos, npositions, valsize, newbuf);
	delete [] newbuf;

	return leftover;
}

MultiBlock* MultiBlock::copySubset(int fromPos, int untilPos) {
	int endPos = getEndPosition();
	assert(startPos <= fromPos);
	assert(startPos <= untilPos);
	assert(fromPos <= endPos);
	assert(untilPos <= endPos);
	assert(fromPos <= untilPos);
	MultiBlock* temp = new MultiBlock(valSorted, posContig, posSorted);
	temp->setBuffer(startPos, untilPos-fromPos+1, valsize, buffer+(valsize*(fromPos-startPos)));
	return temp;
}

void MultiBlock::filterWithPos(MultiPosFilterCursor* filterCursor){
	unsigned int nextPos = 0;
	unsigned int nextFilterPos;
	StringWriter* swriter = new StringWriter(valsize, numValues*valsize*8);
	while((nextPos+1) <= numValues){//block is running out
		swriter->writeString((char*)(currPosPtr+nextPos*valsize));
		nextFilterPos = filterCursor->getNext();
		if (nextFilterPos == 0)
			break; //filter is finished
		nextPos = nextFilterPos-filterCursor->getCurrStartPosition();
	}
	delete[] buffer;
	setBufferDirect(filterCursor->getCurrStartPosition(),swriter->getNumValuesWritten(),valsize,swriter->getBuffer());
	filterCursor->setCurrStartPosition();//Record current start position.
}
