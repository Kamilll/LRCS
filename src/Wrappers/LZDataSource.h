#ifndef _LZDataSource_H_
#define _LZDataSource_H_

#include "../common/DataSource.h"

class LZDataSource : public DataSource
{
	public:
	// Construct a datasource that :
		//		-access data through am_
		//		-writes blocks using writer_
		LZDataSource(AM* am_, bool valSorted_, bool isROS, Decoder* decoder_);

		// Destructor
		virtual ~LZDataSource();

		// Gets the Posiotion block on the predication 
		virtual	MultiPosFilterBlock* getPosOnPred();

		//printColumn: print to string instead of stdout
		//used for testing purpose.
		virtual void printColumn();

	protected:


	private:	

};

#endif //_LZDataSource_H_
