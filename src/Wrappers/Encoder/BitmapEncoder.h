#ifndef _BitmapEncoder_H_
#define _BitmapEncoder_H_

#include "PosEncoder.h"
#include "../../common/PosBlock.h"
#include<queue>
#include<map>
using namespace std;

/*Bitmap Encoder
 *encoder positions into bitmaps. 
 *Only used for positions not values. 
 */
class BitmapEncoder : public PosEncoder
{
public:
	BitmapEncoder(int valSize_, int bfrSizeInBits_);
	virtual ~BitmapEncoder();
	//virtual byte* getPage( ){return NULL;}
	virtual byte* getPageAndValue(byte** value_);

	virtual void addValPos(ValPos* vp_);
	virtual void purgeMap2Queue();
protected:
	map<string, byte*> pageMap;
	map<string, PosBlock*> blockMap;

	struct ele{
       byte* value;
	   byte* page;
	};

	queue<ele> outPutQ;
	int bufferSizeInBytes;
	short valSize;

	void initANewPage(string valueStr);
	void initANewBlock(string valueSt, ValPos* vp_);
	bool appendBlocktoPage(byte* page_, PosBlock* posBlock_);
	void putPage2Queue(byte* page_, string valueStr_);
};

#endif //_BitmapEncoder_H_
