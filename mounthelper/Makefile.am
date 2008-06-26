myexecpdir= $(DESTDIR)@OFS_SBIN@
myexecp_PROGRAMS = mount.ofs 
mount_ofs_SOURCES = ofsmount.cpp options.cpp usage.cpp

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/libraries/libofs \
	-I$(top_srcdir)/libraries/libofsconf -I$(top_srcdir)/libraries/libofshash $(all_includes)

# the library search path.
mount_ofs_LDFLAGS = $(all_libraries) 
noinst_HEADERS = options.h printusage.h
mount_ofs_LDADD = $(top_builddir)/libraries/libofshash/libofshash.la \
	$(top_builddir)/libraries/libofs/libofs.la $(top_builddir)/libraries/libofsconf/libofsconf.a -lconfuse


