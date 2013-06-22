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
                     10,             // string Size
					 8*PAGE_SIZE);   // buffer size in bits
	StringDecoder* decoder = new StringDecoder(true);         // value sorted
 	PagePlacer* pagePlacer = new PagePlacer(encoder,
					    decoder, 
					    2,       
					    false);
    pagePlacer->placeColumn("StringTest03.ID", false, true);
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;

    Log::writeToLog("Query1S", 10, "Creating Column2...");
    //string path=RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH;
    ce = new SColumnExtracter(path, 2, 11,false);
    encoder = new StringEncoder((Operator*) ce, // data source
					 0,              // column index
                     11,             // string Size
					 8*PAGE_SIZE);   // buffer size in bits
    decoder = new StringDecoder(false);         // value sorted
    pagePlacer = new PagePlacer(encoder,
					    decoder, 
					    2,       
					    false);  
    pagePlacer->placeColumn("StringTest03.DESC", false, false);
    delete pagePlacer;
    delete ce;
    delete encoder;
    delete decoder;

	system("mv StringTest03* " RUNTIME_DATA);
  }
 
  Log::writeToLog("Query1S", 10, "Opening Column1...");
  StopWatch stopWatch;
  stopWatch.start();

  ROSAM* am1 = new ROSAM( "StringTest03.ID" , 2 ,sizeof(int), 10, ValPos::INTTYPE, ValPos::STRINGTYPE);
  ROSAM* am2 = new ROSAM( "StringTest03.DESC" , 2, sizeof(int), 11, ValPos::INTTYPE, ValPos::STRINGTYPE);
  Predicate* pred=new Predicate(Predicate::OP_GREATER_THAN);

  StringDataSource* ds1=new StringDataSource(am1,true,true);
  StringDataSource* ds2=new StringDataSource(am2,false,true);
  ds1->setPositionFilter(ds2, 0);
  ds2->setPredicate(pred);
  ValPos* rhs = new StringValPos(1,11);
  //char* RHSVal = "Cstore00004";
  char* RHSVal = "Cstore99997";
  rhs->set(RHSVal);
  ds2->changeRHSBinding(rhs);
  
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
  delete ds2;
  delete am2;
  return success;	
}
