bin_PROGRAMS = ofs
ofs_SOURCES = backingtree.cpp backingtreemanager.cpp backingtreepersistence.cpp \
	file.cpp fileref.cpp filestatusmanager.cpp mutex.cpp mutexlocker.cpp ofs.cpp \
	ofs_fuse.cpp ofsexception.cpp ofsfile.cpp persistencemanager.cpp

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/libraries/libofsconf \
	-I$(top_srcdir)/libraries/libofshash $(all_includes) `pkg-config fuse --cflags-only-I`

# the library search path.
ofs_LDFLAGS = $(all_libraries) `pkg-config fuse --libs` -lconfuse
noinst_HEADERS = backingtreepersistence.h file.h fileref.h filestatusmanager.h \
	mutex.h mutexlocker.h ofs_fuse.h ofsexception.h ofsfile.h persistencemanager.h
AM_CXXFLAGS = `pkg-config fuse --cflags-only-other`
ofs_LDADD = $(top_builddir)/libraries/libofshash/libofshash.la
