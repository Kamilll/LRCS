#ifndef _STRINGREADER_H_
#define _STRINGREADER_H_
#include "../../common/Constants.h"
#include "../../common/Util.h"
#include "../../common/StringValPos.h"
class StringReader
{
	public:
		StringReader();
		virtual ~StringReader();
		bool readString(char* value_);
		bool skipToValPos(ValPos* vp_);
		StringValPos* readFirst();
		StringValPos* readLast();
		void unReadString();
		bool hasNextString();
		void resetPos();
		bool skipToStringPos(unsigned int intPos_);	
		int getCurrentPos();

		void setBuffer(byte* buffer_, int numBitsInBuffer_);
		void setStringSize(int stringSize_);
		byte* getBuffer(int& numVals);			
	protected:
		byte* buffer;
		byte* currPosPtr;
		unsigned int numBytesInBuffer;
		unsigned int currPos;
		int stringSize;
	private:
		bool biSearch(ValPos* vp_, unsigned int begin, unsigned int last);
};

#endif //_STRINGREADER_H_
