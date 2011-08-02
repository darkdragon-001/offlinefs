/***************************************************************************
 *   Copyright (C) 2007 by                                                 *
 *                 Frank Gsellmann, Tobias Jaehnel, Carsten Kolassa        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef OFSFILE_H
#define OFSFILE_H

#include "file.h"
#include "conflictmanager.h"
#include <string>
#include <fusexx.hpp>
#include <dirent.h>

// when calling getfattr -d filename attributes are only shown
// when starting with 'user.' - this might be a FUSE bug
#define OFS_ATTRIBUTE_OFFLINE "ofs.offline"
#define OFS_ATTRIBUTE_AVAILABLE "ofs.available"
#define OFS_ATTRIBUTE_STATE "ofs.offlinestate"
#define OFS_ATTRIBUTE_CONFLICT "ofs.conflict"
#define OFS_ATTRIBUTE_VALUE_YES "yes"
#define OFS_ATTRIBUTE_VALUE_NO "no"
#define OFS_ATTRIBUTE_VALUE_CURRENT "current"
#define OFS_ATTRIBUTE_VALUE_OUTDATED "outdated"
#define OFS_ATTRIBUTE_VALUE_CONFLICT "conflict"
#define OFS_ATTRIBUTE_VALUE_UPDATING "updating"
#define OFS_ATTRIBUTE_VALUE_REINTEGRATING "reintegrating"
/**
	@author Tobias Jaehnel <tjaehnel@gmail.com>
	The Object represents one open file. It holds file/directory
	handles and is responsible for performing the
	operations on this file.
	Most of the methods are called by the ofs_fuse callback functions

	TODO: There might be one class for directories and one for files
*/
class OFSFile{
public:
    explicit OFSFile(const string path);
    explicit OFSFile(const char *path);
    int op_access(int mask);
    int op_getattr(struct stat *stbuf);
    int op_readlink(char *buf, size_t size);
    int op_chmod(mode_t mode);
    int op_chown(uid_t uid, gid_t gid);
    int op_create(mode_t mode);
    int op_fgetattr(struct stat *stbuf);
    int op_flush();
    int op_fsync(int isdatasync);
    int op_mkdir(mode_t mode);
    int op_mknod(mode_t mode, dev_t rdev);
    int op_open(int flags);
    int op_opendir();
    int op_read(char *buf, size_t size, off_t offset);
    int op_readdir(void *buf, fuse_fill_dir_t filler, off_t offset);
    int op_release();
    int op_releasedir();
    int op_rmdir();
    int op_statfs(struct statvfs *stbuf);
    int op_truncate(off_t size);
    int op_ftruncate(off_t size);
    int op_unlink();
    int op_utimens(const struct timespec ts[2]);
    int op_write(const char *buf, size_t size, off_t offset);
    int op_rename(OFSFile *to);
    int op_link(OFSFile *from);
    int op_symlink(const char* from);
    void update_cache();
    OFSFile * get_parent_directory();
    void update_amtime();
#if (__FreeBSD__ >= 10)
    int op_getxattr(const char *name, char *value, size_t size, uint32_t position);
#else
    int op_getxattr(const char *name, char *value, size_t size);
#endif
#if (__FreeBSD__ >= 10)
    int op_setxattr(const char *name, const char *value, size_t size, int flags, uint32_t position);
#else
    int op_setxattr(const char *name, const char *value, size_t size, int flags);
#endif
    ~OFSFile();
    inline string get_remote_path() { return fileinfo.get_remote_path(); }
    inline string get_cache_path() { return fileinfo.get_cache_path(); }
    inline bool get_availability() { return fileinfo.get_availability(); }
    inline bool get_offline_state() { return fileinfo.get_offline_state(); }
    inline string get_relative_path() { return fileinfo.get_relative_path(); }
    inline bool isConflictPath() { return
         ConflictManager::Instance().isConflicted(get_relative_path()); };
    int op_removexattr(const char *name);
    int op_listxattr(char *list, size_t size);
    void savemtime();
    bool filesync();
private:
    File fileinfo;
    DIR *dh_cache;
    DIR *dh_remote;
    int fd_cache;
    int fd_remote;
};

#endif
