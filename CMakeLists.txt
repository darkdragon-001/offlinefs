cmake_minimum_required(VERSION 3.8)

project(ofs-bin LANGUAGES C CXX)

set(OFS_BIN_SOURCES_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
add_executable(${PROJECT_NAME}
	${OFS_BIN_SOURCES_DIR}/conflictmanager.cpp
	${OFS_BIN_SOURCES_DIR}/filestatusmanager.cpp
	${OFS_BIN_SOURCES_DIR}/offlinerecognizer.cpp
	${OFS_BIN_SOURCES_DIR}/ofsexception.cpp
	${OFS_BIN_SOURCES_DIR}/synchronizationmanager.cpp
	${OFS_BIN_SOURCES_DIR}/syncronisationmanager.cpp
  ${OFS_BIN_SOURCES_DIR}/backingtree.cpp
  ${OFS_BIN_SOURCES_DIR}/backingtreemanager.cpp
  ${OFS_BIN_SOURCES_DIR}/backingtreepersistence.cpp
  ${OFS_BIN_SOURCES_DIR}/conflictpersistence.cpp
  ${OFS_BIN_SOURCES_DIR}/file_sync.cpp
  ${OFS_BIN_SOURCES_DIR}/file.cpp
  ${OFS_BIN_SOURCES_DIR}/filesystemstatusmanager.cpp
  ${OFS_BIN_SOURCES_DIR}/lazywrite.cpp
  ${OFS_BIN_SOURCES_DIR}/logger.cpp
  ${OFS_BIN_SOURCES_DIR}/ofs_fuse.cpp
  ${OFS_BIN_SOURCES_DIR}/ofs.cpp
  ${OFS_BIN_SOURCES_DIR}/ofsbroadcast.cpp
  ${OFS_BIN_SOURCES_DIR}/ofsenvironment.cpp 
  ${OFS_BIN_SOURCES_DIR}/ofsfile.cpp
  ${OFS_BIN_SOURCES_DIR}/ofslog.cpp
  ${OFS_BIN_SOURCES_DIR}/persistable.cpp
  ${OFS_BIN_SOURCES_DIR}/persistencemanager.cpp 
  ${OFS_BIN_SOURCES_DIR}/synchronizationpersistence.cpp
  ${OFS_BIN_SOURCES_DIR}/synclogentry.cpp
  ${OFS_BIN_SOURCES_DIR}/synclogger.cpp 
)
set_target_properties(ofs-bin
  PROPERTIES
    CXX_STANDARD 11
    OUTPUT_NAME ofs
)

target_compile_definitions(${PROJECT_NAME}
  PUBLIC
    OFS_STATE_DIR="${CMAKE_INSTALL_PREFIX}/var/ofs"
)

target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src
)

################
# dependencies #
################

find_package(DBus1 REQUIRED)
find_package(PkgConfig)
pkg_check_modules(FUSE REQUIRED fuse)
pkg_check_modules(CONFUSE REQUIRED libconfuse)
add_subdirectory(libraries/libofs)
add_subdirectory(libraries/libofsconf)
add_subdirectory(libraries/libofshash)

target_compile_options(${PROJECT_NAME}
  PUBLIC
    ${FUSE_CFLAGS}
    ${CONFUSE_CFLAGS}
)

target_link_directories(${PROJECT_NAME}
  PUBLIC
    ${FUSE_LIBRARY_DIRS}
    ${CONFUSE_LIBRARY_DIRS}
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    dbus-1
    ${FUSE_LIBRARIES}
    ${CONFUSE_LIBRARIES}
    ofs
    ofsconf
    ofshash
)

##########
# checks #
##########

include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckFunctionExists)
unset(HAVE_REQUIRED_HEADERS CACHE)
check_include_files(
  "arpa/inet.h;fcntl.h;netdb.h;stdlib.h;string.h;sys/file.h;sys/mount.h;sys/socket.h;sys/time.h;syslog.h;unistd.h;utime.h"
  HAVE_REQUIRED_HEADERS
)
unset(HAVE_REQUIRED_FUNCTIONS CACHE)
check_function_exists(
  "ftruncate;gethostbyname;lchown;memset;mkdir;mkfifo;rmdir;select;socket;strchr;strerror;strstr;utime;setxattr"
  HAVE_REQUIRED_FUNCTIONS
)
if(NOT HAVE_REQUIRED_FUNCTIONS OR NOT HAVE_REQUIRED_FUNCTIONS)
  message(FATAL_ERROR "Not all required headers or functions are available.")
endif()
check_include_file(
  "sys/mount.h"
  HAVE_SYS_MOUNT_H
)
if(HAVE_SYS_MOUNT_H)
  target_compile_definitions(${PROJECT_NAME}
    PUBLIC
      HAVE_SYS_MOUNT_H
  )
endif()
check_function_exists(
  "umount2"
  HAVE_UMOUNT2
)
if(HAVE_UMOUNT2)
  target_compile_definitions(${PROJECT_NAME}
    PUBLIC
      HAVE_UMOUNT2
  )
endif()
check_function_exists(
  "fdatasync"
  HAVE_FDATASYNC
)
if(HAVE_FDATASYNC)
  target_compile_definitions(${PROJECT_NAME}
    PUBLIC
      HAVE_FDATASYNC
  )
endif()

################
# xattr checks #
################

pkg_check_modules(ATTR libattr)
check_include_file(
  "attr/xattr.h"
  HAVE_ATTR_XATTR_H
)
if(HAVE_ATTR_XATTR_H)
  target_compile_definitions(${PROJECT_NAME}
    PUBLIC
      HAVE_ATTR_XATTR_H
  )
endif()
check_include_file(
  "sys/xattr.h"
  HAVE_SYS_XATTR_H
)
if(HAVE_SYS_XATTR_H)
  target_compile_definitions(${PROJECT_NAME}
    PUBLIC
      HAVE_SYS_XATTR_H
  )
endif()
check_function_exists(
  "setxattr"
  HAVE_SETXATTR
)
if(HAVE_SETXATTR)
  target_compile_definitions(${PROJECT_NAME}
    PUBLIC
      HAVE_SETXATTR
  )
endif()

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.19.0")
  # CheckSourceCompiles was added in CMake 3.19
  include(CheckSourceCompiles)
  check_source_compiles(
    C
    "
      #include <sys/types.h>
      #if HAVE_ATTR_XATTR_H
      #include <attr/xattr.h>
      #elif HAVE_SYS_XATTR_H
      #include <sys/xattr.h>
      #endif
      getxattr(0, 0, 0, 0, 0, 0);
    "
    COMPILES_XATTR_ADD_OPT
  )
  if (COMPILES_XATTR_ADD_OPT)
    message("xattr functions have additional options")
    target_compile_definitions(${PROJECT_NAME}
      PUBLIC
        XATTR_ADD_OPT=1
    )
  endif()

  check_source_compiles(
    C
    "
      #define FUSE_USE_VERSION 26
      #include <fuse.h>
      struct fuse_operations f;
      (f.getxattr)(0, 0, 0, 0, 0);
    "
    COMPILES_FUSE_CV_XATTR_ADD_OPT
  )
  if (COMPILES_FUSE_CV_XATTR_ADD_OPT)
    message("FUSE xattr functions have additional options")
    target_compile_definitions(${PROJECT_NAME}
      PUBLIC
        FUSE_XATTR_ADD_OPT=1
    )
  endif()
else()
    message("Please consider to switch to CMake 3.19.0")
endif()

###########
# install #
###########

set(CMAKE_INSTALL_BINDIR sbin)
install(TARGETS ${PROJECT_NAME})

if(UNIX)
  if(APPLE)
    set(MOUNT_HELPER_PROG "mount_ofs")
  else()  # Linux
    set(MOUNT_HELPER_PROG "mount.ofs")
  endif()
else()
  message(FATAL_ERROR "FUSE is only available on UNIX.")
endif()
case "${target_os}" in
linux*)
	MOUNT_HELPER_PROG="mount.ofs"
	;;
darwin*|freebsd*)
	MOUNT_HELPER_PROG="mount_ofs"
	;;
esac
install(DIRECTORY DESTINATION /sbin)
install(CODE "execute_process( \
    COMMAND ${CMAKE_COMMAND} -E create_symlink \
    ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/ofs \
    /sbin/${MOUNT_HELPER_PROG}   \
    )"
)
install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/man8 TYPE MAN)

install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/var/ofs/remote)
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/var/ofs/backing)
