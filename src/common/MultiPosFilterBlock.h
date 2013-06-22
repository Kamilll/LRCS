#ifndef MULTIPOSFILTERBLOCK_H
#define MULTIPOSFILTERBLOCK_H
#include "PosFilterBlock.h"
#include <vector>
#include "assert.h"
#include "UnexpectedException.h"

using namespace std;

class MultiPosFilterBlock
{
public:
	MultiPosFilterBlock( );
	virtual ~MultiPosFilterBlock();
    
	
	virtual void addPosFilterBlock(PosFilterBlock* posFilterBlock_);
	virtual void addPosition(unsigned int pos);	
	virtual void addInt(unsigned int int_);
    virtual void optimize( );
	virtual void resetBlock();
	virtual MultiPosFilterBlock* clone();

	// Iterator access to blocks
	virtual bool hasNext();
	virtual unsigned int getNext();
	virtual PosFilterBlock* getNextBlock();
	virtual PosFilterBlock* getFirstBlock();
	virtual unsigned int getStartPosition();
	virtual unsigned int getEndPosition();
	virtual PosFilterBlock* getCurrBlock();
	virtual bool setCurrBlock(int i);
	virtual unsigned int getCurrPosition();
	virtual unsigned int getNumBlocks();
	virtual unsigned int getNumValues();
	virtual unsigned int getNumValuesR();
	virtual unsigned int getCurrStartPosition();//Get current start position after filtered a DB page
	virtual void setCurrStartPosition();//Set current start position after filtered a DB page
	virtual void printBlocks();

protected:
	vector<PosFilterBlock*> posFilterBlockVec;

	unsigned int startPos;
	unsigned int endPos;
    unsigned int currBlockNum;
	unsigned int numValues;
	PosFilterBlock* currBlock;

	virtual void init( );
	virtual bool removePrecedingZero(PosFilterBlock* &posFilterBlock_);
};
#endif // POSFILTERBLOCK_H