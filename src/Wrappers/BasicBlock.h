#ifndef _BASICBLOCK_H_
#define _BASICBLOCK_H_

#include "../common/Block.h"
#include "CodingException.h"

class BasicBlock : public Block
{
public:
	BasicBlock(bool valSorted_, bool posContig_, bool posSorted_);
	BasicBlock(bool valSorted_, bool posContig_, bool posSorted_, short type_);
	BasicBlock(bool valSorted_, bool posContig_, bool posSorted_, ValPos* vp_);
	BasicBlock(const BasicBlock&);
	virtual ~BasicBlock();
	Block* clone(Block&);
	virtual void setValue(int val_, int pos_);
	virtual void setValue(ValPos* val_);
	virtual void resetBlock();
	// Iterator access to block
	virtual bool hasNext();
	virtual bool hasNext(int value_);
	virtual ValPos* getNext();
	virtual ValPos* peekNext();
	virtual int getValue();
	virtual unsigned int getPosition();
	virtual unsigned int getEndPosition();

	// Zero indexed, gets the pair at this pos_
	virtual ValPos* getPairAtLoc(unsigned int loc_);
	
	//Like getPairAtLoc except set the regular pair variable rather than 
	//the utilityPair variable so can be called by getNext(). 
	virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
	virtual int getCurrLoc();

	// return size of block		
	virtual int getSize();
	virtual int getSizeInBits();
	virtual ValPos* getStartPair();

	// Stream properties
	virtual bool isValueSorted();	
	virtual bool isPosSorted();
	
	// Block properties
	virtual bool isOneValue();
	virtual bool isPosContiguous();
	virtual bool isBlockValueSorted();	
	virtual bool isBlockPosSorted();
	
	int currPos;
protected:
	bool valSorted;
	bool posContig;
	bool posSorted;

	bool pairNewed;
};

#endif //_BASICBLOCK_H_
