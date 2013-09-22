#ifndef RLEBLOCK_H
#define RLEBLOCK_H

#include "../common/Block.h"
//#include "../common/Constants.h"
//#include "../common/Interfaces.h"
//#include "../common/Util.h"
#include "RLETriple.h"
#include "CodingException.h"


class RLEBlock : public Block {
public:
	RLEBlock(bool valSorted_); // must be immediately followed by call to setTriple
	RLEBlock(RLETriple* trip_, bool valSorted_);
	RLEBlock(const RLEBlock&);
	virtual ~RLEBlock();
	Block* clone(Block&);
	
	virtual void resetBlock();
	// Iterator access to block
	virtual bool hasNext();
	virtual ValPos* getNext();
	virtual ValPos* peekNext();

	// Zero indexed, gets the pair at this pos_
	virtual ValPos* getPairAtLoc(unsigned int loc_);
	
	//Like getPairAtLoc except set the regular pair variable rather than 
	//the utilityPair variable so can be called by getNext(). 
	virtual ValPos* getPairAtLocNotUtility(unsigned int loc_);
	virtual int getCurrLoc();
	virtual bool skipToLoc(unsigned int loc_);

	// return size of block		
	virtual int getSize();
	virtual int getSizeInBits();
	virtual ValPos* getStartPair();
	virtual unsigned int getEndPosition();
	
	// Stream properties
	virtual bool isValueSorted();	
	virtual bool isPosSorted();
	
	// Block properties
	virtual bool isOneValue();
	virtual bool isPosContiguous();
	virtual bool isBlockValueSorted();	
	virtual bool isBlockPosSorted();
	
	bool isSplittable();
	Block* split(int npositions);
	
	virtual RLETriple* getTriple();
	virtual void setTriple(RLETriple* trip_);
	
protected:
	RLETriple* triple;

	unsigned int adjustedCurPos;
	bool valSorted;
	bool createdTriple;
};

#endif // RLEBLOCK_H
