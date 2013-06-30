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
// Uncompressed Int Datasource
// Problems: migfer@mit.edu

#include "StringDataSource.h"

StringDataSource::StringDataSource(AM* am_, bool valSorted_, bool isROS) 
: DataSource(am_, isROS)
{
	valSorted = valSorted_;
	decoder = new StringDecoder(valSorted);
	
	//posOutBlock=NULL;
	useWP=false;	                                                                                                              
}

 
StringDataSource::~StringDataSource()
{

	if (decoder!=NULL) delete decoder;
	if (currBlock!=NULL) delete currBlock;
	//if (posOutBlock!=NULL) delete posOutBlock;
}

//printColumn, print to stringstream instead. For testing purpose
void StringDataSource::printColumn() {
	using namespace std;
	int count=0;
	//int total=0;
	//char prevVal="xxxxxxxxxx";

	Block* blk=this->getNextValBlock(1);
	while (blk!=NULL) {
		while (blk->hasNext()) {
			ValPos* vp = blk->getNext();
			//count++;
			//cout<<count<<endl;
			cout<<(char*)vp->value<<endl;
			/*char currVal= (char*)vp->value;
			if (prevVal!=currVal)  {
				if (count%10==0) ss  << endl << "Read uniques value: ";
				ss << currVal << ", ";
				count++;
				prevVal=currVal;
		}
		total++;*/
		}
		blk=this->getNextValBlock(0);
	}
	/*  ss << endl;
	 ss << "Total number of values read: " << total << endl;*/
}//printColumn, print to stringstream instead. For testing purpose

