#ifndef POSOPERATOR_H
#define POSOPERATOR_H
#include "../common/MultiPosFilterBlock.h"
#include "assert.h"

struct whereOp
{
   char op; //'(' = '(', 'AND' = '&', 'OR' = '|',')' = ')', 'Predication' = 'P'
   MultiPosFilterBlock* posBlock;
     whereOp* nextOp;
};

class PosOperator{
public:
	PosOperator();
	virtual ~PosOperator();	
	
	// Add where operators to be caculated
	virtual	bool addWhereOp(char op);

	virtual void finishWhereOp( );

	// Add position filter blocks
	virtual	bool addPosBlock(MultiPosFilterBlock* posBlock);

	// Parse the where conditions and do "And" "Or" Caculate to get the final position filters
	virtual	MultiPosFilterBlock* getPosFilter( );
	
protected:

	whereOp* WhereOp;
	whereOp* currWhereOp;
    MultiPosFilterBlock* caculatedPosBlock;
	MultiPosFilterBlock* tempPosBlock1;
	MultiPosFilterBlock* tempPosBlock2;
	MultiPosFilterBlock* cacuPosBlock;
	char prevOp;
	char intermOp;
	int Level;

	virtual bool parseWhereOpreation( char& op_, MultiPosFilterBlock* &intermPosBlock_);
	virtual MultiPosFilterBlock* doAndCaculate(MultiPosFilterBlock* posBlock1_, MultiPosFilterBlock* posBlock2_);
	virtual MultiPosFilterBlock* doOrCaculate(MultiPosFilterBlock* posBlock1_, MultiPosFilterBlock* posBlock2_);
};

#endif // POSOPERATOR_H
