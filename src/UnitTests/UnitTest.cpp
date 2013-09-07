#include "UnitTest.h"

UnitTest::UnitTest()
{
}

UnitTest::~UnitTest()
{
}

bool UnitTest::test(char* msg_, int retBool_, int val_, int exp_) {
	using namespace std;
	if (retBool_) {
		cout << msg_ << " X: " << val_ << "\tE[X]: " << exp_ << " \t";
		if (val_==exp_) {
			cout << "SUCCESS" << endl;
			return true;
		}
		else {
			cout << "FAILED" << endl;
			return false;
		}
	}
	else {
		cout << msg_ << " FAILED (function return failed)" <<  endl;
		return false;
	}	
}
