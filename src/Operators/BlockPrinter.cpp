#include "BlockPrinter.h"

BlockPrinter::BlockPrinter(DataSource* dataSrc_[], int numCols_[], int numSrcs_, char* fileName_)
{
	using namespace std;
	dataSrc=dataSrc_;
	numCols=numCols_;
	numSrcs=numSrcs_;
	nprinted = 0;
	blks = NULL;

	if (fileName_==NULL) {
		stdOut=true;					
	}
	else {
		outstream.open(fileName_);  
		outstream.precision(15);

		stdOut=false;
	}
}

BlockPrinter::~BlockPrinter()
{
	outstream.close();	
}

void BlockPrinter::printColumns(bool skip_output) {
  using namespace std;
  int totalNumCols=0;
  int currCol;
  bool success=true;
  //PosBlock* posB;
  //Operator* PosFilter;
  MultiPosFilterBlock* posFilter;
  
  for (int i=0; i<numSrcs; i++) {
    totalNumCols+=numCols[i];
  }
  blks = new Block*[totalNumCols];
  for (int i=0;i<totalNumCols;i++)
    blks[i]=NULL;

  PosOperator* posOperator = new PosOperator();
  for(int i=0; i<numSrcs; i++){
	 //zklee: Currently only consider AND situtation
	 if (i>0)posOperator->addWhereOp('|'); 
	 posOperator->addPosBlock(dataSrc[i]->getPosOnPred());	 
  }
  posOperator->finishWhereOp();
	
  posFilter = posOperator->getPosFilter();
  posFilter->getNumValuesR();
  for(int i=0; i<numSrcs; i++)
	dataSrc[i]->setPositionFilter(posFilter);
  
  while (success) {
    currCol=0;
    for (int i=0; i<numSrcs; i++) {
      for (int j=0; j < numCols[i]; j++) {
		success &= printEntryForColumn(dataSrc[i], j, currCol, currCol==totalNumCols-1, skip_output); 
		currCol++;
      }
    }
	++nprinted;
  }
  delete[] blks;
}

void BlockPrinter::printColumnsWithPosition() {
  for (int i=0; i<numSrcs; i++) {
    printColumnsWithPositionForSrc(dataSrc[i], numCols[i]); 
  }  
}

 bool BlockPrinter::printEntryForColumn(DataSource* dataSrc_, int currCol, int currTotalCol, bool isLast, bool skip_output) {
  using namespace std;
  if (dataSrc_==NULL) return false;
  if ((blks[currTotalCol] == NULL) || (!blks[currTotalCol]->hasNext())) {
    blks[currTotalCol]=dataSrc_->getNextValBlock(currCol);
    if (blks[currTotalCol] == NULL)
      return false;
  }
  ValPos* pair=blks[currTotalCol]->getNext();
// DSM disable printing
	if (!skip_output) {
	  if (stdOut) {
	    pair->printVal(&cout);
	    if (isLast) 
	      cout << endl;
	    else
	      cout << ",";
	    }
	  else 
	    {
	    pair->printVal(&outstream);
	    if (isLast) 
	      outstream << endl;
	    else
	      outstream << ",";
	    }
	}

  //BUG FIX: hasNext might be false now, but true next time this is called since the data blks[currTotalCol] is pointing to can be updated. However, we want to force getNextValBlock to be called to update the pointer, so put NULL in blks[currTotalCol] if !hasNext
  
  if (!blks[currTotalCol]->hasNext()) 
    blks[currTotalCol] = NULL;
  return true;

 }
void BlockPrinter::printColumnsForSrc(DataSource* dataSrc_, int numCols_) {
  using namespace std;
  
  if (dataSrc_==NULL) return;
  Block* blks[numCols_];
  for (int i=0; i<numCols_; i++) {
    blks[i]=dataSrc_->getNextValBlock(i);
  }
  while (blks[0]!=NULL) {
    for (int j=0; j<blks[0]->getSize(); j++) {
      for (int col=0; col<numCols_-1; col++) {
	ValPos* pair=blks[col]->getNext();

	if (stdOut) {
	  pair->printVal(&cout);
	  cout << ",";
	}
	else 
	  {
	    //int before = StopWatch::ticks();
	    pair->printVal(&outstream);
	    outstream << ",";
	    //int after = StopWatch::ticks();
	    //totalPrint+= (after-before)*StopWatch::secondsPerTick();
	    //cout << (after-before ) << " -------> " << (after-before ) << " seconds per tick? " << StopWatch::secondsPerTick() << endl;				  
	  }
      }	
      ValPos* pair=blks[numCols_-1]->getNext();
      if (stdOut) {
	pair->printVal(&cout);
	cout << endl;
      }
      else 
	{
	  pair->printVal(&outstream);
	  //int before = StopWatch::ticks();
	  outstream << endl;
	  //int after = StopWatch::ticks();
	  
	  //totalPrint+= (after-before)*StopWatch::secondsPerTick();
	  //cout << (after-before ) << " -------> " << (after-before ) << endl;
	}
    }
    for (int i=0; i<numCols_; i++) {
      blks[i]=dataSrc_->getNextValBlock(i);
    }	
  }
  
  //cout << " Found " << totalPrint << " Seconds for printing itself " << endl << " VS COUNT " << count << endl;
}

void BlockPrinter::printColumnsWithPositionForSrc(DataSource* dataSrc_, int numCols_) {
  using namespace std;

  if (dataSrc_==NULL) return;
  Block* blks[numCols_];
  for (int i=0; i<numCols_; i++) {
    blks[i]=dataSrc_->getNextValBlock(i);
  }
  while (blks[0]!=NULL) {
    for (int j=0; j<blks[0]->getSize(); j++) {
      for (int col=0; col<numCols_-1; col++) {
        ValPos* pair=blks[col]->getNext();
        if (stdOut) {
	  cout << "(";
	  pair->printVal(&cout);
          cout << "," <<  pair->position << ")"<< "\t\t";
	}
        else 
        {
          outstream << "(";
	  pair->printVal(&outstream);
	  cout << "," <<  pair->position << ")"<< "\t\t";
        }
      } 
      ValPos* pair=blks[numCols_-1]->getNext();
      if (stdOut) {
	cout << "(";
	pair->printVal(&cout);
	cout << "," <<  pair->position << ")"<< "\t\t" << endl;
      }
      else 
      {
        outstream << "(";
	pair->printVal(&outstream);
	cout << "," <<  pair->position << ")"<< "\t\t" << endl;
      }
    }
    for (int i=0; i<numCols_; i++) {
      blks[i]=dataSrc_->getNextValBlock(i);
    } 
  }
}
