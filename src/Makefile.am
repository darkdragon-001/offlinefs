bin_PROGRAMS = ofs
ofs_SOURCES = filesystem.cpp filesystemstatusmanager.cpp mutex.cpp \
	mutexlocker.cpp ofs.cpp ofs_fuse.cpp

# set the include path found by configure
INCLUDES = $(all_includes) `pkg-config fuse --cflags-only-I`

# the library search path.
ofs_LDFLAGS = $(all_libraries) `pkg-config fuse --libs`
noinst_HEADERS = filesystem.h filesystems.h filesystemstatusmanager.h mutex.h \
	mutexlocker.h ofs_fuse.h
AM_CXXFLAGS = `pkg-config fuse --cflags-only-other`
