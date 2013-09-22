#ifndef _BitmapDataSource_H_
#define _BitmapDataSource_H_

#include "../common/DataSource.h"

class BitmapDataSource : public DataSource
{
	public:
	// Construct a datasource that :
		//		-access data through am_
		//		-writes blocks using writer_
		BitmapDataSource(AM* am_, bool valSorted_, bool isROS, Decoder* decoder_);

		// Destructor
		virtual ~BitmapDataSource();

		// Gets the Posiotion block on the predication 
		virtual	MultiPosFilterBlock* getPosOnPred();

		virtual bool BitmapDataSource::skipToRightPosOnPage(unsigned int pos_);

	protected:
        bool getPosOnPredValueUnsorted(ROSAM* am_,ValPos* rhsvp_, ValPos* tempVP_);
 
	private:	

};

#endif //_BitmapDataSource_H_
