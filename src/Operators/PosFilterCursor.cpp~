#include "PosFilterCursor.h"
#include "../common/PosFilterBlock.h"

PosFilterCursor::PosFilterCursor(PosFilterBlock* SPFB_){
	bfrWithHeader = SPFB_->getBuffer(); 
	startPos=(unsigned int*)bfrWithHeader;
	endPos=(unsigned int*) (bfrWithHeader+sizeof(int));
	numValues=(unsigned int*) (bfrWithHeader+2*sizeof(int));

	buffer = bfrWithHeader + (3*sizeof(int));
	bufferPtrAsIntArr = (unsigned int*)buffer; 
	currInt = 1;
	currPos = 0;
	currStartPos = 0;
	endInt = (*endPos-*startPos)/(8*sizeof(int))+1;

	posIndex = new int[8*sizeof(int)+1];
	currIndexInVal = 0;
}

PosFilterCursor::~PosFilterCursor(){
	delete[] posIndex;
}

unsigned int PosFilterCursor::getNext() {
	if (currInt<=endInt) {
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
	else{
		currPos = 0;
		currInt = 0;
		return 0;
	}
	return currPos;
}

bool PosFilterCursor::hasNext() {
	return (currPos < *endPos);
}

void PosFilterCursor::setCurrStartPosition() {
	currStartPos = currPos;
}

unsigned int PosFilterCursor::getCurrStartPosition() {
	return currStartPos;
}

unsigned int PosFilterCursor::getCurrPosition() {
	return currPos;
}

void PosFilterCursor::resetCursor() {
	currInt = 1;
	currIndexInVal=0;
}

void PosFilterCursor::setPosIndex(int v, int* pidx, int& currIndexInVal) {
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