#include "BitUtil.h"

char* BitUtil::bits_in_16bits = new char[0x1u << 16];

BitUtil::BitUtil(){

}

BitUtil::~BitUtil(){

}

int BitUtil::iterated_bitcount (unsigned int n)
{
    int count=0;    
    while (n)
    {
        count += n & 0x1u ;    
        n >>= 1 ;
    }
    return count ;
}

void BitUtil::compute_bits_in_16bits ()
{
	if ((int)bits_in_16bits[1]!=1){ //check wheteher computed or not
		unsigned int i ;    
		for (i = 0; i < (0x1u<<16); i++)
			bits_in_16bits [i] = iterated_bitcount (i) ;
	}
}

int BitUtil::precomputed16_bitcount (unsigned int n)
{
    // works only for 32-bit int 
    return bits_in_16bits [n         & 0xffffu]
        +  bits_in_16bits [(n >> 16) & 0xffffu] ;
}