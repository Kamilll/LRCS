#include "Encoder.h"

Encoder::Encoder(Operator* dataSrc_, int colIndex_)
{
	dataSrc=dataSrc_;
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
