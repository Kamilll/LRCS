#ifndef DATASOURCE_H
#define DATASOURCE_H

#include "Interfaces.h"
#include "MultiPosFilterBlock.h"
#include "../Util/StringUtil.h"
#include "../Wrappers/MultiBlock.h"
#include "../Wrappers/Decoder/Decoder.h"
#include "../Wrappers/Decoder/PosDecoder.h"
#include "../Wrappers/Decoder/StringDecoder.h"
#include "../Operators/MultiPosFilterCursor.h"

// Super class that:
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

		// Changes the RHS binding for this datasource
		virtual void changeRHSBinding(ValPos* rhs_);

		// sets a new predicate for this datasource
		virtual void setPredicate(Predicate* pred_);

		// Sets a filter for postions
		virtual void setPositionFilter(MultiPosFilterBlock* bitstringDataSource_);

		// Gets the next value block from the operator 
		virtual	Block* getNextValBlock(int colIndex_);

		// Gets the next position block (bitstring of positions) from the operator
		virtual PosBlock* getNextPosBlock(int colIndex_){return NULL;}

		// Gets the Posiotion block on the predication 
		virtual	MultiPosFilterBlock* getPosOnPred();

		// Gets the maximum position in the data source
		virtual int getLastPosition();

	protected:
		AM* am;	
		Predicate* pred;
		Decoder* decoder;
		MultiBlock* currBlock;

		bool posPrimaryIndex;
		bool isROS;
		bool valSorted;	
		bool posFilterChanged;
		bool predChanged;

		// Filter streams
		MultiPosFilterBlock* posFilter;
		MultiPosFilterBlock* matchedPredPos;
		RLETriple* posOutTripleOnPred;
		MultiPosFilterCursor* filterCursor;

		// Main method to get a page given the predicates and filters
		virtual byte* getRightPage();

		//Skips to the right position on a page that buffered
		virtual bool skipToRightPosOnPage(unsigned int pos_);

		//skips to page along the value index
		virtual const void* skipToPageValue(char* key);

		//skips to page along the position index
		virtual const void* skipToPagePosition(int key);

		//gets the next page along value index	
		virtual const void* getNextPageValue( );

		//Decode the page get from BDB
		Block* getDecodedBlock();
		Block* getDecodedBlock(Decoder* decoder_);
		virtual byte* getPage();
		virtual byte* getPageOnPos();
		virtual bool getPosOnPredValueSorted(ValPos* rhsvp_, ValPos* tempVP_);
		virtual bool getPosOnPredValueUnsorted(ROSAM* am_,ValPos* rhsvp_, ValPos* tempVP_);

	private:
		void init(AM* am_, bool isROS_);
		bool firstCall;
};

#endif // DATASOURCE_H
