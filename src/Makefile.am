sbin_PROGRAMS = ofs
ofs_SOURCES = backingtree.cpp backingtreemanager.cpp backingtreepersistence.cpp \
	conflictmanager.cpp conflictpersistence.cpp file.cpp file_sync.cpp \
	filestatusmanager.cpp filesystemstatusmanager.cpp logger.cpp \
	offlinerecognizer.cpp ofs.cpp ofs_fuse.cpp ofsbroadcast.cpp ofsenvironment.cpp \
	ofsexception.cpp ofsfile.cpp ofslog.cpp persistable.cpp persistencemanager.cpp \
	synchronizationmanager.cpp synchronizationpersistence.cpp synclogentry.cpp synclogger.cpp \
	syncronisationmanager.cpp lazywrite.cpp

dist_man8_MANS = mount.ofs.8



# set the include path found by configure
ofs_CPPFLAGS = $(DBUS_CFLAGS) $(FUSE_CFLAGS) $(CONFUSE_CFLAGS) \
	-I$(top_srcdir)/libraries/libofs -I$(top_srcdir)/libraries/libofsconf \
	-I$(top_srcdir)/libraries/libofshash $(all_includes)

# the library search path.
ofs_LDFLAGS = $(all_libraries) $(DBUS_LIBS) $(FUSE_LIBS) $(CONFUSE_LIBS)
noinst_HEADERS = backingtreepersistence.h conflictmanager.h \
	conflictpersistence.h file.h file_sync.h filestatusmanager.h \
	offlinerecognizer.h ofs_fuse.h ofsenvironment.h ofsexception.h ofsfile.h ofslog.h \
	persistable.h persistencemanager.h synchronizationpersistence.h \
	syncronisationmanager.h syncstatetype.h backingtree.h filesystemstatusmanager.h\
	synchronizationmanager.h fusexx.hpp backingtreemanager.h logger.h synclogentry.h\
	ofsbroadcast.h synclogger.h lazywrite.h
AM_CXXFLAGS = -ansi
ofs_LDADD = $(top_builddir)/libraries/libofshash/libofshash.la \
	$(top_builddir)/libraries/libofsconf/libofsconf.la $(top_builddir)/libraries/libofs/libofs.la $(CONFUSE_LIBS)

MOUNT_HELPER_DIR = /sbin
MOUNT_HELPER = $(MOUNT_HELPER_DIR)/@MOUNT_HELPER_PROG@

install-exec-hook:
	${MKDIR_P} $(DESTDIR)${localstatedir}/ofs/remote
	${MKDIR_P} $(DESTDIR)${localstatedir}/ofs/backing
	${MKDIR_P} $(DESTDIR)$(MOUNT_HELPER_DIR) && \
	${LN_S} -f ${sbindir}/ofs $(DESTDIR)$(MOUNT_HELPER)
