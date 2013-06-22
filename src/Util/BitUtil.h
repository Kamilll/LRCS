#ifndef __BIT_UTIL_H
#define __BIT_UTIL_H
class BitUtil{
public:
	BitUtil();
	virtual ~BitUtil();
	static void compute_bits_in_16bits();
	static int precomputed16_bitcount(unsigned int n);


protected:		
	static int iterated_bitcount(unsigned int n);
	static char* bits_in_16bits;
};

#endif //_BIT_UTIL_H