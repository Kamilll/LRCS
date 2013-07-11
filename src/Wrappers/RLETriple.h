#ifndef RLETRIPLE_H
#define RLETRIPLE_H
#include "CodingException.h"
#include "../common/ValPos.h"

class RLETriple {
	public:
		RLETriple() { value=NULL; startPos=0; reps=0;}
		RLETriple(ValPos* value_, unsigned int startPos_, unsigned int reps_) 
			: value(value_==NULL?NULL:value_->clone()), startPos(startPos_), reps(reps_) {
				if (reps==0)
					throw new CodingException("RLETriple: error, reps==0");
			}
		RLETriple(const RLETriple& trip_) {
			value=(trip_.value==NULL?NULL:trip_.value->clone());
			startPos=trip_.startPos;
			reps=trip_.reps;
		}
		virtual ~RLETriple(){delete value;}
		void setTriple(ValPos* val_, int pos_, int reps_) {
			delete value;
			value=(val_==NULL?NULL:val_->clone());
			startPos=pos_;
			reps=reps_;
			if (reps==0)
				throw new CodingException("RLETriple: error, reps==0");
		}
		void setTriple(RLETriple* triple_) {
			if (triple_==NULL) 
				throw new CodingException("RLETriple: setTriple with NULL triple");
			else {
				delete value;
				value=(triple_->value==NULL?NULL:triple_->value->clone());
				startPos=triple_->startPos;
				reps=triple_->reps;	
			}
		}

		void setNull() { value = NULL; startPos=0; reps=0;}

		bool isNull() {
			return (value ==NULL && startPos==0 && reps ==0);
		}

		ValPos* value;
		unsigned int startPos;
		unsigned int reps;	
};

#endif // RLETRIPLE_H

