/**
 * Query3S:
 * SELECT *
 * FROM StringTest03
 *
 * Projections: (StringTest03.ID| StringTest03.DESC)
 *
 * Purpose:
 * Test bitmap encoding
 */
#include "Query3S.h"
#include "../common/ValPos.h"
#include "../Wrappers/Encoder/LZEncoder.h"
#include "../Wrappers/Decoder/LZDecoder.h"
#include "../Wrappers/LZDataSource.h"
#include "../Wrappers/Encoder/BitmapEncoder.h"
#include "../Wrappers/Decoder/BitmapDecoder.h"
//#include "../Wrappers/Encoder/PosEncoder.h"
#include "../Wrappers/BitmapDataSource.h"

Query3S::Query3S() {
}

Query3S::~Query3S() {
}

bool Query3S::run() {
	Log::writeToLog("Query3S", 10, "Query 1 starting...");
	bool success=true;
	bool createcolumn=false;
	string path="../data/BitMapTestData.large";  

	if (createcolumn){	
		Log::writeToLog("Query3S", 10, "Creating Column1...");  
		SColumnExtracter* ce = new SColumnExtracter(path, 1, 10,false);
		//BitmapEncoder* posEncoder = new BitmapEncoder(10, 20*PAGE_SIZE);
		StringEncoder* encoder = new StringEncoder((Operator*) ce, // data source
		                                           0,              // column index
		                                           NULL,     //position encoder
		                                           10,             // string Size
		                                           20*PAGE_SIZE);  // buffer size in bits 2.5
		StringDecoder* decoder = new StringDecoder(true);          // value sorted
		LZEncoder* lzEncoder = new LZEncoder(encoder); //Compress the data
		LZDecoder* lzDecoder = new LZDecoder(decoder); //Decompress the data
		PagePlacer* pagePlacer = new PagePlacer(lzEncoder,
		                                        lzDecoder, 
		                                        2,       
		                                        true);
		pagePlacer->placeColumn("StringTest03.ID", false, true);
		delete pagePlacer;
		delete ce;
		delete encoder;
		delete decoder;
		delete lzEncoder;
		delete lzDecoder;

		Log::writeToLog("Query3S", 10, "Creating Column2...");
		ce = new SColumnExtracter(path, 2, 4, false);
		BitmapEncoder* posEncoder = new BitmapEncoder(4,8*PAGE_SIZE);
		encoder = new StringEncoder((Operator*) ce, // data source
		                            0,              // column index
		                            posEncoder,     // position encoder
		                            4,             // string Size
		                            16*PAGE_SIZE);   // buffer size in bits
		decoder = new StringDecoder(true);         // value sorted
        lzEncoder = new LZEncoder(encoder); //Compress the data
		lzDecoder = new LZDecoder(decoder); //Decompress the data
		pagePlacer = new PagePlacer(lzEncoder,
		                            lzDecoder, 
		                            2,       
		                            true);  
		pagePlacer->placeColumn("StringTest03.CODE",false,false);
		delete pagePlacer;
		delete ce;
		delete posEncoder;
		delete encoder;
		delete decoder;
		delete lzEncoder;
		delete lzDecoder;
		
		system("mv StringTest03* " RUNTIME_DATA);
	}

	Log::writeToLog("Query3S", 10, "Opening Column1...");
	StopWatch stopWatch;
	stopWatch.start();

	ROSAM* am1 = new ROSAM( "StringTest03.ID" , 2 ,sizeof(int), 10, ValPos::INTTYPE, ValPos::STRINGTYPE);
	ROSAM* am2 = new ROSAM( "StringTest03.CODE" , 2, sizeof(int), 4, ValPos::INTTYPE, ValPos::STRINGTYPE);
	Predicate* pred1=new Predicate(Predicate::OP_LESS_THAN);
	Predicate* pred2=new Predicate(Predicate::OP_EQUAL);
	LZDataSource* ds1=new LZDataSource(am1,true,true,(new StringDecoder(true)));
    BitmapDataSource* ds2=new BitmapDataSource(am2,false,true,(new StringDecoder(true)));

	ValPos* rhs1 = new StringValPos(10);
	char* RHSVal1 = "abc0000040";
	rhs1->set(RHSVal1);
	ValPos* rhs2 = new StringValPos(4);
	char* RHSVal2 = "1000";
	rhs2->set(RHSVal2);	

	//ds1->setPredicate(pred1);
	//ds1->changeRHSBinding(rhs1);
	//ds2->setPredicate(pred2);
    //ds2->changeRHSBinding(rhs2);
	
	Operator* srcs[2]={ds1,ds2};
	int numCols[2]={1,1};


	BlockPrinter* bPrint=new BlockPrinter(srcs, numCols,2,"Query3S.out");
	bPrint->printColumns();
	
	cout << "Query 3 S took: " << stopWatch.stop() << " ms" <<  endl;

	delete ds1;
	delete am1;
	delete ds2;
	delete am2;	
	return success;	
}
