/**
 * Query1S:
 * SELECT *
 * FROM StringTest01
 *
 * Projections: (StringTest01.ID| StringTest01.DESC)
 *
 * Strategy:
 */
#include "Query1S.h"
#include "../common/ValPos.h"


Query1S::Query1S() {
}

Query1S::~Query1S() {
}

bool Query1S::run() {
	Log::writeToLog("Query1S", 10, "Query 1 starting...");
	bool success=true;
	bool createcolumn=false;
	string path="../data/StringTestData.mid";  

	if (createcolumn){	
		Log::writeToLog("Query1S", 10, "Creating Column1...");  
		SColumnExtracter* ce = new SColumnExtracter(path, 1, 10,false);
		StringEncoder* encoder = new StringEncoder((Operator*) ce, // data source
		                                           0,              // column index
		                                           NULL,
		                                           10,             // string Size
		                                           8*PAGE_SIZE);   // buffer size in bits
		StringDecoder* decoder = new StringDecoder(true);         // value sorted
		PagePlacer* pagePlacer = new PagePlacer(encoder,
		                                        decoder, 
		                                        2,       
		                                        false);
		pagePlacer->placeColumn("StringTest01.ID", false, true);
		delete pagePlacer;
		delete ce;
		delete encoder;
		delete decoder;

		Log::writeToLog("Query1S", 10, "Creating Column2...");
		//string path=RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH;
		ce = new SColumnExtracter(path, 2, 11,false);
		encoder = new StringEncoder((Operator*) ce, // data source
		                            0,              // column index
		                            NULL,
		                            11,             // string Size
		                            8*PAGE_SIZE);   // buffer size in bits
		decoder = new StringDecoder(false);         // value sorted
		pagePlacer = new PagePlacer(encoder,
		                            decoder, 
		                            2,       
		                            false);  
		pagePlacer->placeColumn("StringTest01.DESC", false, false);
		delete pagePlacer;
		delete ce;
		delete encoder;
		delete decoder;

		system("mv StringTest01* " RUNTIME_DATA);
	}

	Log::writeToLog("Query1S", 10, "Opening Column1...");
	StopWatch stopWatch;
	stopWatch.start();

	ROSAM* am1 = new ROSAM( "StringTest01.ID" , 2 ,sizeof(int), 10, ValPos::INTTYPE, ValPos::STRINGTYPE);
	ROSAM* am2 = new ROSAM( "StringTest01.DESC" , 2, sizeof(int), 11, ValPos::INTTYPE, ValPos::STRINGTYPE);
	Predicate* pred1=new Predicate(Predicate::OP_LESS_THAN);
	Predicate* pred2=new Predicate(Predicate::OP_GREATER_THAN);
	StringDataSource* ds1=new StringDataSource(am1,true,true);
	StringDataSource* ds2=new StringDataSource(am2,false,true);

	ds1->setPredicate(pred1);
	ds2->setPredicate(pred2);

	ValPos* rhs1 = new StringValPos(10);
	char* RHSVal1 = "abcde00050";
	rhs1->set(RHSVal1);
	ds1->changeRHSBinding(rhs1);

	ValPos* rhs2 = new StringValPos(11);
	char* RHSVal2 = "Cstore99948";
	rhs2->set(RHSVal2);
	ds2->changeRHSBinding(rhs2);
	//Count* agg = new Count((Operator*) ds1, 0, (Operator*) ds1, 0);


	//ds1->printColumn();
	//ds2->printColumn();

	Operator* srcs[2]={ds1,ds2};
	int numCols[2]={1,1};


	BlockPrinter* bPrint=new BlockPrinter(srcs, numCols,2,NULL);
	bPrint->printColumns();

	cout << "Query 1 S took: " << stopWatch.stop() << " ms" <<  endl;

	delete ds1;
	delete am1;
	delete rhs1;
	delete pred1;
	delete ds2;
	delete am2;
	delete rhs2;
	delete pred2;
	delete bPrint;
	return success;	
}
