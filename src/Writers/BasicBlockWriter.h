#ifndef _BASICBLOCKWRITER_H_
#define _BASICBLOCKWRITER_H_

#include "../common/Writer.h"
#include "../Wrappers/BasicBlock.h"

class BasicBlockWriter : public Writer
{
public:
	BasicBlockWriter(Operator* blockSrc_, int colIndex_);
	virtual ~BasicBlockWriter();
	// Gets the next value block from the operator 
	virtual	Block* getNextValBlock(int colIndex_);	
	
	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_);
	
	inline virtual int getLastPosition() { throw UnimplementedException("Not applicable for BasicBlockWriter"); };

protected:
	Block* currBlock;
	BasicBlock* outBlock;
};

#endif //_BASICBLOCKWRITER_H_
