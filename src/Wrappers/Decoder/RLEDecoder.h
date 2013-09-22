#ifndef RLEDECODER_H
#define RLEDECODER_H

#include "RLESuperDecoder.h"
#include "../Util/BitReader.h"
#include "../RLETriple.h"
#include "../CodingException.h"
#include "../RLEBlock.h"


class RLEDecoder : public RLESuperDecoder {
public:
	RLEDecoder(bool valSorted_, bool isPosColumn_);
	RLEDecoder(byte* buffer, bool valSorted_, bool isPosColumn_);
	virtual ~RLEDecoder();	
	
	virtual void setBuffer(byte* buffer_);
	
	virtual bool hasNextBlock();
	virtual Block* getNextBlock();
	virtual Block* skipToBlock(unsigned int blockPos_);
	virtual bool skipToPos(unsigned int blockPos_);
	virtual Block* getBlock(unsigned int blockPos_);

	virtual bool skipToBlockOnValue(int value_);
	virtual bool skipToBlockOnValue(ValPos* rhs_){throw new UnimplementedException("");}
	virtual bool skipToBlockOnPos(unsigned int pos);
	
	
	virtual ValPos* getStartVal();
	virtual unsigned int getStartPos();
	virtual ValPos* getEndVal();
	virtual unsigned int getEndPos();
	virtual int getPageLengthBits();	
	virtual unsigned int getSize();

protected:
	virtual bool hasNextTriple();
	virtual RLETriple* peekNextTriple();
	virtual RLETriple* getNextTriple();
	
	virtual bool skipToTriple(unsigned int triplePos_);
	virtual RLETriple* getTriple(unsigned int triplePos_);

    BitReader* reader;
	RLETriple* result;
	RLEBlock* block;
	byte* data;
	bool isPosColumn;
	short valueSize;
	unsigned int startPosSize;
	unsigned int repsSize;
	unsigned int tripleSize;
	ValPos* vp;
};

#endif // RLEDECODER_H
