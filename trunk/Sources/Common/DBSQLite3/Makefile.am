lspcdir = ./../../Linux/LinuxSpec
lib_LTLIBRARIES = libDbSqlite3.la
libDbSqlite3_la_SOURCES = CDBSQLite3.h CDBSQLite3.cpp CppSQLite3.h CppSQLite3.cpp tools.cpp tools.h DBSQLite3.cpp sqlite3.h
libDbSqlite3_la_CPPFLAGS = -I../CommonFiles -I$(lspcdir) -I../libCommon
libDbSqlite3_la_LDFLAGS = -lpthread
libDbSqlite3_la_LIBADD = ../libCommon/libCommon.a $(lspcdir)/libLinuxSpec.a /usr/local/lib/libsqlite3.a
