#ifndef _Query4S_H_
#define _Query4S_H_

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

class Query4S : public UnitTest
{
public:
	Query4S();
	virtual ~Query4S();
	bool run();

};

#endif //_Query4S_H_
