#ifndef _BitmapDecoder_H_
#define _BitmapDecoder_H_

#include "Decoder.h"
#include "../../common/Constants.h"
#include "../../common/PosFilterBlock.h"

class BitmapDecoder : public Decoder
{
public:
	BitmapDecoder( );
	//BitmapDecoder(byte* buffer_);
	//BitmapDecoder(BitmapDecoder& decoder_); 
	virtual ~BitmapDecoder();
	
	virtual void setBuffer(byte* buffer_);
	virtual int getValueSize(){return 8*sizeof(int);}
	virtual int getPageLengthBits();
	
	
	virtual bool hasNextBlock();
	//virtual Block* peekNextBlock();
	virtual Block* getNextBlock();
	//virtual Block* getNextBlockSingle();
	virtual bool skipToPos(unsigned int blockPos_){return false;}//Not Necessary
	virtual bool skipToBlockOnValue(ValPos* rhs_){return false;}//Not Necessary

	virtual ValPos* getStartVal(){return NULL;}//Not Necessary
	virtual unsigned int getStartPos(){return 0;}//Not Necessary
	virtual ValPos* getEndVal(){return NULL;}//Not Necessary
	virtual unsigned int getEndPos(){return 0;}//Not Necessary
	
	virtual unsigned int getSize() { return *numBlocksPtr; }
	virtual unsigned int getCurrPosition(){return 0;}//Not Necessary
protected:
	unsigned int* numBlocksPtr;
    byte* blockArray;
	int currBlockNum;
	//PosBlock* posBlock;
	PosFilterBlock* posFilterBlock;

};

#endif //_BitmapDecoder_H_
