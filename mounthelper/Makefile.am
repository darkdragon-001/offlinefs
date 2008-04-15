bin_PROGRAMS = ofsmount
ofsmount_SOURCES = ofsmount.cpp options.cpp usage.cpp

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/libraries/libofs \
	-I$(top_srcdir)/libraries/libofsconf $(all_includes)

# the library search path.
ofsmount_LDFLAGS = $(all_libraries) 
noinst_HEADERS = options.h printusage.h
ofsmount_LDADD = $(top_builddir)/libraries/libofsconf/libofsconf.a \
	$(top_builddir)/libraries/libofs/libofs.la -lconfuse
