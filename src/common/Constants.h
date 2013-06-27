#ifndef CONSTANTS_H
#define CONSTANTS_H

#pragma once

#define RLE_TYPE 1
#define DELTA_POS_TYPE 2
#define INT_SORT_TYPE 3
#define INT_UNSORT_TYPE 4

#define TYPE_INTEGER 1
#define TYPE_FLOAT 2
#define TYPE_STRING 3
#define TYPE_STAR 4

//#define MATERIAL_EXPS 1
//#define MINICSTORE 1

typedef unsigned char byte;
static const int PAGE_SIZE=65000;
#define NULLINT (1<<31)
#define NULLBYTE (1<<7)
//static const int PAGE_SIZE=32;

#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>

#define INT_VALUE_TYPE 1

#define COMPRESSION_TYPE1 1
#define COMPRESSION_TYPE1A 2
#define COMPRESSION_TYPE2 3
#define COMPRESSION_TYPE3 4
#define COMPRESSION_TYPE3A 5
#define COMPRESSION_TYPE4 6

#define BDB_PAGE_SIZE 65536
//#define CACHE_SIZE 104857600  // 100Mb for now
#define CACHE_SIZE 1048576  // 10Mb for now
#define SMALL_CACHE_SIZE 5242880   // 5Mb for now
#define FILE_DELIMITER '|'
#define HEADER_SIZE 8
#define PRIMARY    0
#define SECONDARY  1
#define TERTIARY   2
#define QUATERNARY 3

#define PRIMARY_SUFF    ""
#define SECONDARY_SUFF  ".secondary"
#define TERTIARY_SUFF   ".tertiary"
#define QUATERNARY_SUFF ".quaternary"

#define TEMP_SORTING_DB        "sorted_temp_db"

#define DEBUG false         // output debug info
//#define BUILD_TABLES false;

#define RUNTIME_DATA "../RuntimeData/"

//#define FORCE_REBUILD false

#define NUM_INTS_IN_LZOP 0

//system tables(catalog) file names
#define CSTORE_SYSTABLE_PROJECTION  string("CstoreSysTblProjection.bdb")  //projection primary 
#define CSTORE_SYSTABLE_COLUMN      string("CstoreSysTblColumn.bdb")      //column primary
#define CSTORE_SYSTABLE_COLUMN_AK   string("CstoreSysTblColumnAK.bdb")   //column secondary (alt. key = projectinID)

#define TYPE2MAXVALUES 256

#define BLOCK_SIZE 6000
#define MAX_INT (BLOCK_SIZE - 4*sizeof(int))/sizeof(int)

#endif // CONSTANTS_H

