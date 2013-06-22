/* Copyright (c) 2005, Regents of Massachusetts Institute of Technology, 
 * Brandeis University, Brown University, and University of Massachusetts 
 * Boston. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *   - Neither the name of Massachusetts Institute of Technology, 
 *     Brandeis University, Brown University, or University of 
 *     Massachusetts Boston nor the names of its contributors may be used 
 *     to endorse or promote products derived from this software without 
 *     specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef DATASOURCE_H
#define DATASOURCE_H

class Decoder;

#include "Util.h"
#include "Constants.h"
#include "Interfaces.h"
#include "../Wrappers/PosType2Block.h"
#include "../Wrappers/Decoder/Decoder.h"
#include "../Wrappers/PosRLEBlock.h"
#include "BlockWithPos.h"
#include "../Wrappers/MultiBlock.h"
#include <map>
#include <sstream>

#include "../Util/StringUtil.h"
#include "../Wrappers/Decoder/PosDecoder.h"
#include "../Wrappers/Decoder/StringDecoder.h"
#include "../Wrappers/MultiSBlock.h"
// Abstract class that:
//	-Gets data from the AM
//	-Holds data in memory
// 	-Provides filtering of a column on a bitstring
class DataSource : public Operator {
public:
	// Construct a datasource that :
	//		-access data through am_
	//		-writes blocks using writer_
	DataSource(AM* am_, bool isROS_);
	DataSource(AM* am_, bool isROS_, bool valSorted_, Decoder* decoder_);	
	
	// Destructor
	virtual ~DataSource();
	
	// Gets the next value block from the operator 
	virtual	Block* getNextValBlock(int colIndex_);

	// Gets the Posiotion block on the predication 
	virtual	PosBlock* getPosOnPred();
	
	// Skips to the first block with this value
	// Returns NULL if the value is outside the range of the column
	virtual Block* skipToValBlock(int colIndex_, int val_){return NULL;}
	
	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_){return NULL;}
	
	// Skips to the first block with this position
	// Returns NULL if the value is outside the range of the column
	virtual PosBlock* skipToPosBlock(int colIndex_, int pos_){return NULL;}
		
	// Changes the RHS binding for this datasource
	virtual void changeRHSBinding(ValPos* rhs_);

	// sets a new predicate for this datasource
	virtual void setPredicate(Predicate* pred_);
	
	// Sets a filter for postions
	virtual void setPositionFilter(Operator* bitstringDataSource_, int colIndx_);

	//printColumn, but print to string instead of stdout
	//used for testing purpose.
	virtual void printColumn(stringstream& ss){ss << "DATASOURCE PRINT" << endl;};
	
	// Gets the maximum position in the data source
	virtual int getLastPosition();

	//skips to page along the value index
	virtual const void* skipToPageValue(char* key);

	//skips to page along the position index
	virtual const void* skipToPagePosition(int key);

	//gets the next page along value index	
	virtual const void* getNextPageValue( );

	//gets the next page along position index
	virtual const void* getNextPagePosition( );

protected:
	// Main method to get a page given the predicates and filters
	virtual byte* getRightPage(Decoder* decoder_, bool valSorted_, bool posIndexPrimary_);
	virtual bool skipToRightPosOnPage(Decoder* decoder_);

	bool checkOutBlock(bool& done_);
	void init(AM* am_, bool isROS_);
	Block* getDecodedBlock(Decoder* decoder);
	
	AM* getBlocksCurrAM;	
	Decoder* getBlocksCurrDecoder;
	byte* getBlocksCurrPage;


	// Filtering methods
	bool getPosFilterStartEnd(unsigned int& posFilterStart_, unsigned int& posFilterEnd_, bool getNextFilterRange_);

	// Access method used to get values
	AM* am;
	// Access method used to get positions
	bool posPrimaryIndex;

	// Have we altered filters or predicates?
	bool posFilterChanged;
	bool predChanged;
	bool predLookup;
	
	// Filter streams
	Operator* posFilter;
	unsigned int posColIndex;
	
	Predicate* pred;
	
	// Position Filter flags		
	bool posFilterInit;
	bool posFilterPosSorted;
	bool posFilterCurrBlockPosSorted;
	bool posFilterCurrBlockPosContig;

	// Current Position Block
	PosBlock* currPosBlock;
	bool havePosBlock;
	// Current range for position filtering
	unsigned int posFilterStart;
	unsigned int posFilterEnd;
	unsigned int posSkippedTo; // last position looked up in position index
	
	bool isROS;
	bool getNextNextTime;
	bool RLEAlways;

	bool valSorted;
	bool useWP;
	Decoder* decoder;
	Block* currBlock;
	BlockWithPos* currBlockWP;
	RLEBlock* theRLEBlock;
	RLETriple* currRLETriple;
	//MultiBlock* valOutBlock;
	PosBlock* posOutBlock;
	RLETriple* posOutTripleOnPred;
	PosBlockIter* pba;
	Type2Block* matchedPred;
	PosType2Block* matchedPredPos;
	
private:
	virtual byte* getPage(Decoder* decoder_);
	virtual byte* getPageOnPred(Decoder* decoder_, bool valSorted_);
	virtual byte* getPageOnPos(Decoder* decoder_, bool posIndexPrimary_);
	virtual bool getPosOnPredValueSorted(ValPos* rhsvp_, ValPos* tempVP_);
	virtual bool getPosOnPredValueUnsorted(ROSAM* am_,ValPos* rhsvp_, ValPos* tempVP_);
	
	// used by getPosFilterStartEnd()
	void getNextPosFilterBlock();

	bool firstCall = true;
	bool filterFinished = false;
};

#endif // DATASOURCE_H
