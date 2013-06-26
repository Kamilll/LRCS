#ifndef POSFILTERBLOCK_H
#define POSFILTERBLOCK_H
#include "assert.h"
#include <cstring>
#include <iostream>
#include "../Util/BitUtil.h"
#include "UnexpectedException.h"

typedef unsigned char byte;
static const int BLOCK_SIZE=6000;
//static const int BLOCK_SIZE=28;

class PosFilterBlock
{
public:	
	void *parentMultiBlock;

	PosFilterBlock( );
	virtual ~PosFilterBlock();

	virtual void init( );
	virtual void initEmptyBuffer(unsigned int startpos);
	virtual bool addPosition(unsigned int pos);
	virtual void resetBlock();
	virtual PosFilterBlock* clone();
	virtual PosFilterBlock* cutGetRightAtPos(unsigned int pos_);
	virtual PosFilterBlock* cutGetLeftAtPos(unsigned int pos_);
	virtual void leftMove(int moveBit_);
	virtual void cutTail( );
	virtual bool addInt(unsigned int Int_);

	// Iterator access to block
	virtual bool setCurrInt(unsigned int currInt_);
	virtual bool hasNext();
	virtual bool hasNextInt();
	virtual unsigned int getNext();
	virtual unsigned int getNextInt();
	virtual unsigned int getCurrIntValue();
	virtual unsigned int getStartPosition();
	virtual unsigned int getEndPosition();
	virtual unsigned int getNumValues();
	virtual unsigned int getNumValuesR(); //Re-caculate number of values;
	virtual unsigned int getCurrPosition();
	virtual bool setCurrPosition(unsigned int pos_);
	virtual unsigned int getCurrStartPosition();//Get current start position after filtered a DB page
	virtual void setCurrStartPosition();//Set current start position after filtered a DB page
	virtual unsigned int getCurrIntStartPos();//Get current Int's first bit's position
	virtual unsigned int getCurrIntRealStartPos();//Get current Int's first '1'bit's position
    virtual unsigned int getMaxNumPos();
	virtual void setRangePos(unsigned int length);
	virtual void caculateEndInt();
	virtual void printBlock();

protected:

    byte* bfrWithHeader;
	byte* buffer;
	unsigned int* bufferPtrAsIntArr;
	unsigned int* numValues;
	unsigned int* startPos;
	unsigned int* endPos;
	int* posIndex;
	unsigned int currStartPos;
	int currIndexInVal;
    unsigned int currInt;
	unsigned int currPos;
    unsigned int endInt;
	unsigned int maxInt;

    virtual void setPosIndex(int v, int* pidx, int& currIndexInVal);
	static unsigned int getIntStartPos( unsigned int Int_ );
	static unsigned int getIntEndPos( unsigned int Int_ );
	virtual void printBits(int bits);

};
#endif // POSFILTERBLOCK_H