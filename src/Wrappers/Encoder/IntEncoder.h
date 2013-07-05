#ifndef _INTENCODER_H_
#define _INTENCODER_H_

#include "../Encoder/Encoder.h"
#include "../Util/IntWriter.h"
//#include "../../Writers/BasicBlockWriter.h"

// IntEncoder encodes a page of ints
// The first int indicates the number of ints encoded
// The second int indicates the startPos of this page
// Lastly, the rest of the page is a sequence of ints
class IntEncoder : public Encoder
{
public:
	IntEncoder(Operator* dataSrc_, int colIndex_, int bfrSizeInBits_);
	virtual ~IntEncoder();
	virtual byte* getPage();
	virtual void resetPair();
	virtual bool writeVal(int val_, unsigned int pos_);
    virtual int getNumValsPerPage(){return *lengthPtr;}
	virtual short getValSize(){return (short)sizeof(int);}
	virtual int getBufferSize();
	virtual unsigned int getStartPos(){return (unsigned int)*startPosPtr;}
	virtual unsigned int getPageSize(){return PAGE_SIZE;}
protected:
	//int totalNum;
	//int totalLength;
	IntWriter* writer;
	//BasicBlockWriter* blockWriter;
	byte* buffer;
	int* lengthPtr;
	int* startPosPtr;
	Block* currBlock;
	bool init;
	//Pair* currPair;
	ValPos* currVP;
};

#endif //_INTENCODER_H_
