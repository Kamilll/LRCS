#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <math.h>
#include <stdlib.h>

#include <db_cxx.h>

#include "PageWriter.h"

using namespace std;

PageWriter::PageWriter(int numindicies, int primkeysize_, int seckeysize_,short primkeytype_, short seckeytype_)
{


	_numindicies = numindicies;

	primkeysize=primkeysize_;
	seckeysize=seckeysize_;
	assert(primkeysize_);
	assert(numindicies==1 || seckeysize_);
	primkeytype=primkeytype_;
	seckeytype=seckeytype_;
	assert(primkeytype_);
	assert(numindicies==1 || seckeytype_);
}

PageWriter::~PageWriter()
{

	for ( int i = 0; i < _numindicies; i++ )
	{
		delete _db_local[ i ];
	}
}

void PageWriter::initDB(string c_name, bool dupsort) {
	int ret;
	for (int which = 0; which < _numindicies; which++) {
		_db_local[ which ] = new Db( NULL, 0 );

		_db_local[ which ]->set_cachesize( 0, CACHE_SIZE, 1 );
		_db_local[ which ]->set_pagesize( BDB_PAGE_SIZE );

		short type;
		if (which ==0)
			type = primkeytype;
		else
			type = seckeytype;
		switch (type){
			case ValPos::INTTYPE:
				_db_local[ which ]->set_bt_compare( ROSAM::compare_int );
				break;
			case ValPos::STRINGTYPE:
				_db_local[ which ]->set_bt_compare( ROSAM::compare_key );		
				break;
			case ValPos::DOUBLETYPE:
				_db_local[ which ]->set_bt_compare( ROSAM::compare_double );
				break;
			default:
				throw new UnexpectedException("Unknown Type");
		}
		_db_local[ which ]->set_error_stream( &cout );
		/*Only secondary index support duplicate*/
		if (which!=0 && dupsort) {
			_db_local[ which ]->set_flags(DB_DUP);
			switch (type){
				case ValPos::INTTYPE:
					_db_local[ which ]->set_dup_compare( ROSAM::compare_int );
					break;
				case ValPos::STRINGTYPE:
					_db_local[ which ]->set_dup_compare( ROSAM::compare_key );
					break;
				case ValPos::DOUBLETYPE:
					_db_local[ which ]->set_dup_compare( ROSAM::compare_double );
					break;
				default:
					throw new UnexpectedException("Unknown Type");
			}
		}
		char* dbname = new char[1024];
		strcpy(dbname, (c_name+ 
		                ( which == PRIMARY ? PRIMARY_SUFF : "") +
		                ( which == SECONDARY ? SECONDARY_SUFF : "") +
		                ( which == TERTIARY ? TERTIARY_SUFF : "" ) +
		                ( which == QUATERNARY ? QUATERNARY_SUFF : "" )).c_str());
		ret = _db_local[ which ]->open( NULL, dbname,
		                               NULL, DB_BTREE, DB_CREATE, 0664 );
		delete[] dbname;
		if ( ret < 0 ) {
			cout << "Failed to open table i = " << which << endl;
			throw new AccessException("PageWriter.cpp: Failed to open table");
		}
	}

}

void PageWriter::closeDB() {
	for (int which = 0; which < _numindicies; which++)
		_db_local[ which ]->close( 0 );
}


void PageWriter::placePage(char* page, char* primkey, char* seckey,int page_size) {
	placeRecord( _db_local[ PRIMARY ], primkey, 
	            primkeysize, page, page_size );

	placeRecord( _db_local[ SECONDARY ], seckey, seckeysize, primkey, primkeysize);
}

void PageWriter::placePage(char* page, char* primkey, int page_size) {
	placeRecord( _db_local[ PRIMARY ], primkey, primkeysize, page, page_size );

}

void PageWriter::placeUnsortedRecord(char* seckey_, char* position_){
	placeRecord( _db_local[ SECONDARY ], seckey_, seckeysize, position_, sizeof(int) );

}

void PageWriter::placeUnsortedKey(char* key_, char* val_, int valsize_){
	placeRecord( _db_local[ PRIMARY ], key_, primkeysize, val_, valsize_);

}

void PageWriter::placeRecord( Db *db, char *key, int key_size, 
                             char *data, int data_size )
{
	Dbt dkey, ddata;  

	memset(&dkey, 0, sizeof(key)); 
	memset(&ddata, 0, sizeof(data)); 

	dkey.set_data( key );
	dkey.set_size( key_size );

	ddata.set_data( data );
	ddata.set_size( data_size );

	try
	{
		if (db->put(NULL, &dkey, &ddata, 0) == 0)
		{
		}
		else {
		   throw new AccessException("PageWriter.cpp: placeRecord:  Failed");
		}
	} 
	catch(DbException& e) 
	{
		cout << "Exception " << e.what() << endl;
		exit( 1 );
	}
}

int PageWriter::dup_compare(Db *dbp, const Dbt *a, const Dbt *b)
{
	char* ii = ((char *)a->get_data());
	char* jj = ((char *)b->get_data());
	int size1 = a->get_size();
	int size2 = b->get_size();
	assert(size1=size2);
	if (!jj && !ii)
		return 0;
	if (!jj)
		return 1;
	if (!ii)
		return -1;
	return memcmp(ii,jj,size1);
}
