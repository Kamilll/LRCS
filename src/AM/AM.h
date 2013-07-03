#ifndef __AM_H
#define __AM_H

#include <db_cxx.h>
#include <string>
#include <vector>
#include <map>
#include "AccessException.h"
#include "../common/Constants.h"
#include "../common/Util.h"

/**
 * Access BDB columns
 * Basically IntegerColumn with some modifications
 */
using namespace std;
class AM
{
  public:
    
  // destructor
  virtual ~AM();
  
  // Initializes the cursors (primary/secondary) for this table.
  virtual void initCursors()=0;
  
  // Gets the next page along particular index
  virtual const void* getNextPagePrimary()=0;
  virtual const void* getNextPageSecondary()=0;
  virtual const void* skipToPagePrimary(char* key)=0;
  virtual const void* skipToPageSecondary(char* key)=0;
	
  
  virtual char* getLastIndexValuePrimary()=0;
  virtual char* getLastIndexValueSecondary()=0;
  virtual string toString()=0;
  virtual string getTableName(){ return string("I am just an AM! "); };
  virtual string getColumnName()=0;
  virtual int getNumIndices()=0;
  
 protected:
  AM();
};

#endif // __AM_H
