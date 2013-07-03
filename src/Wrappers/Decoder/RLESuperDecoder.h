#ifndef RLESUPERDECODER_H
#define RLESUPERDECODER_H

#include "Decoder.h"
#include "../Util/BitReader.h"
#include "../RLETriple.h"
#include "../CodingException.h"
#include "../RLEBlock.h"


class RLESuperDecoder : public Decoder {
public:
  RLESuperDecoder() : Decoder() {}
  RLESuperDecoder(byte* buffer) : Decoder(buffer) {}
  virtual ~RLESuperDecoder(){}
	virtual bool hasNextBlock()=0;
	virtual Block* getNextBlock()=0;
	virtual Block* skipToBlock(unsigned int blockPos_)=0;
	virtual bool skipToPos(unsigned int blockPos_)=0;
	virtual Block* getBlock(unsigned int blockPos_)=0;

	virtual bool skipToBlockOnValue(int value_){throw new UnimplementedException("");}
	virtual bool skipToBlockOnValue(ValPos* rhs_){throw new UnimplementedException("");}
	virtual bool skipToBlockOnPos(unsigned int pos)=0;
	
	
	virtual ValPos* getStartVal()=0;
	virtual unsigned int getStartPos()=0;
	virtual ValPos* getEndVal()=0;
	virtual unsigned int getEndPos()=0;
	virtual int getPageLengthBits()=0;	
	virtual unsigned int getSize()=0;

protected:
	virtual bool hasNextTriple()=0;
	virtual RLETriple* peekNextTriple()=0;
	virtual RLETriple* getNextTriple()=0;
	
	virtual bool skipToTriple(unsigned int triplePos_)=0;
	virtual RLETriple* getTriple(unsigned int triplePos_)=0;
};

#endif // RLESUPERDECODER_H
