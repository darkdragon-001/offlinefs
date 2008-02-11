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
#ifndef FILE_H
#define FILE_H
#include <string>
#include <fusexx.hpp>
#include <dirent.h>

using namespace std;
/**
	@author Carsten Kolassa <Carsten@Kolassa.de>
	@author Tobias Jaehnel <tjaehnel@gmail.com>
	The file Object represents one file in the filesystem. It holds all
	ofs-specific infomation and is responsible for performing the
	operations on this file.
	Most of the methods are called by the ofs_fuse callback functionss
	
	Instances are created and filled by the filestatusmanager.
*/
class File{
public:
	File(bool offline_state, bool availability,
		string remote_path, string cache_path);
	File(const File &copy);
	File& operator =(const File &copy);
	bool get_offline_state() const;
	bool get_availability() const;
	string get_remote_path() const;
	string get_cache_path() const;
	~File();
    int op_access(int mask);
    int op_getattr(struct stat *stbuf);
    int op_readlink(char *buf, size_t size);
    int op_chmod(mode_t mode);
    int op_chown(uid_t uid, gid_t gid);
    int op_create(mode_t mode, int flags);
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
    int op_rename(File *to);
    int op_link(File *from);
    int op_symlink(const char* from);
private:
	bool offline_state;
	bool availability;
	string remote_path;
	string cache_path;
	DIR *dh_cache;
	DIR *dh_remote;
	int fd_cache;
	int fd_remote;
};

#endif