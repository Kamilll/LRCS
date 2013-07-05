#ifndef _ENCODER_H_
#define _ENCODER_H_
#include "../../common/Interfaces.h"

class Encoder
{
public:
	Encoder(Operator* dataSrc_, int colIndex_);
	virtual ~Encoder();	
	virtual byte* getPage();
	virtual bool writeVal(int val_, unsigned int pos_);
	virtual ValPos** getValueIndex() { throw new UnexpectedException("Should only be called for Type2 style encoders"); }
    virtual int getNumValsPerPage()=0;
	virtual short getValSize()=0;
	virtual int getBufferSize()=0;
	virtual unsigned int getStartPos()=0;
	virtual unsigned int getPageSize()=0;
protected:
	enum Mode { UNINIT, PULL, PUSH };
	Mode mode;
	Operator* dataSrc;
	int colIndex;
};

#endif //_ENCODER_H_
