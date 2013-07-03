#ifndef _StringDataSource_H_
#define _StringDataSource_H_

#include "../common/DataSource.h"

class StringDataSource : public DataSource
{
	public:
	// Construct a datasource that :
		//		-access data through am_
		//		-writes blocks using writer_
		StringDataSource(AM* am_, bool valSorted_, bool isROS);

		// Destructor
		virtual ~StringDataSource();

		// Gets the Posiotion block on the predication 
		virtual	MultiPosFilterBlock* getPosOnPred();

		//printColumn: print to string instead of stdout
		//used for testing purpose.
		virtual void printColumn();

	protected:


	private:	

};

#endif //_StringDataSource_H_
