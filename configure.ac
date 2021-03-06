AC_INIT([offlinefs], [0.4.7])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_TARGET

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE()

AC_LANG_CPLUSPLUS
AC_PROG_CXX
AM_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P

PKG_CHECK_MODULES([DBUS], [dbus-1])
PKG_CHECK_MODULES([FUSE], [fuse])
PKG_CHECK_MODULES([CONFUSE], [libconfuse])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h stdlib.h string.h sys/file.h sys/mount.h sys/socket.h sys/time.h syslog.h unistd.h utime.h])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_CHECK_FUNCS([ftruncate gethostbyname lchown memset mkdir mkfifo rmdir select socket strchr strerror strstr umount2 utime setxattr])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# taken from netatalk configure.in
AC_CHECK_HEADERS(sys/attributes.h attr/xattr.h sys/xattr.h sys/extattr.h sys/uio.h sys/ea.h)
# Do xattr functions take additional options like on Darwin?
AC_CACHE_CHECK([whether xattr interface takes additional options], xattr_cv_add_opt, [
	old_LIBS=$LIBS
	LIBS="$LIBS $ACL_LIBS"
	AC_TRY_COMPILE([
		#include <sys/types.h>
		#if HAVE_ATTR_XATTR_H
		#include <attr/xattr.h>
		#elif HAVE_SYS_XATTR_H
		#include <sys/xattr.h>
		#endif
	],[
		getxattr(0, 0, 0, 0, 0, 0);
	],
        [xattr_cv_add_opt=yes],
	[xattr_cv_add_opt=no;LIBS=$old_LIBS])
])
if test x"$xattr_cv_add_opt" = x"yes"; then
	AC_DEFINE(XATTR_ADD_OPT, 1, [xattr functions have additional options])
fi

AC_CACHE_CHECK([whether FUSE xattr interface takes additional options], fuse_cv_xattr_add_opt, [
	old_CPPFLAGS=$CPPFLAGS
	CPPFLAGS="$CPPFLAGS $FUSE_CFLAGS"
	AC_TRY_COMPILE([
		#define FUSE_USE_VERSION 26
		#include <fuse.h>
	],[
	    struct fuse_operations f;
		(f.getxattr)(0, 0, 0, 0, 0);
	],
        [fuse_cv_xattr_add_opt=yes;CPPFLAGS=$old_CPFLAGS],
	[fuse_cv_xattr_add_opt=no;CPPFLAGS=$old_CPFLAGS])
])
if test x"$fuse_cv_xattr_add_opt" = x"yes"; then
	AC_DEFINE(FUSE_XATTR_ADD_OPT, 1, [FUSE xattr functions have additional options])
fi
#
case "${target_os}" in
linux*)
	MOUNT_HELPER_PROG="mount.ofs"
	;;
darwin*|freebsd*)
	MOUNT_HELPER_PROG="mount_ofs"
	;;
esac
AC_SUBST(MOUNT_HELPER_PROG) 


AC_CONFIG_FILES([Makefile libraries/Makefile libraries/libofs/Makefile \
	libraries/libofsconf/Makefile libraries/libofshash/Makefile ofs-gui/Makefile \
	src/Makefile src/mount.ofs.8])
AC_OUTPUT
