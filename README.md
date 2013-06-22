LRCS
====
A Light-weight Read-Only Column Store

	
How to build:

	1. Install Fedora 17 (The current version is build on Fedora 17)
	
	2. Open SSH: 
		1. # systemctl start sshd.service   
		2. # systemctl enable sshd.service 
		3. # iptables -I INPUT -p tcp --dport 22 -j ACCEPT
		4. # iptables -I INPUT -p tcp --dport 5901 -j ACCEPT
		5. # iptables-save

	3. VNC Server:
		1. # yum install tigervnc-server
		2. cp /lib/systemd/system/vncserver@.service  /etc/systemd/system/vncserver@:1.service
		3. Change the parameter: ExecStart=/sbin/runuser -l vincezk -c "/usr/bin/vncserver :1 -geometry 1024x768 -depth 24"
		4. # sudo systemctl daemon-reload
		5. # sudo systemctl enable vncserver@:1.service
		6. # sudo systemctl start  vncserver@:1.service

	4. gsettings set org.gnome.settings-daemon.plugins.power lid-close-ac-action 'nothing' (һ��Ҫ�ڱ���ִ��)
	5. yum -y install binutils
	6. yum install gcc
	7. yum -y install wget
	8. yum install gcc-c++ libstdc++-devel -y
	9. yum -y install flex
	10. yum -y install ctags
	11. yum -y install ctags-etags
	12. yum -y install anjuta

		1. # yum -y install automake
		2. # yum -y install intltool
		3. # yum -y install libtool
		4. # yum -y install glib
		5. # yum install glib2-devel

	13. yum -y install bison
	14. install Berkeley DB

		1. cd /db-4.2.52/build_unix
		2. modify dist/configure according to i50696-berkeleydb.diff ��See Appendix��
		3. ../dist/configure --prefix=/usr/local/BerkeleyDB.4.2   --enable-cxx
		4. make
		5. make install
		6. vi /etc/ld.so.conf, add berkeley DB and LZO lib
		7. su root ldconfig
		8. Verify "g++ -o T test.cpp -L. -ldb -lpthread" 

	15. install LZO

		1. cd /LRCS/lzo-1.08
		2. ./configure --prefix=/GDB/lzo-1.08 --enable-shared
		3. make
		4. make install
		5. vi /etc/ld.so.conf, add  LZO lib
		6. su root ldconfig
		7. cp lzo-1.08/include/*.* /usr/include/

	16. Build CStore 

		1. cd /usr/local/BerkeleyDB.4.2
		2. ln -s libdb-4.2.so /usr/lib/libdb-4.2.so   (libdb_cxx-4.2.so, liblzo.so)
		3. cd /LRCS/src
		4. modify build/makefile.init: CC:=$(WTF) g++ -w -fpermissive
		5. modify codes to compatible new C++ compiler

			1. add "using namespace std;"
			2. add #include <stdlib.h> #include <string.h> .......
			3. Case { }
			4. Main.cpp: if (line.size()>1)
          line = StringUtil::ltrim(line, whitespace);

		6. modify src/makefile.target: etags -l c++ - <SRCS     etags --languages-force=c++ -L - <SRCS
		7. make



Customization:


	* Find a way to type SQL in command:

          ./LRCSqptest 1 test.cnf global.cnf      follow the steps to type SQL
./LRCSqptest 0 qa.cnf global.cnf 


	* Pass a SQL string to a method:

         Add a method in file /parser/lexer.l and /parser/lexer.cpp

PageWriter
         
gcc test.c -ggdb -I/usr/local/BerkeleyDB.4.2/include/ -L/usr/local/BerkeleyDB.4.2/lib/ -ldb -lpthread -o test.out
**************************************************************************************
Appendix:
i50696-berkeleydb.diff 
diff -ur ooo_ooo64bit02_src.orig/berkeleydb/db-4.2.52.patch ooo_ooo64bit02_src/berkeleydb/db-4.2.52.patch
--- ooo_ooo64bit02_src.orig/berkeleydb/db-4.2.52.patch  2005-06-13 21:16:39.640200612 +0200
+++ ooo_ooo64bit02_src/berkeleydb/db-4.2.52.patch       2005-06-13 21:16:48.810059771 +0200
@@ -205,6 +205,42 @@
 !         $(SLO)$/dbreg_util.obj
 ! 
 ! .INCLUDE :  target.mk
+*** misc/db-4.2.52.NC/dist/configure   Wed Dec  3 22:10:02 2003
+--- misc/build/db-4.2.52.NC/dist/configure     Mon Jun 13 21:04:50 2005
+***************
+*** 5624,5630 ****
+        # Check to see if the program is GNU ld.  I'd rather use --version,
+        # but apparently some GNU ld's only accept -v.
+        # Break only if it was the GNU/non-GNU ld that we prefer.
+!       case `"$lt_cv_path_LD" -v 2>&1 </dev/null` in
+        *GNU* | *'with BFD'*)
+       test "$with_gnu_ld" != no && break
+       ;;
+--- 5624,5630 ---- Line Number
+        # Check to see if the program is GNU ld.  I'd rather use --version,
+        # but apparently some GNU ld's only accept -v.
+        # Break only if it was the GNU/non-GNU ld that we prefer.
+!       case `$lt_cv_path_LD -v 2>&1 </dev/null` in
+        *GNU* | *'with BFD'*)
+       test "$with_gnu_ld" != no && break
+       ;;
+***************
+*** 11180,11186 ****
+      # linker, instead of GNU ld.  If possible, this setting should
+      # overridden to take advantage of the native linker features on
+      # the platform it is being used on.
+!     archive_cmds_CXX='$CC -shared $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags -o $lib'
+    fi
+  
+    # Commands to make compiler produce verbose output that lists
+--- 11180,11186 ----
+      # linker, instead of GNU ld.  If possible, this setting should
+      # overridden to take advantage of the native linker features on
+      # the platform it is being used on.
+!     archive_cmds_CXX='$CC -shared -nostdlib $predep_objects $libobjs $deplibs $postdep_objects $compiler_flags -o $lib'
+    fi
+  
+    # Commands to make compiler produce verbose output that lists
 *** misc/db-4.2.52.NC/env/makefile.mk  2005-04-07 17:09:42.000000000 +0200
 --- misc/build/db-4.2.52.NC/env/makefile.mk    2005-04-07 17:05:25.000000000 +0200
 ***************


DIRECTORY STRUCTURE:

LRCS/data: contains all data files (that were received when you did make data)
LRCS/Build: contains all build related files for project. Contains makefile.init which contains some of the important makefile flags used when compiling LRCS
LRCS/RuntimeData: all BerkeleyDB data files will be put in this directory
LRCS/src: root code source code directory. Also contains test.cnf and global.cnf which are described below.
LRCS/src/UnitTests: contains the unit tests that can be run via test.cnf
LRCS/src/AM: contains interface classes of LRCS with BerkeleyDB, currently also contains the catalog code
LRCS/src/TM: contains code for the tuple mover
LRCS/src/Util: contains some useful files for the project (e.g. an output logger, a stopwatch, and a HashMap)
LRCS/src/Operators: contains all operator code for LRCS
LRCS/src/Writers: contains all Block Writers for each compression type
LRCS/src/Wrappers: contains datasources, blocks, encoders, and decoders for each compression type
LRCS/src/common: contains some source code used by all parts of C-Store