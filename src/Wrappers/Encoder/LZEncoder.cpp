#include "LZEncoder.h"
//#include "PosEncoder.h"
#include <lzo1x.h>
#include <lzoutil.h>

LZEncoder::LZEncoder(Encoder* encoder_): Encoder(NULL, 1,NULL)
{
	assert(encoder_ != NULL);
	encoder = encoder_;		
	posEncoder = encoder_->posEncoder;
	page=new byte[PAGE_SIZE];
}

LZEncoder::~LZEncoder()
{
}
byte* LZEncoder::getPage() {
	buffer=encoder->getPage();	
	if(buffer != NULL){
		//startPos = encoder->getStartPos();
		//numValsPerPage=encoder->getNumValsPerPage();
		//valsize=encoder->getValSize();
		bufferSize = encoder->getBufferSize();
	}else{
		return NULL;
	}
	memset(page, 0, PAGE_SIZE);

	if(lzo_init() != LZO_E_OK)
		throw UnexpectedException("LZEncoder: Error, lzo initialization error!");

	lzo_byte *compressedData = 
		(lzo_bytep) lzo_malloc(bufferSize + bufferSize / 64 + 16 + 3);
	//new byte[numValsPerPage*valsize + numValsPerPage*valsize / 64 + 16 + 3];
	lzo_byte *wrkmem = (lzo_bytep) lzo_malloc(LZO1X_1_MEM_COMPRESS);

	int r = 0;		
	r = lzo1x_1_compress((const unsigned char*)buffer, bufferSize, compressedData, &sizeCompressedData, wrkmem);
	if (r != LZO_E_OK) {
		throw new UnexpectedException("LZEncoder: compress error!");
	}


	// Writing header for decoder to use
	*((int*) page)=sizeCompressedData;
	*((int*) (page+sizeof(int)))=bufferSize;
	//*((int*) (page+2*sizeof(int)))=startPos;
	//*((int*) (page+3*sizeof(int)))=valsize;

	if (sizeCompressedData>PAGE_SIZE-2*sizeof(int)) 
		throw CodingException("LZEncoder: Error, compressed data larger than what we can fit on page");
    
	memcpy(page+2*sizeof(int), compressedData, sizeCompressedData);
	delete[] compressedData;
	lzo_free(wrkmem);
	return page;
}

unsigned int LZEncoder::getPageSize() {
   return (2*sizeof(int) + sizeCompressedData);
}
