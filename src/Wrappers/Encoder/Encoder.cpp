#include "Encoder.h"
#include "PosEncoder.h"

Encoder::Encoder(Operator* dataSrc_, int colIndex_, PosEncoder* posEncoder_)
{
	dataSrc=dataSrc_;
	posEncoder=posEncoder_;
	colIndex=colIndex_;
	mode=Encoder::UNINIT;
}

Encoder::~Encoder()
{

}


byte* Encoder::getPage() {
	if (mode==Encoder::UNINIT) {	
		mode=Encoder::PULL;	
	}
	return NULL;
}

bool Encoder::writeVal(int val_, unsigned int pos_) {
	if (mode==Encoder::PULL) {
		throw new UnexpectedException("Encoder: writeVal(int,int): Encoder set as pull interface");		
	}
	else {	
		mode=Encoder::PUSH;
	}
	return false;
}

byte* Encoder::getEncodedPosPage(byte** posValue_, unsigned int** posPageSize_){
	assert(posEncoder != NULL);
	//*posValue_ = new byte[*ssizePtr];
	byte* _posValue = NULL;
	byte* _page = posEncoder->getPageAndValue(&_posValue);
	*posValue_ = _posValue;
	if(_page != NULL){
		unsigned int pageSize = BLOCK_SIZE*(*(unsigned int*)_page);
		*posPageSize_ = &pageSize;
		return _page;
	}else return NULL;
}