#ifndef OPERATOR_H
#define OPERATOR_H

#include "../common/Constants.h"
#include "../common/UnimplementedException.h"

class PosBlock;
class Block;

// Abstract class for implementaion of an Operator
class Operator{
	public:
		Operator();

		// destructor
		virtual ~Operator();	

		// Gets the next value block from the operator 
		virtual	Block* getNextValBlock(int colIndex_)=0;

		// Gets the next position block (bitstring of positions) from the operator
		virtual PosBlock* getNextPosBlock(int colIndex_)=0;

		// Gets the maximum position in the data source
		virtual int getLastPosition()=0;


	protected:


};

#endif // OPERATOR_H
