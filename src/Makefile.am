myexecpdir= $(DESTDIR)@OFS_SBIN@
myexecp_PROGRAMS = ofs
ofs_SOURCES = backingtree.cpp backingtreemanager.cpp backingtreepersistence.cpp \
	conflictmanager.cpp conflictpersistence.cpp file.cpp file_sync.cpp fileref.cpp \
	filestatusmanager.cpp filesystemstatusmanager.cpp logger.cpp ofs.cpp ofs_fuse.cpp \
	ofsbroadcast.cpp ofsenvironment.cpp ofsexception.cpp ofsfile.cpp persistable.cpp \
	persistencemanager.cpp synchronizationmanager.cpp synchronizationpersistence.cpp \
	synclogentry.cpp synclogger.cpp syncronisationmanager.cpp ofslog.cpp

# set the include path found by configure
INCLUDES = -I$(kernel_includes) -I$(asm_includes) \ 
	-I$(top_srcdir)/libraries/libofs \
	-I$(top_srcdir)/libraries/libofsconf -I$(top_srcdir)/libraries/libofshash $(all_includes) \
	`pkg-config dbus-1 --cflags-only-I` `pkg-config fuse --cflags-only-I`

# the library search path.
ofs_LDFLAGS = $(all_libraries) `pkg-config fuse --libs` `pkg-config dbus-1 --libs` -lconfuse
noinst_HEADERS = backingtreepersistence.h conflictmanager.h \
	conflictpersistence.h file.h file_sync.h fileref.h filestatusmanager.h ofs_fuse.h \
	ofsenvironment.h ofsexception.h ofsfile.h persistable.h persistencemanager.h \
	synchronizationpersistence.h syncronisationmanager.h syncstatetype.h ofslog.h
AM_CXXFLAGS = `pkg-config fuse --cflags-only-other` `pkg-config dbus-1 --cflags-only-other` -ansi
ofs_LDADD = $(top_builddir)/libraries/libofshash/libofshash.la \
	$(top_builddir)/libraries/libofsconf/libofsconf.a $(top_builddir)/libraries/libofs/libofs.la
