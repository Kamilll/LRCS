#ifndef _Query3S_H_
#define _Query3S_H_

#include "../common/Interfaces.h"
#include "../AM/AM.h"
#include "../Operators/SColumnExtracter.h"
#include "../Wrappers/Encoder/StringEncoder.h"
#include "../AM/PageWriter.h"
#include "../common/Constants.h"
#include "../common/Util.h"
#include "../Wrappers/CodingException.h"
#include "../Wrappers/Decoder/StringDecoder.h"
#include "../Wrappers/StringDataSource.h"
#include "../common/DataSource.h"
#include "../Paths.h"
#include "../Operators/BlockPrinter.h"
#include "../AM/PagePlacer.h"
#include <ctime>
#include "UnitTest.h"

class Query3S : public UnitTest
{
public:
	Query3S();
	virtual ~Query3S();
	bool run();

};

#endif //_Query3S_H_
