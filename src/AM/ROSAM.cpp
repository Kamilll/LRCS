#include <fstream>
#include <sstream>
#include <strstream>
#include <stdlib.h>
#include "ROSAM.h"
//#include "CatalogInstance.h"
using namespace std;

ROSAM::ROSAM( string table_, int numindicies, int primkeysize_, int seckeysize_ , short primkeytype_, short seckeytype_)
{
	table=table_;
	assert(numindicies > 0 && numindicies <= 4);
	_colName = table.substr( 0, table.size() - 4);
	_iterator = NULL;
	_numindicies = numindicies;

	for ( int i = 0; i < numindicies; i++ )
	{
		_db_arr.push_back( new Db( NULL, 0 ) );

		short type;
		if (i ==0)
			type = primkeytype_;
		else
			type = seckeytype_;
		switch (type){
			case ValPos::INTTYPE:
				_db_arr[ i ]->set_bt_compare( ROSAM::compare_int );
				break;
			case ValPos::STRINGTYPE:
				_db_arr[ i ]->set_bt_compare( ROSAM::compare_key );
				break;
			case ValPos::DOUBLETYPE:
				_db_arr[ i ]->set_bt_compare( ROSAM::compare_double );
				break;
			default:
				throw new UnexpectedException("Unknown Type");
		}
		_db_arr[ i ]->set_error_stream( &cout );

		_iter_arr.push_back( NULL );
	}

	// No need to call heavy artillery for non-primary indices.
	_db_arr[ PRIMARY ]->set_cachesize( 0, CACHE_SIZE, 1 );
	_db_arr[ PRIMARY ]->set_pagesize( BDB_PAGE_SIZE );
	_db_arr[ PRIMARY ]->set_error_stream( &cout );


	loadTable( table); 

	initCursors();
	_internalBuffer = NULL;

	//zklee: bulk read is the default read mode
	_bulkBuffer_size = ROSAM::BULK_BUFFER_SIZE;
	if (_bulkBuffer_size > 0) {
		_bulkBuffer = new char[_bulkBuffer_size];
		pos_array = new char[_bulkBuffer_size];
		bulk_data.set_flags(DB_DBT_USERMEM);
		bulk_data.set_data( _bulkBuffer );
		bulk_data.set_ulen( _bulkBuffer_size  );
		bulk_iter = NULL;
		dBulk_iter = NULL;
		bulk_empty = true;
	} else {
		_bulkBuffer = NULL;  
	}

	primkeysize=primkeysize_;
	seckeysize=seckeysize_;
	primkeytype=primkeytype_;
	seckeytype=seckeytype_;
}

ROSAM::~ROSAM()
{
	if ( _iterator != NULL )
		_iterator->close( );

	for ( int i = 0; i < _numindicies; i++ )
		if ( _db_arr[ i ] != NULL )
	{
		_db_arr[ i ]->close( 0 );

		delete _db_arr[ i ];
		_db_arr[ i ] = NULL;
	}

	if ( _internalBuffer ){
		delete [] _internalBuffer;
	    _internalBuffer = NULL;
	}

	if ( _bulkBuffer ) {
		delete [] _bulkBuffer;
		_bulkBuffer = NULL;
	}

	if ( pos_array ) {
		delete [] pos_array;
		pos_array = NULL;
	}
}

char *ROSAM::acquireInternalBuffer()
{
	if ( _internalBuffer == NULL )
		_internalBuffer = new char[PAGE_SIZE];

	// not sure if new will zero out memory. To be safe -- do it always for now.
	memset(_internalBuffer, 0, PAGE_SIZE);
	if(_internalBuffer == NULL) 
		throw new UnexpectedException("No Memory");
	else
		return _internalBuffer;
}

char *ROSAM::acquireMoreBuffer()
{
	char* newBuffer = NULL;
	newBuffer = (char*)realloc(_internalBuffer, PAGE_SIZE);
	if(newBuffer == NULL) 
		throw new UnexpectedException("No Memory");
	else{
		_internalBuffer = newBuffer;
		return _internalBuffer;
	}
}

void ROSAM::loadTable( string table )
{
	int ret;
	_tableName = table;

	try
	{
		for ( int i = 0; i < _numindicies; i++ )
		{
			cout << " LOAD " << ( (RUNTIME_DATA+table) + 
			                     ( i == PRIMARY ? PRIMARY_SUFF : "") +
			                     ( i == SECONDARY ? SECONDARY_SUFF : "") +
			                     ( i == TERTIARY ? TERTIARY_SUFF : "" ) +
			                     ( i == QUATERNARY ? QUATERNARY_SUFF : "" )) << endl;
			ret = _db_arr[ i ]->open
				( NULL, ( (RUNTIME_DATA+table) + 
				         ( i == PRIMARY ? PRIMARY_SUFF : "") +
				         ( i == SECONDARY ? SECONDARY_SUFF : "") +
				         ( i == TERTIARY ? TERTIARY_SUFF : "" ) +
				         ( i == QUATERNARY ? QUATERNARY_SUFF : "" )).c_str(), 
				 NULL, DB_BTREE, DB_CREATE, 0664 );
			if ( ret < 0 )
			{
				cerr << "Throw AccessException, ROSAM.cpp unable to load table " << endl;
				throw new AccessException("ROSAM.cpp: loadTable(): Unable to load table");
			}
		}
	}
	catch (DbException& e) 
	{
		cout << "Exception " << e.what() << endl;
		exit( 1 );
	}
}

void ROSAM::initCursors( )
{
	Log::writeToLog("ROSAM",0,"Called init cursors");
	if ( _iterator != NULL )
		_iterator->close();

	for ( int i = 0; i < _numindicies; i++ )
	{
		if ( _iter_arr[ i ] != NULL )
			_iter_arr[ i ]->close( );
		_db_arr[ i ]->cursor( NULL, &(_iter_arr[ i ]), 0);

		Dbt key, data;
		memset(&key, 0, sizeof(key)); 
		memset(&data, 0, sizeof(data)); 
	}
}

const void* ROSAM::skipToPagePrimary( char* key )
{
	Dbt data,pkey; // Dbt is a key / data pair
	int ret;

	memset(&pkey, 0, sizeof(pkey)); 
	memset(&data, 0, sizeof(data)); 

	pkey.set_flags(DB_DBT_USERMEM);
	data.set_flags(DB_DBT_USERMEM);

	char* key_buf=new char[primkeysize];
	memcpy(key_buf, key, primkeysize);
	char* buffer= acquireInternalBuffer();
	data.set_data( buffer );
	data.set_size( PAGE_SIZE );
	// BDB documentation neglects to mention that once you use your own
	// buffer you MUST set this parameter or things will break horribly.
	// thanks to a helpful post on a google newsgroup...
	data.set_ulen( PAGE_SIZE );

	pkey.set_data( key_buf );
	pkey.set_size( primkeysize);
	pkey.set_ulen( primkeysize );


	ret = _iter_arr[ PRIMARY ]->get( &pkey, &data, DB_SET_RANGE );

	delete[] key_buf;
	
	if ( ret < 0 )
	{
		// Iterator on last value failed. That can only mean one thing
		// there is no such value in the DB.	
		return NULL;
	}

	
	return data.get_data();
}

const void* ROSAM::skipToPageSecondary( char* key )
{ 
	Dbt data, data2, pkey; // Dbt is a key / data pair
	int ret;

	memset(&pkey, 0, sizeof(pkey)); 
	memset(&data, 0, sizeof(data)); 
	memset(&data2, 0, sizeof(data2)); 

	pkey.set_flags(DB_DBT_MALLOC);
	data.set_flags(DB_DBT_MALLOC);
	data2.set_flags(DB_DBT_MALLOC);

	pkey.set_size( seckeysize );
	pkey.set_data( key );

	data.set_flags(DB_DBT_MALLOC);
	data.set_size(primkeysize);

	ret = _iter_arr[ SECONDARY]->get( &pkey, &data, DB_SET_RANGE );

	if ( ret < 0 )
	{
		// Iterator on last value failed. That can only mean one thing
		// there is no such value in the DB.
		return NULL;
	}
	
	ret = _iter_arr[ PRIMARY ]->get( &data, &data2, DB_SET_RANGE );

	if ( ret < 0 )
	{
		cerr << "Additional index corrupt, returning NULL " << endl;
		return NULL;
	}
	return data2.get_data();
} 

const void* ROSAM::getNextPageSecondary( )
{
	return getNextPage( NULL, _iter_arr[ SECONDARY ], DB_NEXT, false, 
	                   acquireInternalBuffer() );
}

const void* ROSAM::getNextPagePrimary()
{
	return getNextPage( NULL, _iter_arr[ PRIMARY ], DB_NEXT, true, 
	                   acquireInternalBuffer() );
}

const void* ROSAM::getNextPage( char* key, Dbc *iter, u_int32_t flags, 
                               bool primary, char *buffer )
{	
	Dbt skey, data, pkey; // Dbt is a key / data pair

	memset(&pkey, 0, sizeof(pkey)); 
	memset(&data, 0, sizeof(data)); 

	pkey.set_flags(DB_DBT_USERMEM);
	data.set_flags(DB_DBT_USERMEM);

	int keysize;
	if (primary)keysize = primkeysize;
	else keysize = seckeysize;
	int ret = -1/*, b = -1*/;
	char* a = key;
	char* key_buf=new char[keysize];
	a = key_buf;

	data.set_data( buffer );
	data.set_size( PAGE_SIZE );
	// BDB documentation neglects to mention that once you use your own
	// buffer you MUST set this parameter or things will break horribly.
	// thanks to a helpful post on a google newsgroup...
	data.set_ulen( PAGE_SIZE );

	pkey.set_data( key_buf );
	pkey.set_size( keysize);
	pkey.set_ulen( keysize );

	try
	{
	  ret = iter->get( &pkey, &data, flags );
	}
	catch(DbException& e) 
	{
		if (DEBUG) cout << "Exception " << e.what() << endl;
		exit( 1 );
	}

	if ( ret < 0 )
	{
		delete[] key_buf;
		return NULL;
	}

	delete[] key_buf;
	return data.get_data();
}

const void* ROSAM::doCursorGetPos(char* key_, u_int32_t flags)
{
	Dbt data, key;
	unsigned int* pos_size = (unsigned int*)pos_array; 
	byte* pos_ptr = pos_array+sizeof(int);	

	int ret = 0;
	memset(&key, 0, sizeof(key)); 
	memset(&data, 0, sizeof(data)); 
	memset(pos_array, 0, _bulkBuffer_size);
	*pos_size = 0;	

	key.set_data( key_ );
	key.set_size( seckeysize);
	key.set_ulen( seckeysize );
	key.set_flags(DB_DBT_USERMEM);

	data.set_data( acquireInternalBuffer () );
	data.set_ulen( PAGE_SIZE );
	data.set_size( PAGE_SIZE );
	data.set_flags( DB_DBT_USERMEM );


	/*zklee: all the index is position primary and value secondary*/
	ret = _iter_arr[ SECONDARY ]->get( &key, &data, flags );
	if ( ret < 0 )return NULL;
	else {
		*pos_size = 1;
		memcpy(pos_ptr, data.get_data(),sizeof(int));
		memcpy(key_, key.get_data(),seckeysize);
		return pos_array;
	}
}

const void* ROSAM::doCursorGetPosMulti(char* key_, u_int32_t flags)
{
	u_int32_t _flags = flags | DB_MULTIPLE;
	Dbt myData;
	//number of positions in the array
	unsigned int* pos_size = (unsigned int*)pos_array; 
	byte* pos_ptr = pos_array+sizeof(int);	
	int ret = 0;

	memset(&myData, 0, sizeof(myData));
	memset(&bulk_key, 0, sizeof(bulk_key)); 
	bulk_key.set_data( key_ );
	bulk_key.set_size( seckeysize);
	bulk_key.set_ulen( seckeysize);
	bulk_key.set_flags(DB_DBT_USERMEM);

	
	memset(_bulkBuffer, 0, _bulkBuffer_size);
	memset(pos_array, 0, _bulkBuffer_size);
	*pos_size = 0;

	/*zklee: all the index is position primary and value secondary*/
	ret = _iter_arr[ SECONDARY ]->get( &bulk_key, &bulk_data, _flags );
	if ( ret < 0 ) return NULL;

	dBulk_iter = new DbMultipleDataIterator(bulk_data);
	while(dBulk_iter->next(myData) ) {
		*pos_size+=1;
		if(((*pos_size + 1) * sizeof(int))>_bulkBuffer_size)
			throw new AccessException("buffer size is not enough!");
		memcpy(pos_ptr, myData.get_data(),sizeof(int));
		pos_ptr+=sizeof(int);
	}
	
    delete dBulk_iter;
	dBulk_iter = NULL;
	
	if (*pos_size > 0)
		return pos_array;
	else 
		return NULL;
}

const void* ROSAM::doCursorGetPage(char* key,u_int32_t flags)
{ 
	Dbt data, pkey; 
	int ret;

	memset(&pkey, 0, sizeof(pkey)); 
	memset(&data, 0, sizeof(data)); 

	pkey.set_flags(DB_DBT_MALLOC);
	pkey.set_size( seckeysize );
	pkey.set_data( key );


	ret = _iter_arr[ SECONDARY]->get( &pkey, &data, flags );

	if ( ret < 0 )
	{
		// Iterator on last value failed. That can only mean one thing
		// there is no such value in the DB.
		return NULL;
	}
	
	return data.get_data();
} 

/*zklee:Move the cursor to the specified key/data pair of the database,
 and return the datum associated with the given key.
 The datum is a position array */
const void* ROSAM::getDbSet ( char* rkey )
{
	return doCursorGetPosMulti(rkey,DB_SET); 
}

/*zklee:Identical to the getDbSet, except the key is returned as well as the 
 data item and the returned key/data pair is the smallest key greater than 
 or equal to the specified key.  The datum is a position array*/
const void* ROSAM::getDbSetRange (char* rkey)
{
	return doCursorGetPosMulti(rkey,DB_SET_RANGE);
}

/*zklee:the cursor is moved to the next key/data pair of the database, 
 and that data is returned.  The datum is a position array*/
const void* ROSAM::getDbNext ( )
{
	char* _key = new char[seckeysize];
	const void* rdata = doCursorGetPosMulti(_key, DB_NEXT);
	delete[] _key;
	return rdata;
}


/*zklee:the method get data from a range (lowKey, highKey).
The datum is a position array.*/
const void* ROSAM::getDbNextRange (char* lowKey, char* highKey,bool firstCall_ )
{
	assert(memcmp(lowKey,highKey,seckeysize)<0);
	char* _key;
	Dbt _data;
	if (firstCall_)
		return getDbSetRange (lowKey);
	else
		_key = new char[seckeysize];
	const void* rdata = doCursorGetPosMulti(_key, DB_NEXT);
	if(rdata != NULL){
		if(memcmp(_key, highKey, seckeysize)<0){
			delete[] _key;
			return rdata;
		}else{
			delete[] _key;
			return NULL;}
	}else {
		delete[] _key;
		return NULL;}
}

/*zklee:the method get data from a range (firstkey, highKey).
 The datum is a position array*/
const void* ROSAM::getDbNextRange (char* highKey,bool firstCall_ )
{
	char* _key = new char[seckeysize];
	u_int32_t _flag = (firstCall_?DB_FIRST:DB_NEXT);
	const void* rdata = doCursorGetPosMulti(_key, _flag);
	if(rdata != NULL){
		if(memcmp(_key, highKey, seckeysize)<0){
			delete[] _key;
			return rdata;
		}else{
			delete[] _key;
			return NULL;}
	}else {
		delete[] _key;
		return NULL;}
}

/*zklee:Move the cursor to the specified key/data pair of the database,
 and return the datum associated with the given key.
 The datum is a whole DB page */
const void* ROSAM::getDbPageSet ( char* rkey )
{
	return doCursorGetPage(rkey,DB_SET); 
}

/*zklee:Identical to the getDbSet, except the key is returned as well as the 
 data item and the returned key/data pair is the smallest key greater than 
 or equal to the specified key.  The datum is a whole DB page*/
const void* ROSAM::getDbPageRange (char* rkey)
{
	return doCursorGetPage(rkey,DB_SET_RANGE);
}

/*zklee:the cursor is moved to the next key/data pair of the database, 
 and that data is returned.  The datum is a whole DB page*/
const void* ROSAM::getDbPageNext( )
{
	char* _key = new char[seckeysize];
	const void* rdata = doCursorGetPage(_key, DB_NEXT);
	delete[] _key;
	return rdata;
}

/*zklee:the cursor is moved to the next key/data pair of the database.
 If the next key is duplicate, data is returned, else NULL.
 The datum is a whole DB page*/
const void* ROSAM::getDbPageNextDup ( )
{
	char* _key = new char[seckeysize];
	const void* rdata = doCursorGetPage(_key, DB_NEXT_DUP);
	delete[] _key;
	return rdata;
}


/*zklee:the method get data from a range (lowKey, highKey).
 he datum is a whole DB page.*/
const void* ROSAM::getDbPageNextRange (char* lowKey, char* highKey,bool firstCall_ )
{
	assert(memcmp(lowKey,highKey,seckeysize)<0);
	char* _key;
	Dbt _data;
	if (firstCall_)
		return getDbPageRange (lowKey);
	else
		_key = new char[seckeysize];
	const void* rdata = doCursorGetPage(_key, DB_NEXT);
	if(rdata != NULL){
		if(memcmp(_key, highKey, seckeysize)<0){
			delete[] _key;
			return rdata;
		}else{
			delete[] _key;
			return NULL;}
	}else {
		delete[] _key;
		return NULL;}
}

/*zklee:the method get data from a range (firstkey, highKey).
 he datum is a whole DB page*/
const void* ROSAM::getDbPageNextRange (char* highKey,bool firstCall_ )
{
	char* _key = new char[seckeysize];
	u_int32_t _flag = (firstCall_?DB_FIRST:DB_NEXT);
	const void* rdata = doCursorGetPage(_key, _flag);
	if(rdata != NULL){
		if(memcmp(_key, highKey, seckeysize)<0){
			delete[] _key;
			return rdata;
		}else{
			delete[] _key;
			return NULL;}
	}else {
		delete[] _key;
		return NULL;}
}

char* ROSAM::getLastIndexValuePrimary() 
{
	Dbt key, data;
	char* answer=NULL;

	memset(&key, 0, sizeof(key)); 
	memset(&data, 0, sizeof(data)); 

	key.set_flags(DB_DBT_MALLOC);
	data.set_flags(DB_DBT_MALLOC);

	if ( _iterator != NULL ) {
		_iterator->close( );
		_iterator=NULL;
	}

	//+ this change the cursor position and may affect the operator that is being used as position filter
	_iter_arr[PRIMARY]->dup( &_iterator, DB_POSITION );
	_iterator->get( &key, &data, DB_LAST );
	//-

	if ( key.get_data() == NULL )
	{
		cerr << "Throw AccessException, ROSAM.cpp no data in index data " << endl;

		throw new AccessException("ROSAM: No data in index data");
	}
	else
		answer = (char*)key.get_data();

	// setting the cursor to first value is not an option, since
	// that would lose a page...

	//+ this change the cursor position and may affect the operator that is being used as position filter
	//initCursors();
	//-

	return answer;
}
char* ROSAM::getLastIndexValueSecondary()
{
	//cout << " ROSAM calls get last index val sec " << endl;

	Dbt key, data;
	char* answer;

	if (_numindicies> 1) {

		memset(&key, 0, sizeof(key)); 
		memset(&data, 0, sizeof(data)); 

		key.set_flags(DB_DBT_MALLOC);
		data.set_flags(DB_DBT_MALLOC);

		if ( _iterator != NULL ) {
			_iterator->close( );
			_iterator=NULL;
		}

		//+ this change the cursor position and may affect the operator that is being used as position filter
		_iter_arr[SECONDARY]->dup( &_iterator, DB_POSITION );
		_iterator->get( &key, &data, DB_LAST );
		//-

		if ( key.get_data() == NULL )
		{
			cerr << "Throw AccessException, ROSAM.cpp no data in index data 2 " << endl;

			throw new AccessException("ROSAM: No data in index data");
		}
		else
			answer = (char*)key.get_data();

		// setting the cursor to first value is not an option, since
		// that would lose a page...

		//+ this change the cursor position and may affect the operator that is being used as position filter
		//initCursors();

	}
	else
	{
		cerr << "Throw AccessException, ROSAM.cpp no secondary index for this column " << endl;

		throw new AccessException("ROSAM: no secondary index for this column");
	}
	return answer;
}


/* 
 * Returns: 
 * < 0 if a < b 
 * = 0 if a = b 
 * > 0 if a > b 
 */ 

int ROSAM::compare_int(Db *dbp, const Dbt *a, const Dbt *b)
{
	int i = *((int *)a->get_data());
	int j = *((int *)b->get_data());

	if (i > j)
		return (1);
	if (i < j)
		return (-1);
	return (0);
}
int ROSAM::compare_double(Db *dbp, const Dbt *a, const Dbt *b)
{
	double i = *((double *)a->get_data());
	double j = *((double *)b->get_data());

	if (i > j)
		return (1);
	if (i < j)
		return (-1);
	return (0);
}

int ROSAM::compare_key(Db *dbp, const Dbt *a, const Dbt *b)
{
	char* i = ((char *)a->get_data());
	char* j = ((char *)b->get_data());
	int size1 = a->get_size();
	int size2 = b->get_size();
	assert(size1 == size2);
	return memcmp(i,j,size1);
}

string ROSAM::toString()
{
	return "Integer Column string representation";
}
string ROSAM::getTableName() {
	return table;	
}
