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
#include "main.h"
#include <string.h>

main::main()
{
}

main::~main()
{
}

void runTest(string, char*, char*);

int main(int argc, char *argv[]) {
	using namespace std;
	Log::logInit();
	string testName;
	bool useOwn = true;
	char* arg1 = NULL;
	char* arg2 = NULL;

    testName = "Query1S";
	
	if(argc >=2){  
		useOwn = (!(argv[1][0] == '0'));
		if (useOwn) {
			if (argc >= 3) {
				arg1 = argv[2];
				if (argc >=4)
					arg2 = argv[3];
			}
		}
		else {		
			assert(argc>=4);
			testName = argv[3];
			if (argc >= 5) {
				arg1 = argv[4];
				if (argc >=6)
					arg2 = argv[5];
			}
		}
	}
	//cout << "args: " << arg1 << " " <<  arg2 << endl;

	runTest(testName, arg1, arg2);

	Log::logDestroy();
	return 0;
}

void runTest(string testName, char* arg1, char* arg2){
	bool success = true;

	cout << testName << endl;

    UnitTest* query1s = new Query1S();
	
	query1s->run();
	
	if (success) 
		cout << endl << "SUCCESSFULL ON ALL SUITES" << endl;
	else
		cout << endl << "FAILED SOME SUITES" << endl;
}

