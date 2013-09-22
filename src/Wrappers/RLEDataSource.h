#ifndef _RLEDataSource_H_
#define _RLEDataSource_H_

#include "../common/DataSource.h"

class RLEDataSource : public DataSource
{
	public:
	// Construct a datasource that :
		//		-access data through am_
		//		-writes blocks using writer_
		RLEDataSource(AM* am_, bool valSorted_, bool isROS, Decoder* decoder_);

		// Destructor
		virtual ~RLEDataSource();

		// Gets the Posiotion block on the predication 
		virtual	MultiPosFilterBlock* getPosOnPred();

		//virtual bool RLEDataSource::skipToRightPosOnPage(unsigned int pos_);

	protected:
        bool getPosOnPredValueUnsorted(ROSAM* am_,ValPos* rhsvp_, ValPos* tempVP_);
 
	private:	

};

#endif //_RLEDataSource_H_
