#include "Block.h"

Block::Block() {
}

Block::Block(const Block& block_) {
	valsize = block_.valsize;
}

Block::~Block() {
}

void Block::setType(short type_) {
	delete vp;
	delete utilityVP;
	switch (type_) {
		case ValPos::INTTYPE:
			vp = new IntValPos();
			break;
		case ValPos::FLOATTYPE:
			vp = new FloatValPos();
			break;
		case ValPos::DOUBLETYPE:
			vp = new DoubleValPos();
			break;
		case ValPos::LONGTYPE:
			vp = new LongValPos();
			break;
		case ValPos::STRINGTYPE:
			vp = new StringValPos();		  
			break;
		default:
			throw new UnexpectedException("Unknown type.");
	}

	utilityVP = vp->clone();
	valsize = vp->getSize();
}

void Block::setType(ValPos* vp_) {
	delete vp;
	delete utilityVP;
	vp = vp_->clone();
	utilityVP = vp_->clone();
	valsize = vp_->getSize();
}
