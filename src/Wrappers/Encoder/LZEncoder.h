#ifndef _LZENCODER_H_
#define _LZENCODER_H_

#include "Encoder.h"
#include "../../Writers/BasicBlockWriter.h"
#include "../../common/Interfaces.h"


using namespace std;

class LZEncoder : public Encoder
{
public:
	LZEncoder(Encoder* encoder_);
	virtual ~LZEncoder();
	virtual byte* getPage();
    virtual int getNumValsPerPage(){return 0;} //I don't think it is necessary
	virtual short getValSize(){return 0;}//I don't think it is necessary
	virtual int getBufferSize(){return bufferSize;}
	virtual unsigned int getStartPos(){return 0;}//I don't think it is necessary
	virtual unsigned int getPageSize();
protected:
	int bufferSize;
	byte* page;
	byte* buffer;
	Encoder* encoder;
	unsigned int sizeCompressedData;
};

#endif //_LZENCODER_H_
