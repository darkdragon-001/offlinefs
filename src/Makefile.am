bin_PROGRAMS = ofs
ofs_SOURCES = backingtree.cpp backingtreemanager.cpp backingtreepersistence.cpp \
	file.cpp file_sync.cpp fileref.cpp filestatusmanager.cpp \
	filesystemstatusmanager.cpp ofs.cpp ofs_fuse.cpp ofsenvironment.cpp ofsexception.cpp ofsfile.cpp \
	persistable.cpp persistencemanager.cpp syncronisationmanager.cpp

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/libraries/libofs \
	-I$(top_srcdir)/libraries/libofsconf -I$(top_srcdir)/libraries/libofshash $(all_includes) \
	`pkg-config dbus-1 --cflags-only-I` `pkg-config fuse --cflags-only-I`

# the library search path.
ofs_LDFLAGS = $(all_libraries) `pkg-config fuse --libs` `pkg-config dbus-1 --libs` -lconfuse
noinst_HEADERS = backingtreepersistence.h file.h file_sync.h fileref.h \
	filestatusmanager.h ofs_fuse.h ofsenvironment.h ofsexception.h ofsfile.h persistable.h \
	persistencemanager.h syncronisationmanager.h syncstatetype.h
AM_CXXFLAGS = `pkg-config fuse --cflags-only-other` `pkg-config dbus-1 --cflags-only-other`
ofs_LDADD = $(top_builddir)/libraries/libofshash/libofshash.la \
	$(top_builddir)/libraries/libofsconf/libofsconf.a $(top_builddir)/libraries/libofs/libofs.la
