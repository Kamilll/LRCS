#include "main.h"
#include <string.h>

main::main()
{
}

main::~main()
{
}

void runTest(string, char*, char*);
void initAllTestSuites(map<string, UnitTest*> &testSuites);

int main(int argc, char *argv[]) {
	using namespace std;
	Log::logInit();
	string testName;
	bool useOwn = true;
	char* arg1 = NULL;
	char* arg2 = NULL;
	
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
	map<string, UnitTest*> testSuites;
    initAllTestSuites(testSuites);
	
	cout << testName << endl;
	if(testSuites[testName] != NULL)
	    success &= testSuites[testName]->run();
	
	if (success) 
		cout << endl << "SUCCESSFULL ON ALL SUITES" << endl;
	else
		cout << endl << "FAILED SOME SUITES" << endl;

	for (map<string, UnitTest*>::iterator iterPos=testSuites.begin(); iterPos !=testSuites.end(); ++iterPos)
		delete iterPos->second;
}

void initAllTestSuites(map<string, UnitTest*> &testSuites){
   testSuites["Query1S"] = new Query1S();
   testSuites["Query2S"] = new Query2S();
   testSuites["Query3S"] = new Query3S();
   testSuites["Query4S"] = new Query4S();
}
