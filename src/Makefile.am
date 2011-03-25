sbin_PROGRAMS = ofs
ofs_SOURCES = backingtree.cpp backingtreemanager.cpp backingtreepersistence.cpp \
	conflictmanager.cpp conflictpersistence.cpp file.cpp file_sync.cpp \
	filestatusmanager.cpp filesystemstatusmanager.cpp logger.cpp \
	offlinerecognizer.cpp ofs.cpp ofs_fuse.cpp ofsbroadcast.cpp ofsenvironment.cpp \
	ofsexception.cpp ofsfile.cpp ofslog.cpp persistable.cpp persistencemanager.cpp \
	synchronizationmanager.cpp synchronizationpersistence.cpp synclogentry.cpp synclogger.cpp \
	syncronisationmanager.cpp

# set the include path found by configure
INCLUDES = -I$(top_srcdir)/libraries/libofs \
	-I$(top_srcdir)/libraries/libofsconf -I$(top_srcdir)/libraries/libofshash $(all_includes) 
ofs_CPPFLAGS = $(DBUS_CFLAGS) $(FUSE_CFLAGS) $(CONFUSE_CFLAGS)
 
# the library search path.
ofs_LDFLAGS = $(all_libraries) $(DBUS_LIBS) $(FUSE_LIBS) $(CONFUSE_LIBS)
noinst_HEADERS = backingtreepersistence.h conflictmanager.h \
	conflictpersistence.h file.h file_sync.h filestatusmanager.h \
	offlinerecognizer.h ofs_fuse.h ofsenvironment.h ofsexception.h ofsfile.h ofslog.h \
	persistable.h persistencemanager.h synchronizationpersistence.h \
	syncronisationmanager.h syncstatetype.h backingtree.h filesystemstatusmanager.h\
	synchronizationmanager.h fusexx.hpp backingtreemanager.h logger.h synclogentry.h\
	ofsbroadcast.h synclogger.h
AM_CXXFLAGS = -ansi
ofs_LDADD = $(top_builddir)/libraries/libofshash/libofshash.la \
	$(top_builddir)/libraries/libofsconf/libofsconf.la $(top_builddir)/libraries/libofs/libofs.la
