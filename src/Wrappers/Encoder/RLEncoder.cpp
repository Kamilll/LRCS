#include "RLEncoder.h"
/*
 *RLEBlocks	
 *  +-----------------------------------+
 *	|StartPostion(int)|Repetion(int)
 *	+-----------------------------------+
 */
RLEncoder::RLEncoder() : PosEncoder()
{
	currTriple = NULL;
}

RLEncoder::~RLEncoder()
{
   
}

byte* RLEncoder::getPageAndValue(byte** value_, unsigned int** valSize_) {
	if(!outPutQ.empty()){
		outTriple = outPutQ.front();
		outPutQ.pop();
		*value_ = outTriple->value->value;
		unsigned int _valSize = 2*sizeof(int);
		*valSize_ = &_valSize;
		byte* page = new byte[2*sizeof(int)];
		unsigned int* _startPos = (unsigned int*)page;
		unsigned int* _reps = (unsigned int*)(page+sizeof(int));
		*_startPos = outTriple->startPos;
		*_reps = outTriple->reps;
		return page;
	}else return NULL;
}

void RLEncoder::addValPos(ValPos* vp_){
    if (currTriple == NULL){
		currTriple = new RLETriple(vp_, vp_->position, 1);
	}else{		  
		if(*vp_ == currTriple->value){
		    currTriple->reps++;
		}else{
			putTriple2Queue(currTriple);
			currTriple = new RLETriple(vp_, vp_->position, 1);
		}
	}
}


void RLEncoder::putTriple2Queue(RLETriple* RLETriple_){
	outPutQ.push(RLETriple_);
}

void RLEncoder::purgeMap2Queue(){
	if (currTriple != NULL){
		putTriple2Queue(currTriple);
		currTriple = NULL;
	}
}