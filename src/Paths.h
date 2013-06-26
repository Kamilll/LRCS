// New Paths.h, using new data infrastructure

#define SAMPLEDATALOC "../data/"
#define TPCHDATALOC "../data/"
#define TPCHDATAWOSROSLOC TPCHDATALOC
//#define TPCHDATALOC "/data/eoneil/mycstore/tpch/dbgen/scale10/final/"
//#define TPCHDATAWOSROSLOC TPCHDATALOC "wosrosplay/"

#define RLE_EXTRACT_LOAD_AND_ACCESS_DATA_PATH SAMPLEDATALOC "lshipdate.sorted.mini"

#define SHIPDATE_MINI SAMPLEDATALOC "lshipdate.sorted.mini"
#define SHIPDATE_TINY SAMPLEDATALOC "lshipdate.sorted.tiny"
#define DELTAPOS_TEST SAMPLEDATALOC "deltaPos.test"
#define LINEITEM_TABLE_PATH SAMPLEDATALOC "lineitem.sorted.full"
#define DELTAPOS_TEST_BIG SAMPLEDATALOC "deltaPosTestBig2"
#define SUPPKEY_PATH SAMPLEDATALOC "d1.lsuppkey.full"

#define ORDERDATE_SHIPDATE_FULL TPCHDATALOC "D2.data"
#define D4_DATA TPCHDATALOC "D4.data"
//#define ORDERDATE_SHIPDATE_MINI SAMPLEDATALOC "d2.data.mini"

#define D1_DATA_MINI SAMPLEDATALOC "D1.data.mini"
#define D1_SHIPDATE_MINI SAMPLEDATALOC "D1.shipdate.mini"
#define D1_SUPPKEY_MINI SAMPLEDATALOC "D1.suppkey.mini"
#define D2_DATA_MINI SAMPLEDATALOC "D2.data.mini"
#define D3_DATA_MINI SAMPLEDATALOC "D3.data.mini"
#define D3_CUSTKEY_MINI SAMPLEDATALOC "D3.custkey.mini"
#define D4_DATA_MINI SAMPLEDATALOC "D4.data.mini"
#define D5_DATA_MINI SAMPLEDATALOC "D5.data.mini"
#define SUPP_DATA SAMPLEDATALOC "supp.data"

/*
OLD paths no longer used
#define D1_DATA_FULL "Data/wos-ros/D1.data."
#define D1_SHIPDATE_FULL "Data/wos-ros/D1.shipdate."
#define D1_SUPPKEY_FULL "Data/wos-ros/D1.suppkey."
#define D2_DATA_FULL "Data/wos-ros/D2.data."
#define D3_DATA_FULL "Data/wos-ros/D3.data."
#define D3_CUSTKEY_FULL "Data/wos-ros/D3.custkey."
#define D4_DATA_FULL "Data/wos-ros/D4.data."
#define D5_DATA_FULL "Data/wos-ros/D5.data."
*/

#define D6_DATA_ROS TPCHDATAWOSROSLOC "D6.data.ros"
#define D6_DATA_WOS TPCHDATAWOSROSLOC "D6.data.wos"
//#define D6_DATA_WOS_RANDOMIZED TPCHDATAWOSROSLOC "D6.data.wos.randomized"
//#define D6_DATA_WOS "temp/D6.data.wos"
//#define D6_DATA_WOS_2 TPCHDATAWOSROSLOC "D6.data.wos.2"
#define D7_DATA_ROS TPCHDATAWOSROSLOC "D7.data.ros"
#define D7_DATA_WOS TPCHDATAWOSROSLOC "D7.data.wos"
#define D8_DATA_ROS TPCHDATAWOSROSLOC "D8.data.ros"
#define D8_DATA_WOS TPCHDATAWOSROSLOC "D8.data.wos"
#define D9_DATA_ROS TPCHDATAWOSROSLOC "D9.data.ros"
#define D9_DATA_WOS TPCHDATAWOSROSLOC "D9.data.wos"
#define D10_DATA_ROS TPCHDATAWOSROSLOC "D10.data.ros"
#define D10_DATA_WOS TPCHDATAWOSROSLOC "D10.data.wos"

#define D6_FULL TPCHDATAWOSROSLOC "D6.data."
#define D7_FULL TPCHDATAWOSROSLOC "D7.data."
#define D8_FULL TPCHDATAWOSROSLOC "D8.data."
#define D9_FULL TPCHDATAWOSROSLOC "D9.data."
#define D10_FULL TPCHDATAWOSROSLOC "D10.data."

//Note: D1.data not included in release
//#define D1_COLUMN TPCHDATALOC "D1.data"

//replacing the following line with the next since I don't think this variable is used any more and don't want to include D1.data.retsuppsort in the release
//#define D1_RETSUPPSORT TPCHDATALOC "D1.data.retsuppsort"
#define D1_RETSUPPSORT TPCHDATALOC "D1.data.retsupp50sort"
#define D1_RETSUPP50SORT TPCHDATALOC "D1.data.retsupp50sort"
