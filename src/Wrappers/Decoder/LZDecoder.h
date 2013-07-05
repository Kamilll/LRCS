#ifndef _LZDECODER_H_
#define _LZDECODER_H_

#include "Decoder.h"

using namespace std;

class LZDecoder : public Decoder
{
	public:
		LZDecoder(Decoder* decoder_);
		//LZDecoder(LZDecoder& decoder_); 
		virtual ~LZDecoder();

		virtual void setBuffer(byte* buffer_);
		virtual int getPageLengthBits(){return sizeCompressed*8;}

		virtual bool hasNextBlock();
		virtual Block* getNextBlock();
		virtual Block* getNextBlockSingle();
		virtual bool skipToPos(unsigned int blockPos_);
	    virtual bool skipToBlockOnValue(ValPos* rhs_);

		virtual ValPos* getStartVal();
		virtual unsigned int getStartPos();
		virtual ValPos* getEndVal();
		virtual unsigned int getEndPos();
		virtual unsigned int getSize(){return sizeCompressed;}

	protected:
		//unsigned int numVals;
		//unsigned int startPos;
		unsigned int sizeCompressed;
		unsigned int sizeOrignal;
		//short valSize;
		Decoder* decoder;
};

#endif //_LZDECODER_H_
