#ifndef POSFILTERBLOCK_H
#define POSFILTERBLOCK_H
#include "assert.h"
#include <cstring>
#include "../Util/BitUtil.h"
#include "Constants.h"
#include "UnexpectedException.h"
#include "PosBlock.h"
class PosFilterCursor;

class PosFilterBlock: public PosBlock
{
public:	
	void *parentMultiBlock;

	PosFilterBlock( );
	PosFilterBlock(byte* buffer_);
	
	virtual ~PosFilterBlock();
	
	virtual void initEmptyBuffer(unsigned int startpos);
	//virtual void setBuffer(byte* buffer_);
	virtual bool addPosition(unsigned int pos);	
	
	virtual PosFilterBlock* clone();
	virtual PosFilterBlock* cutGetRightAtPos(unsigned int pos_);
	virtual PosFilterBlock* cutGetLeftAtPos(unsigned int pos_);
	virtual void leftMove(int moveBit_);
	virtual void cutTail( );
	virtual bool addInt(unsigned int Int_);

	// Iterator access to block
	virtual bool hasNextInt();
	virtual unsigned int getNextPos();
	virtual unsigned int getNextInt();
	virtual unsigned int getCurrIntValue();
	virtual unsigned int getStartPosition();
	virtual unsigned int getEndPosition();
	virtual unsigned int getNumValues();
	virtual unsigned int getNumValuesR(); //Re-caculate number of values;
	virtual unsigned int getCurrPosition();
	virtual unsigned int getCurrStartPosition();//Get current start position after filtered a DB page
	virtual void setCurrStartPosition();//Set current start position after filtered a DB page
	virtual unsigned int getCurrIntStartPos();//Get current Int's first bit's position
	virtual unsigned int getCurrIntRealStartPos();//Get current Int's first '1'bit's position
    virtual unsigned int getMaxNumPos();
	virtual void setRangePos(unsigned int length);
	
  	virtual PosFilterCursor* getCursor();
	//virtual void printBlock();

protected:
	unsigned int currStartPos;

	static unsigned int getIntStartPos( unsigned int Int_ );
	static unsigned int getIntEndPos( unsigned int Int_ );
	//virtual void printBits(int bits);
	
private:	
	//bool isBufferSet;
	//void init( );
};
#endif // POSFILTERBLOCK_H