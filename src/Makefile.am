bin_PROGRAMS = ofs
ofs_SOURCES = ofs.cpp ofs_fuse.cpp

# set the include path found by configure
INCLUDES = -I/usr/local/include/fuse $(all_includes)

# the library search path.
ofs_LDFLAGS = $(all_libraries) 
noinst_HEADERS = ofs_fuse.h
AM_CXXFLAGS = `pkg-config fuse --cflags`
