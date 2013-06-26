#ifndef WRITER_H
#define WRITER_H

#include "Constants.h"
#include "Interfaces.h"

class Block;

/* Writer class, takes in some format of block and translate it to a new type. 
 * Writers are thus coding specific operators. The functioning is simple, 
 * it pulls in blocks from some blockSrc and it outputs them in a different coding 
 * with calls to getBlock. The writer may consume more blocks than it outputs due
 * to the changes in the coding scheme.
 */
// Problems: migfer@mit.edu
class Writer : public Operator {
public:

	Writer(Operator* blockSrc_, int colIndex_);
	virtual ~Writer();
	
	// Gets the next value block from the operator 
	virtual	Block* getNextValBlock(int colIndex_);	
	
	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_);
	
	// Gets the maximum position in the data source
	virtual int getLastPosition()=0;
		
protected:
	Operator* blockSrc;
	int colIndex;
};

#endif // WRITER_H
