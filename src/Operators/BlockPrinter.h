#ifndef _BLOCKPRINTER_H_
#define _BLOCKPRINTER_H_

#include "Operator.h"
#include "../common/Block.h"
#include "../Util/StopWatch.h"
#include <iostream>
#include <fstream>
#include "../common/MultiPosFilterBlock.h"
#include "../common/DataSource.h"
#include "PosOperator.h"

class BlockPrinter
{
	public:
		BlockPrinter(DataSource* dataSrc_[], int numCols_[], int numSrcs_, char* fileName_);
		virtual ~BlockPrinter();
		void printColumns(bool skip_output = false);
		void printColumnsWithPosition();
		int getNumPrinted() { return nprinted; }
	protected:
		void printColumnsForSrc(DataSource* dataSrc_, int numCols_);
		bool printEntryForColumn(DataSource* dataSrc_, int currCol, int currTotalCol, bool isLast, bool skip_output);
		void printColumnsWithPositionForSrc(DataSource* dataSrc_, int numCols_);
		DataSource** dataSrc;
		std::ofstream outstream;
		int* numCols;
		bool stdOut;
		int numSrcs;
		int nprinted;
		Block** blks;
		int totalCols;
};

#endif //_BLOCKPRINTER_H_
