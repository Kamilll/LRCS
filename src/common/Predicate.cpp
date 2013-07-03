#include "Predicate.h"

Predicate::Predicate(PredicateType type_) {
  predType=type_;
  if ((predType==OP_LESS_THAN) ||
      (predType==OP_LESS_THAN_OR_EQUAL) ||
      (predType==OP_EQUAL) ||
      (predType==OP_GREATER_THAN_OR_EQUAL) ||
      (predType==OP_GREATER_THAN))
    isBinary=true;
  else 
    isBinary=false;
  
  isBoundLHS=false;
  isBoundRHS=false;
  vp = NULL;
}

Predicate::Predicate(PredicateType type_, ValPos* rhs_) {
  (Predicate(type_));
  isBoundRHS=true;
  rhsVal=rhs_;
  vp = NULL;
}

Predicate::Predicate(const Predicate& pred) {
  this->lhsVal=pred.lhsVal->clone();
  this->rhsVal=pred.rhsVal->clone();
  this->isBinary=pred.isBinary;
  this->isBoundLHS=pred.isBoundLHS;
  this->isBoundRHS=pred.isBoundRHS;
  this->predType=pred.predType;	
  vp = NULL;
}

Predicate::~Predicate() {
  delete vp;
}

void Predicate::setRHS(ValPos* rhs_) {
  rhsVal=rhs_;
  isBoundRHS=true;	
}
void Predicate::setRHS(int rhsint) {
  vp = new IntValPos(0, rhsint);
  rhsVal=vp;
  isBoundRHS=true;	
}
void Predicate::setLHS(ValPos* lhs_) {
  lhsVal=lhs_;
  isBoundLHS=true;	
}
ValPos* Predicate::getLHS() {
  // TODO Throw exception if not bound
  return lhsVal;
}
ValPos* Predicate::getRHS() {
  // TODO Throw exception if not bound
  return rhsVal;
}

Predicate::PredicateType Predicate::getPredType() {
  return this->predType;
}	

bool Predicate::evalPredicate() {
  // TODO check arguements are bound	
  switch (predType) {
  case OP_LESS_THAN:
    return (*lhsVal<rhsVal);
  case OP_LESS_THAN_OR_EQUAL:
    return (*lhsVal<=rhsVal);			
  case OP_EQUAL:
    return (*lhsVal==rhsVal);			
  case OP_GREATER_THAN_OR_EQUAL:
    return (*lhsVal>=rhsVal);
  case OP_GREATER_THAN:
    return (*lhsVal>rhsVal);			
    //case OP_EVEN:
    //return (lhsVal%2==0);			
    //case OP_ODD:
    //return (lhsVal%2==1);			
  default:
    // TODO throw exception
    return false;
  }
  
}

bool Predicate::evalPredicate(int val) {
  // TODO check arguements are bound	
  switch (predType) {
  case OP_LESS_THAN:
    return (*lhsVal<val);
  case OP_LESS_THAN_OR_EQUAL:
    return (*lhsVal<=val);			
  case OP_EQUAL:
    return (*lhsVal==val);			
  case OP_GREATER_THAN_OR_EQUAL:
    return (*lhsVal>=val);
  case OP_GREATER_THAN:
    return (*lhsVal>val);			
    //case OP_EVEN:
    //return (lhsVal%2==0);			
    //case OP_ODD:
    //return (lhsVal%2==1);			
  default:
    // TODO throw exception
    return false;
  }
  
}
