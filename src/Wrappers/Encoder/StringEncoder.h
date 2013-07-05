#ifndef _STRINGENCODER_H_
#define _STRINGENCODER_H_

#include "Encoder.h"
#include "../Util/StringWriter.h"
#include "../MultiBlock.h"
//#include "../../Writers/SBasicBlockWriter.h"
// StringEncoder encodes a page of strings
// The first int indicates the number of strings encoded
// The second int indicates the startPos of this page
// The third int indicates the string size of strings in this page
// Lastly, the rest of the page is a sequence of strings
class StringEncoder : public Encoder
{
public:
	StringEncoder(Operator* dataSrc_, int colIndex_, int stringSize, int bfrSizeInBits_);
	virtual ~StringEncoder();
	virtual byte* getPage();
	virtual void resetPair();
    virtual int getNumValsPerPage(){return *lengthPtr;}
	virtual short getValSize(){return (short)*ssizePtr;}
	virtual int getBufferSize();//Get the exact buffer size in the page
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
	MultiBlock* currBlock;
	bool init;
	ValPos* currPair;
};

#endif //_STRINGENCODER_H_
