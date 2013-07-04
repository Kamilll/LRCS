#ifndef _UNCOMPRESSEDENCODER_H_
#define _UNCOMPRESSEDENCODER_H_

#include "Encoder.h"
#include "../Util/StringWriter.h"
//#include "../../Writers/SBasicBlockWriter.h"
// UncompressedEncoder encodes a page of strings
// The first int indicates the number of strings encoded
// The second int indicates the startPos of this page
// The third int indicates the string size of strings in this page
// Lastly, the rest of the page is a sequence of strings
class UncompressedEncoder : public Encoder
{
public:
	UncompressedEncoder(Operator* dataSrc_, int colIndex_, int stringSize, int bfrSizeInBits_);
	virtual ~UncompressedEncoder();
	virtual byte* getPage();
	virtual void resetPair();
	virtual bool writeVal(char* val_, unsigned int pos_);
    virtual int getNumValsPerPage(){return *lengthPtr;}
	virtual short getValSize(){return (short)*ssizePtr;}
	virtual unsigned int getStartPos(){return (unsigned int)*startPosPtr;}
	virtual unsigned int getPageSize(){return PAGE_SIZE;}
protected:
	//int totalNum;
	//int totalLength;
	StringWriter* writer;
	byte* buffer;
	int* lengthPtr;
	int* startPosPtr;
	int* ssizePtr;
	//int ssize;
	Block* currBlock;
	bool init;
	ValPos* currVP;
};

#endif //_UNCOMPRESSEDENCODER_H_
