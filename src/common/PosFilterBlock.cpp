#include "PosFilterBlock.h"
using namespace std;
PosFilterBlock::PosFilterBlock()
{
	init();
}

void PosFilterBlock::init(){
    currInt = 0;
    endInt = 0;
	posIndex = new int[8*sizeof(int)+1];
	assert(BLOCK_SIZE >= 5*sizeof(int));
	maxInt = (BLOCK_SIZE - 4*sizeof(int))/sizeof(int);
}


PosFilterBlock::~PosFilterBlock( )
{
  if(bfrWithHeader)delete[] bfrWithHeader;
  delete[] posIndex;
}


void PosFilterBlock::resetBlock() {
  if (bfrWithHeader)
    memset(bfrWithHeader, 0, BLOCK_SIZE);
}

PosFilterBlock* PosFilterBlock::clone() {
  PosFilterBlock* newBlock = new PosFilterBlock();
  newBlock->initEmptyBuffer(0);
  memcpy(newBlock->bfrWithHeader, bfrWithHeader, BLOCK_SIZE);
  newBlock->caculateEndInt();
  newBlock->setCurrInt(1);
  return newBlock;
}

PosFilterBlock*  PosFilterBlock::cutGetRightAtPos(unsigned int pos_){
  if ((int)(pos_ - *startPos) < 0)return NULL;
  if ((int)(pos_ - *startPos) == 0)return this;
  int cutInt = (pos_-*startPos)/(8*sizeof(int));
  int leftInt = endInt - cutInt;
  if(leftInt < 0 )return NULL;
  int moveBit = (pos_-*startPos)%(8*sizeof(int));
  *startPos = pos_;
  *numValues = 0; //number of values has no use in this case, so just ignore it.
  memcpy(buffer+sizeof(int), buffer+(cutInt+1)*sizeof(int), leftInt*sizeof(int));
  endInt = leftInt;
  leftMove(moveBit);
  caculateEndInt();
  if(maxInt>endInt)//set all the left ints to zero
    memset(buffer+(endInt+1)*sizeof(int),0,(maxInt-endInt)*sizeof(int));
  setCurrInt(1);
  return this;
}

PosFilterBlock*  PosFilterBlock::cutGetLeftAtPos(unsigned int pos_){
	if ((int)(pos_ - *startPos) <= 0)return NULL;
	int cutInt = (pos_-*startPos)/(8*sizeof(int));
	if(cutInt>endInt)return NULL;
	*endPos = pos_;
	*numValues = 0; //number of values has no use in this case, so just ignore it.
	caculateEndInt();
	cutTail();
	if(maxInt>endInt)//set all the left ints to zero
		memset(buffer+(endInt+1)*sizeof(int),0,(maxInt-endInt)*sizeof(int));
	setCurrInt(1);
    return this;
}

void PosFilterBlock::leftMove(int moveBit_){
	if (moveBit_ <= 0 || moveBit_ > 31)return;
	unsigned int valToOr;
	bufferPtrAsIntArr[1] = bufferPtrAsIntArr[1] << moveBit_;
	for(int i = 2; i<=endInt; i++){
		valToOr = bufferPtrAsIntArr[i] >>(32-moveBit_);
		bufferPtrAsIntArr[i] = bufferPtrAsIntArr[i] << moveBit_;
		bufferPtrAsIntArr[i-1] |= valToOr;
	}
}

void PosFilterBlock::cutTail( ){
	bufferPtrAsIntArr[endInt] &= 0xffffffffu<<(31-(*endPos - *startPos)%32);
}

void PosFilterBlock::initEmptyBuffer(unsigned int startPos_) {
/*
 *	+-----------------------------------+
 *	|  startPos |   endPos  | numValues |
 *	+-----------------------------------+
 *  |  Value (Int type )                |
 *  +-----------------------------------+
 *	|	 F R E E A R E A-->
 *	+-----------------------------------+
 *  startPos is the start position of this block,
 *  endPos is the end position of this block,
 *  numValues is number of values of this block,
 */

  bfrWithHeader=new byte[BLOCK_SIZE];
  memset(bfrWithHeader, 0, BLOCK_SIZE);

  startPos=(unsigned int*)bfrWithHeader;
  endPos=(unsigned int*) (bfrWithHeader+sizeof(int));
  numValues=(unsigned int*) (bfrWithHeader+2*sizeof(int));
  *startPos = startPos_;
  *endPos = startPos_;
  *numValues = 1;
  currStartPos = *startPos;

  buffer = bfrWithHeader + (3*sizeof(int));
  bufferPtrAsIntArr = (unsigned int*)buffer; 
  currInt = 1;
  endInt = currInt;
  bufferPtrAsIntArr[currInt] = bufferPtrAsIntArr[currInt] | (1 << 31);
}

bool PosFilterBlock::addPosition(unsigned int pos) {
	unsigned int movBit;
	unsigned int valToOr;
	unsigned int newEndInt;

	if(pos == 0)return false;//illegal position

	if(pos < *startPos){
		newEndInt = (*endPos-pos)/32+1;	  	
		if (newEndInt > maxInt) return false;//Block is full
		unsigned int avaBits = 31 - (*endPos - *startPos)%32;
		int gap = newEndInt - endInt;
		endInt = newEndInt;
		movBit = (*startPos-pos)%32;
		*startPos = pos;

		if (movBit == 0){
			for(int y=endInt;y>gap;y--){
				bufferPtrAsIntArr[y] = bufferPtrAsIntArr[y-gap];	
				bufferPtrAsIntArr[y-gap] = 0;
			}
		}else{	    
			if(movBit <= avaBits){
				for(int y=endInt;y>gap;y--){
					bufferPtrAsIntArr[y] = bufferPtrAsIntArr[y-gap]>>movBit;	
					if( (y-gap-1) > 0){
						valToOr = bufferPtrAsIntArr[y-gap-1]<<(32-movBit);
						bufferPtrAsIntArr[y] |= valToOr;  						
					}
					if (gap > 0) bufferPtrAsIntArr[y-gap] = 0;
				}
			}else{
				for(int y=endInt;y>gap;y--){
					bufferPtrAsIntArr[y] |=  bufferPtrAsIntArr[y-gap]<<(32-movBit);
					bufferPtrAsIntArr[y-1]  = bufferPtrAsIntArr[y-gap]>>movBit;
					bufferPtrAsIntArr[y-gap] = 0;
				}
			}
		}
		bufferPtrAsIntArr[1] =  bufferPtrAsIntArr[1]|(1 << 31); 
		*numValues+=1;
		return true;
	}else if (pos == *startPos)return true;
	else if((pos > *startPos) && (pos < *endPos)){
		currInt = (pos - *startPos)/32 + 1;
		movBit = (pos - *startPos)%32;
		if (!((1 << (31-movBit)) & bufferPtrAsIntArr[currInt])) {//make sure the bit is not 1
			bufferPtrAsIntArr[currInt] |= (1 << (31 - movBit));
			*numValues+=1;
		}
		return true;
	}else if(pos == *endPos)return true;
	else{
		newEndInt = (pos-*startPos)/32+1;	  	
		if (newEndInt > maxInt) return false;//Block is full
		currInt = newEndInt;
		endInt = newEndInt;
		*endPos = pos;
		movBit = (pos - *startPos)%32;
		bufferPtrAsIntArr[currInt] |= (1 << (31 - movBit));
		*numValues+=1;
		return true;
	}
}

void PosFilterBlock::setRangePos(unsigned int length){
   assert(length<=getMaxNumPos());
   memset(bufferPtrAsIntArr+sizeof(int), 1, length);
   *endPos = *startPos + length - 1;
   numValues = length;
}

bool PosFilterBlock::setCurrInt(unsigned int currInt_){
	if(currInt_ > endInt)return false;
	currInt = currInt_;
	/*if (currInt > 0){
		if (bufferPtrAsIntArr[currInt] == 0)currPos=0;
		else{
		  setPosIndex(bufferPtrAsIntArr[currInt], posIndex, currIndexInVal);
		  currPos = ((currInt-1)*sizeof(int)*8)+*startPos+posIndex[currIndexInVal];
		  currIndexInVal++;
		  if (posIndex[currIndexInVal] == 255)currIndexInVal=0;
		}
		setCurrStartPosition( );
	}*/
	currIndexInVal=0;
	return true;
}

unsigned int PosFilterBlock::getNext() {
	if (currInt<=endInt) {
		if(currInt == 0)setCurrInt(1);
		if (currIndexInVal) {
			currPos = ((currInt-1)*sizeof(int)*8)+*startPos+posIndex[currIndexInVal];
			currIndexInVal++;

			if (posIndex[currIndexInVal] == 255) {
				currInt++;	
				currIndexInVal=0;
			}
		}
		else {			
			while ((bufferPtrAsIntArr[currInt] == 0) && (currInt < endInt))
				currInt++;	

			if ((currInt == endInt) && ((bufferPtrAsIntArr[currInt] == 0)))
				return 0;
			else
				setPosIndex(bufferPtrAsIntArr[currInt], posIndex, currIndexInVal);
		
			currPos=((currInt-1)*sizeof(int)*8)+*startPos+posIndex[currIndexInVal];
			currIndexInVal++;
			if (posIndex[currIndexInVal] == 255) {
				currInt++;	
				currIndexInVal=0;
			}
		}
	}
	else
		return 0;

	return currPos;
}

unsigned int PosFilterBlock::getNextInt() {
	currInt++;	
	if (currInt<=endInt) {
		return bufferPtrAsIntArr[currInt];
	}else{
		currInt = endInt;//reset currInt to endInt;
		return 0;
	}
}

unsigned int PosFilterBlock::getCurrIntValue() {
	return bufferPtrAsIntArr[currInt];
}

//Append a new int 
bool PosFilterBlock::addInt(unsigned int Int_) {
	if(currInt == maxInt)return false;//block is full
	
	bufferPtrAsIntArr[++currInt] = Int_;
	if(getIntEndPos(Int_) != 0)
	*endPos = *startPos + (currInt-1)*8*sizeof(int) + getIntEndPos(Int_) - 1;
	endInt = currInt;
	return true;
}

unsigned int PosFilterBlock::getIntEndPos( unsigned int Int_ ) {
	if(Int_==0)return 0;
	for (int i=0; i < 8*sizeof(int); i++)
		if((1<<i) & Int_){
			return (8*sizeof(int) - i);
			break;
		}
}

unsigned int PosFilterBlock::getIntStartPos( unsigned int Int_ ) {
	if(Int_==0)return 0;
	for (int i=(8*sizeof(int)-1); i >= 0; i--)
		if((1<<i) & Int_){
			return (8*sizeof(int) -i);
			break;
		}
}

bool PosFilterBlock::hasNext() {
	return (currPos < *endPos);
}

bool PosFilterBlock::hasNextInt() {
	return (currInt < endInt);
}

void PosFilterBlock::setPosIndex(int v, int* pidx, int& currIndexInVal) {
  currIndexInVal=0;
  for (int i = (8*sizeof(int))-1; i>=0; i--) {
    if ((1 << i) & v) {
      pidx[currIndexInVal] = ((8*sizeof(int))-1)-i;
      currIndexInVal++;
    }
  }
  pidx[currIndexInVal]=255;
  currIndexInVal=0;
}

unsigned int PosFilterBlock::getStartPosition() {
  return *startPos;
}

unsigned int PosFilterBlock::getEndPosition() {
   return *endPos;
}

unsigned int PosFilterBlock::getNumValues() {
   return *numValues;
}

unsigned int PosFilterBlock::getNumValuesR() {
	*numValues = 0;
	BitUtil::compute_bits_in_16bits();

	for(int i = 1; i<=endInt; i++)
		*numValues += BitUtil::precomputed16_bitcount (bufferPtrAsIntArr[i]);

	return *numValues;
}

unsigned int PosFilterBlock::getMaxNumPos(){
   return maxInt*sizeof(int)*8;
};

unsigned int PosFilterBlock::getCurrPosition() {
   return currPos;
}
bool PosFilterBlock::setCurrPosition(unsigned int pos_) {
	if(pos_ > *endPos)return false;
	currPos = pos_;
	return true;
}

unsigned int PosFilterBlock::getCurrStartPosition() {
   return currStartPos;
}

unsigned int PosFilterBlock::getCurrIntStartPos() {
   return (*startPos + (currInt - 1)*8*sizeof(int));
}

unsigned int PosFilterBlock::getCurrIntRealStartPos() {
	return (*startPos + (currInt - 1)*8*sizeof(int) + getIntStartPos(bufferPtrAsIntArr[currInt]) - 1);
}

void PosFilterBlock::setCurrStartPosition( ) {
/*Record the current position to the current start position.
The purpose is to continue the filtering at pervious break point
*/
	currStartPos = currPos;
}

void PosFilterBlock::caculateEndInt( ) {
	//endInt =(*endPos-*startPos+1)%(8*sizeof(int))?((*endPos-*startPos+1)/(8*sizeof(int)) + 1):(*endPos-*startPos+1)/(8*sizeof(int));
	endInt = (*endPos-*startPos)/(8*sizeof(int))+1;
}


void PosFilterBlock::printBits(int bits) {
	for (int i = 0; i < 32; i++)
		cout << (bool)(bits & (((unsigned int)1) << (31-i)));
	cout << endl;
}

void PosFilterBlock::printBlock( ) {
	cout<<"startPos:" <<*startPos<<", "
		<<"endPos:" <<*endPos<<", "
		<<"numValues:" <<*numValues<<endl;
	for(int i = 1; i<=endInt; i++)
       printBits(bufferPtrAsIntArr[i]);
}

