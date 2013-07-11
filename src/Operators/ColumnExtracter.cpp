#include "ColumnExtracter.h"
#include <stdlib.h>
using namespace std;

ColumnExtracter::ColumnExtracter(string fileName, int c_index, bool FORCE_REBUILD)
{
  bb = new BasicBlock(true, true, true);
  //assume int type for now ... fix later
  p = new IntValPos();
  curPos = 1; 


  string sortedColumn = extractColumn( fileName, c_index, FORCE_REBUILD );
  infile = new ifstream( sortedColumn.c_str());


}

string ColumnExtracter::extractColumn( string fileName, int index, bool FORCE_REBUILD )
{
  char* buff = new char[256];
  if (FORCE_REBUILD)
    sprintf(buff, "cat %s | awk '{print $%d}' > %s.%d", fileName.c_str(), index, fileName.c_str(), index);
  else
    sprintf(buff, "./filechecker.sh %s %d %s.%d", fileName.c_str(), index, fileName.c_str(), index);

  system(buff);
  sprintf(buff, "%s.%d", fileName.c_str(), index);
  string s = buff;
  delete[] buff;
  return s;
}


ColumnExtracter::~ColumnExtracter()
{
  delete bb;
  delete infile;
  delete p;
  //delete bb;
  //delete infile;
}
	
Block* ColumnExtracter::getNextValBlock(int colIndex_) {
  if (colIndex_!=0) return NULL;
  string line;
  int curVal;
  if (!(getline (*infile, line)))
    return NULL;
  curVal = atoi( line.c_str() );
  p->set(curPos, (byte*)&curVal);
  //p->value = curVal;
  //p->position = curPos;
  curPos++;
  bb->setValue(p);
  return bb;
}
	
PosBlock* ColumnExtracter::getNextPosBlock(int colIndex_) {
	if (colIndex_!=0) return NULL;
	return NULL;	
}
	
