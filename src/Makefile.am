bin_PROGRAMS = ofs
ofs_SOURCES = ofs.cpp ofs_fuse.cpp

# set the include path found by configure
INCLUDES = $(all_includes) `pkg-config fuse --cflags-only-I`

# the library search path.
ofs_LDFLAGS = $(all_libraries) `pkg-config fuse --libs`
noinst_HEADERS = ofs_fuse.h
AM_CXXFLAGS = `pkg-config fuse --cflags-only-other`
