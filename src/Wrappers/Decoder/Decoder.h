#ifndef DECODER_H
#define DECODER_H

#include "../../common/Constants.h"
#include "../../common/Interfaces.h"
#include "../CodingException.h"

#define HEADER_SIZE 8


class Decoder{
public:
	Decoder();
	Decoder(byte* buffer_);
	Decoder(Decoder& decoder_); 
	virtual ~Decoder();
	
	virtual void setBuffer(byte* buffer_);
	virtual int getPageLengthBits()=0;
	virtual byte* getBuffer();
	
	virtual bool hasNextBlock()=0;
	virtual Block* getNextBlock()=0;
	virtual Block* getNextBlockSingle() {return getNextBlock();}
	virtual bool skipToPos(unsigned int blockPos_)=0;
	virtual bool skipToBlockOnValue(ValPos* rhs_)=0;
	
	virtual ValPos* getStartVal()=0;
	virtual unsigned int getStartPos()=0;
	virtual ValPos* getEndVal()=0;
	virtual unsigned int getEndPos()=0;
	virtual unsigned int getSize()=0;
protected:
	bool initialized;
	byte* buffer;
	ValPos* utilityPair;
	bool valSorted;
};

#endif // DECODER_H
