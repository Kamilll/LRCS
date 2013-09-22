#include "StringValPos.h"

StringValPos::StringValPos( ) {
	type=ValPos::STRINGTYPE;
	valsize=0;
	position=0;
	value=NULL;
}

StringValPos::StringValPos(unsigned short vs_) {
	type = ValPos::STRINGTYPE;
	valsize=vs_;
	position=0;
	value=new byte[vs_];
}

StringValPos::StringValPos(unsigned int pos_, byte* value_, unsigned short vs_) {
	type = ValPos::STRINGTYPE;
	valsize=vs_;
	position=0;
	value=new byte[vs_];
	memcpy(value, value_, valsize);
}

StringValPos::StringValPos(StringValPos& valpos_) {
	assert(valpos_.type == ValPos::STRINGTYPE);
	type=valpos_.type;	
	valsize=valpos_.valsize;
	position=valpos_.position;
	if(valsize > 0)
		set(valpos_.value, valsize);
	else
		value = NULL;	
}

ValPos* StringValPos::clone()
{
	ValPos* newVP = new StringValPos(*this);
	return newVP;
}

StringValPos::~StringValPos()
{
	delete[] value;
}

unsigned short StringValPos::getSize() {
	return valsize;
}

void StringValPos::set(byte* value_) {
	if (valsize!=0)
		memcpy(value, value_, valsize);  
}

void StringValPos::set(byte* value_, unsigned short valSize_) {
	valsize = valSize_;
	if (valsize!=0){
		value = new byte[valsize];
		memcpy(value, value_, valsize+1);
	}
}

void StringValPos::set(unsigned int pos_, byte* value_) {
	position=pos_;
	if (valsize!=0 && value_ != NULL)
		memcpy(value, value_, valsize);
}

void StringValPos::set(unsigned int pos_, byte* value_, unsigned short valSize_) {
	position=pos_;
	valsize = valSize_;
	if(valsize!=0){
		value = new byte[valsize];
		memcpy(value, value_, valsize);
	}
}

void StringValPos::printVal(std::ostream* o) {
	for (int i = 0; i < valsize; i++) {
		if (value[i])
			(*o) << value[i];
		else
			return;
	}

}

//generic function overloads
bool StringValPos::operator> (ValPos* vp) {
	if ((vp->type != ValPos::STRINGTYPE) || (vp->getSize()!=valsize))
		throw new UnexpectedException("Incompatible types");
	return memcmp(value, vp->value, valsize) > 0;
}

bool StringValPos::operator>= (ValPos* vp) {
	if ((vp->type != ValPos::STRINGTYPE) || (vp->getSize()!=valsize))
		throw new UnexpectedException("Incompatible types");
	return memcmp(value, vp->value, valsize) >= 0;
}

bool StringValPos::operator== (ValPos* vp) {
	if ((vp->type != ValPos::STRINGTYPE) || (vp->getSize()!=valsize))
		throw new UnexpectedException("Incompatible types");
	return memcmp(value, vp->value, valsize) == 0;
}

bool StringValPos::operator< (ValPos* vp) {
	if ((vp->type != ValPos::STRINGTYPE) || (vp->getSize()!=valsize))
		throw new UnexpectedException("Incompatible types");
	return memcmp(value, vp->value, valsize) < 0;
}

bool StringValPos::operator<= (ValPos* vp) {
	if ((vp->type != ValPos::STRINGTYPE) || (vp->getSize()!=valsize))
		throw new UnexpectedException("Incompatible types");
	return memcmp(value, vp->value, valsize) <= 0;
}

bool StringValPos::operator!= (ValPos* vp) {
	if ((vp->type != ValPos::STRINGTYPE) || (vp->getSize()!=valsize))
		throw new UnexpectedException("Incompatible types");
	return memcmp(value, vp->value, valsize) != 0;
}