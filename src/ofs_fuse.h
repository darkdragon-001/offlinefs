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
#ifndef OFS_FUSE_H
#define OFS_FUSE_H

#include <fusexx.hpp>
#include <string>
#include <dirent.h>

#include "file.h"

using namespace std;
#define HAVE_SETXATTR
// when calling getfattr -d filename attributes are only shown
// when starting with 'user.' - this might be a FUSE bug
#define OFS_ATTRIBUTE_OFFLINE "ofs.offline"
#define OFS_ATTRIBUTE_AVAILABLE "ofs.available"
#define OFS_ATTRIBUTE_VALUE_YES "yes"
#define OFS_ATTRIBUTE_VALUE_NO "no"

/**
	@author Tobias Jaehnel <tjaehnel@gmail.com>
	
	This class is based on the example 'fusexmp', which is located 
	in the 'example' subdirectory of the fuse sourcecode distribution
*/
class ofs_fuse : public fusexx::fuse<ofs_fuse> 
{
public:
	ofs_fuse();

	~ofs_fuse();
	// Overload the fuse methods
	static int fuse_getattr (const char *path, struct stat *stbuf);
	static int fuse_fgetattr(const char *path, struct stat *stbuf,
		struct fuse_file_info *fi);
	static int fuse_access(const char *path, int mask);
	static int fuse_readlink(const char *path, char *buf, size_t size);
	static int fuse_opendir(const char *path, struct fuse_file_info *fi);
	static int fuse_readdir(const char *path, void *buf,
		fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi);
	static int fuse_releasedir(const char *path,
		struct fuse_file_info *fi);
	static int fuse_mknod(const char *path, mode_t mode, dev_t rdev);
	static int fuse_mkdir(const char *path, mode_t mode);
	static int fuse_unlink(const char *path);
	static int fuse_rmdir(const char *path);
	static int fuse_symlink(const char *from, const char *to);
	static int fuse_rename(const char *from, const char *to);
	static int fuse_link(const char *from, const char *to);
	static int fuse_chmod(const char *path, mode_t mode);
	static int fuse_chown(const char *path, uid_t uid, gid_t gid);
	static int fuse_truncate(const char *path, off_t size);
	static int fuse_ftruncate(const char *path, off_t size,
			struct fuse_file_info *fi);
	static int fuse_utimens(const char *path, const struct timespec ts[2]);
	static int fuse_create(const char *path, mode_t mode,
			struct fuse_file_info *fi);
	static int fuse_open(const char *path, struct fuse_file_info *fi);
	static int fuse_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi);
	static int fuse_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi);
	static int fuse_statfs(const char *path, struct statvfs *stbuf);
	static int fuse_flush(const char *path, struct fuse_file_info *fi);
	static int fuse_release(const char *path, struct fuse_file_info *fi);
	static int fuse_fsync(const char *path, int isdatasync,
                     struct fuse_file_info *fi);
	static int fuse_setxattr(const char *path, const char *name,
			const char *value, size_t size, int flags);
	static int fuse_getxattr(const char *path, const char *name, char *value,
                    size_t size);
	static int fuse_listxattr(const char *path, char *list, size_t size);
	static int fuse_removexattr(const char *path, const char *name);
/*	static int fuse_lock(const char *path, struct fuse_file_info *fi, int cmd,
                    struct flock *lock);*/
	static void *fuse_init (struct fuse_conn_info *conn);

	std::string m_strHelloWorld;

private:
	// Private variables
	// Notice that they aren't static, i.e. they belong to an instantiated object
	std::string m_strPath;
	int cntnr;
	
	string basepath;

protected:
	string get_absolute_path(string rel_path);
	
	class openfile_info
	{
	public:
		openfile_info(const int fd_remote, const int fd_cache,
							const File fileinfo) :
			fd_remote(fd_remote), fd_cache(fd_cache),
			fileinfo(fileinfo) {};
		int get_fd_remote() const { return fd_remote; };
		int get_fd_cache() const { return fd_cache; };
		File get_fileinfo() const { return fileinfo; };
	private:
		int fd_remote;
		int fd_cache;
		File fileinfo;
	};

	class opendir_info
	{
	public:
		opendir_info(DIR *dirptr_remote, DIR *dirptr_cache,
				File fileinfo) :
			dirptr_remote(dirptr_remote), dirptr_cache(dirptr_cache),
			fileinfo(fileinfo) {};
		DIR *get_dirptr_remote() const { return dirptr_remote; };
		DIR *get_dirptr_cache() const { return dirptr_cache; };
		File get_fileinfo() const { return fileinfo; };
	private:
		DIR *dirptr_remote;
		DIR *dirptr_cache;
		File fileinfo;
	};

};

#endif
