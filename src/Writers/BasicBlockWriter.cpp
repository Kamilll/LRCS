#include "BasicBlockWriter.h"

BasicBlockWriter::BasicBlockWriter(Operator* blockSrc_, int colIndex_) : Writer(blockSrc_, colIndex_)
{
	currBlock=blockSrc_->getNextValBlock(colIndex_);
	if (currBlock!=NULL)
		outBlock=new BasicBlock(currBlock->isValueSorted(),
		                        currBlock->isPosContiguous(),
		                        currBlock->isPosSorted());
	else 
		outBlock=NULL;
}

BasicBlockWriter::~BasicBlockWriter()
{
	if (outBlock!=NULL) delete outBlock;
}
// Gets the next value block from the operator 
Block* BasicBlockWriter::getNextValBlock(int colIndex_) {
	if (colIndex_!=0) return NULL;
	if ((currBlock==NULL) || (!currBlock->hasNext())) {
		do {
			currBlock=blockSrc->getNextValBlock(colIndex);
			if (currBlock==NULL) 
				return NULL;
		} while (!currBlock->hasNext());
	}
	ValPos* vp = currBlock->getNext();
	if (vp==NULL) throw new UnexpectedException("BasicBlockWriter: Error, block returned NULL pair when it said it had next");
	outBlock->setValue(vp);
	return outBlock;

}	

// Gets the next position block (bitstring of positions) from the operator
PosBlock* BasicBlockWriter::getNextPosBlock(int colIndex_) {
	return NULL;
}
