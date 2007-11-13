bin_PROGRAMS = ofs
ofs_SOURCES = backingtree.cpp backingtreemanager.cpp file.cpp fileref.cpp \
	filestatusmanager.cpp mutex.cpp mutexlocker.cpp ofs.cpp ofs_fuse.cpp

# set the include path found by configure
INCLUDES = $(all_includes) `pkg-config fuse --cflags-only-I`

# the library search path.
ofs_LDFLAGS = $(all_libraries) `pkg-config fuse --libs`
noinst_HEADERS = file.h fileref.h filestatusmanager.h mutex.h mutexlocker.h \
	ofs_fuse.h
AM_CXXFLAGS = `pkg-config fuse --cflags-only-other`
