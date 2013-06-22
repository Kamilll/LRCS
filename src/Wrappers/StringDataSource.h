/* Copyright (c) 2005, Regents of Massachusetts Institute of Technology, 
 * Brandeis University, Brown University, and University of Massachusetts 
 * Boston. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *   - Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the 
 *     documentation and/or other materials provided with the distribution.
 *   - Neither the name of Massachusetts Institute of Technology, 
 *     Brandeis University, Brown University, or University of 
 *     Massachusetts Boston nor the names of its contributors may be used 
 *     to endorse or promote products derived from this software without 
 *     specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
//Uncompressed string data sources
#ifndef _StringDataSource_H_
#define _StringDataSource_H_

#include "../common/DataSource.h"
#include "Decoder/StringDecoder.h"
#include "BasicBlock.h"
#include "../common/BlockWithPos.h"
#include "PosBasicBlock.h"
#include "PosMultiBlock.h"
#include "PosType2Block.h"
#include "RLETriple.h"
#include "../common/UnimplementedException.h"
#include "PosRLEBlock.h"
#include <vector>
#include "../Util/StringUtil.h"
#include "Decoder/PosDecoder.h"
#include "MultiSBlock.h"
#include "MultiBlock.h"

class StringDataSource : public DataSource
{
public:
	// Construct a datasource that :
	//		-access data through am_
	//		-writes blocks using writer_
	StringDataSource(AM* am_, bool valSorted_, bool isROS);
	
	// Destructor
	virtual ~StringDataSource();

	// Gets the next position block (bitstring of positions) from the operator
	virtual PosBlock* getNextPosBlock(int colIndex_);

	//printColumn: print to string instead of stdout
	//used for testing purpose.
	virtual void printColumn();

protected:
	bool getPosBlockDB(ROSAM* am_);
	// checks the current out block to see if it matches pred and filters
	bool init;	
	
	byte* currPage;
	MultiSBlock* currBlock;

	
private:	
	bool firstCall = true;

};

#endif //_StringDataSource_H_
