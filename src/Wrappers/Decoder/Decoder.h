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
// Abstract class for all decoders
// Problems: migfer@mit.edu

#ifndef DECODER_H
#define DECODER_H

#include "../../common/Constants.h"
#include "../../common/Interfaces.h"
#include "../CodingException.h"

#define HEADER_SIZE 8


class Decoder{
public:
	Decoder();
	Decoder(byte* buffer_);
	Decoder(Decoder& decoder_); 
	virtual ~Decoder();
	
	virtual void setBuffer(byte* buffer_);
	virtual int getPageLengthBits()=0;
	virtual byte* getBuffer();
	
	virtual bool hasNextBlock()=0;
	virtual Block* getNextBlock()=0;
	virtual Block* getNextBlockSingle() {return getNextBlock();}
	virtual bool skipToPos(unsigned int blockPos_)=0;
	
	virtual ValPos* getStartVal()=0;
	virtual unsigned int getStartPos()=0;
	virtual ValPos* getEndVal()=0;
	virtual unsigned int getEndPos()=0;
	
	virtual unsigned int getSize()=0;
protected:
	bool initialized;
	byte* buffer;
	ValPos* utilityPair;
};

#endif // DECODER_H
