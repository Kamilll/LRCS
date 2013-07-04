#ifndef BLOCK_H
#define BLOCK_H

#pragma once

#include "Constants.h"
#include "Predicate.h"
#include "IntValPos.h"
#include "NullValPos.h"
#include "LongValPos.h"
#include "FloatValPos.h"
#include "DoubleValPos.h"
#include "StringValPos.h"
#include "UnexpectedException.h"
//#include "PosBlockArray.h"

// Interface for data object passed between operators
class Block {
	public:	
		Block();
		Block(const Block&);
		virtual ~Block();	

		virtual Block* clone(Block&) = 0;

		// Iterator access to block
		virtual bool hasNext()=0;
		// returns true if has next value and next value is equal to value_ 
		//virtual bool hasNext(int value_)=0;
		virtual ValPos* getNext()=0;
		virtual ValPos* peekNext()=0;
		virtual void setType(short type_);
		virtual void setType(ValPos* vp_);

		// Split the block such that it retains the first npositions positions and
		// the remaining positions are in the returned Block. 
		//	virtual Block* split(int npositions);

		// Method to determine whether or not a Block can be split
		//	virtual bool isSplittable();

		// Zero indexed, gets the pair at this loc
		virtual ValPos* getPairAtLoc(unsigned int loc_)=0;

		//Like getPairAtLoc except set the regular pair variable rather than 
		//the utilityPair variable so can be called by getNext(). 
		virtual ValPos* getPairAtLocNotUtility(unsigned int loc_)=0;
		virtual int getCurrLoc()=0;

		// return size of block	(number of values)
		virtual int getSize()=0;
		//return buffer size in block
		virtual int getSizeInBits()=0;
		virtual ValPos* getStartPair()=0;
		virtual unsigned int getEndPosition()=0;

		virtual void resetBlock()=0;

		// Stream properties
		virtual bool isValueSorted()=0;	
		virtual bool isPosSorted()=0;

		// Block properties
		virtual bool isOneValue()=0;
		virtual bool isPosContiguous()=0;
		virtual bool isBlockValueSorted()=0;	
		virtual bool isBlockPosSorted()=0;

		virtual Block* getBlock() { return this; }

		ValPos* vp;
		short valsize;

	protected:
		ValPos* utilityVP;
};

#endif // BLOCK_H
