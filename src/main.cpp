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

