#ifndef __PageWriter_H
#define __PageWriter_H

#include <db_cxx.h>
#include <string>
#include <vector>
#include <map>
#include "ROSAM.h"

/**
 * Comments go here
 */
using namespace std;

class PageWriter
{
 private:

    vector<Db*> _db_arr;

    string _tableName;
    map<int, Db*> _db_local;

    int _numindicies;
    int primkeysize;
    int seckeysize;
    int primkeytype;
    int seckeytype;
	//bool valsorted = true;

  // Methods
  public:

    /**
     * Constructs a page encoder
     */
    PageWriter( int numindicies, int primkeysize_, int seckeysize_,short primkeytype_, short seckeytype_);

    /**
     * Destructs this PageEncoder.
     */
    ~PageWriter();

    static void placeRecord( Db *db, char *key, int key_size,
				char *data, int data_size );

    //static void* PageWriter::getRecord( int key, Dbc *cursor, u_int32_t flags, 
    //bool primary, char *buffer );

    void initDB(string c_name, bool dupsort);

    void closeDB();

    void placePage(char* page, char* primkey, char* seckey, int page_size=PAGE_SIZE);
    void placePage(char* page, char* primkey,  int page_size=PAGE_SIZE);
	void placePageSecondary(char* page, char* seckey_,  int page_size=PAGE_SIZE);
    void placeUnsortedRecord(char* seckey_, char* position_);
	void placeUnsortedKey(char* key_, char* val_, int valsize_);
    static int dup_compare(Db *dbp, const Dbt *a, const Dbt *b);
    //static int dup_compare2(Db *dbp, const Dbt *a, const Dbt *b);
};

#endif // __PageWriter_H
