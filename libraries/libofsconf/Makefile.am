METASOURCES = AUTO
OFS_STATE_DIR = \"${localstatedir}/ofs\"
noinst_LTLIBRARIES = libofsconf.la
libofsconf_la_CPPFLAGS = $(CONFUSE_CFLAGS) -DOFS_STATE_DIR=${OFS_STATE_DIR} -I$(top_srcdir)/libraries/libofs
libofsconf_la_SOURCES = ofsconf.cpp ofsconf.h
libofsconf_la_LIBADD = $(top_builddir)/libraries/libofs/libofs.la
