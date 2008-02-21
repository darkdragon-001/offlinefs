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
#include "file.h"
#include <sys/time.h>
#include <unistd.h>

File::~File()
{
}

File::File(const bool offline_state, const bool availability,
		const string remote_path, const string cache_path) :
	offline_state(offline_state), availability(availability),
	remote_path(remote_path), cache_path(cache_path),
	dh_remote(NULL), dh_cache(NULL), fd_remote(0), fd_cache(0)
{}

string File::get_cache_path() const
{
	return cache_path;
}

bool File::get_offline_state() const
{
	return offline_state;
}

bool File::get_availability() const
{
	return availability;
}

string File::get_remote_path() const
{
	return remote_path;
}

File::File(const File &copy)
{
	operator =(copy);
}

File & File::operator =(const File &copy)
{
	offline_state = copy.offline_state;
	availability = copy.availability;
	remote_path = copy.remote_path;
	cache_path = copy.cache_path;

	return *this;
}


/**
 * Check file access permissions
 *
 * This will be called for the access() system call.
 * If the 'default_permissions' mount option is given,
 * this method is not called.
 *
 * This method is not called under Linux kernel versions 2.4.x
 * @param mask 
 * @return 
 */
int File::op_access(int mask)
{
	int res;

	res = access(get_remote_path().c_str(), mask);
	if (res == -1)
		return -errno;

	return 0;
}


/**
 * Change the permission bits of a file
 * @param mode 
 * @return 
 */
int File::op_chmod(mode_t mode)
{
	int res;

	res = chmod(get_remote_path().c_str(), mode);
	if (res == -1)
		return -errno;

	return 0;
}


/**
 * Get file attributes.
 *
 * Similar to stat(). The 'st_dev' and 'st_blksize' fields are ignored.
 * The 'st_ino' field is ignored except if the 'use_ino' mount option is given.
 * @param stbuf 
 * @return 
 */
int File::op_getattr(struct stat *stbuf)
{
	int res;
	res = lstat(get_remote_path().c_str(), stbuf);
	if (res == -1)
		return -errno;
	return 0;
}


/**
 * Read the target of a symbolic link
 *
 * The buffer should be filled with a null terminated string.
 * The buffer size argument includes the space for the terminating null
 * character. If the linkname is too long to fit in the buffer,
 * it should be truncated. The return value should be 0 for success.
 * @param buf 
 * @param size 
 * @return 
 */
int File::op_readlink(char *buf, size_t size)
{
	int res;

	res = readlink(get_remote_path().c_str(), buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}



/**
 * Change the owner and group of a file
 * @param uid 
 * @param gid 
 * @return 
 */
int File::op_chown(uid_t uid, gid_t gid)
{
	int res;

	res = lchown(get_remote_path().c_str(), uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * Create and open a file
 *
 * If the file does not exist, first create it with the specified mode,
 * and then open it.
 *
 * If this method is not implemented or under Linux kernel versions earlier
 * than 2.6.15, the mknod() and open() methods will be called instead.
 * @param mode 
 * @return 
 */
int File::op_create(mode_t mode, int flags)
{
	int fd;
	fd = open(get_remote_path().c_str(), flags, mode);
	if (fd == -1)
		return -errno;
	fd_remote = fd;	
	return 0;

}

/**
 * Get attributes from an open file
 *
 * This method is called instead of the getattr() method if the file 
 * information is available.
 * Currently this is only called after the create() method if that
 * is implemented (see above). Later it may be called for invocations of fstat() too.
 * @param stbuf 
 * @return 
 */
int File::op_fgetattr(struct stat *stbuf)
{
	int res;

	res = fstat(fd_remote, stbuf);
	if (res == -1)
		return -errno;
	return 0;
}

/**
 * Possibly flush cached data
 *
 * BIG NOTE: This is not equivalent to fsync().
 *           It's not a request to sync dirty data.
 *
 * Flush is called on each close() of a file descriptor. So if a filesystem
 * wants to return write errors in close() and the file has cached dirty data,
 * this is a good place to write back data and return any errors. Since many
 * applications ignore close() errors this is not always useful.
 *
 * NOTE: The flush() method may be called more than once for each open(). 
 * This happens if more than one file descriptor refers to an opened file
 * due to dup(), dup2() or fork() calls. It is not possible to determine if
 * a flush is final, so each flush should be treated equally. Multiple
 * write-flush sequences are relatively rare, so this shouldn't be a problem.
 *
 * Filesystems shouldn't assume that flush will always be called after some
 * writes, or that if will be called at all.
 * @return 
 */
int File::op_flush()
{
	// TODO: Implement this
}

/**
 * Synchronize file contents
 *
 * If the datasync parameter is non-zero, then only the user data should be
 * flushed, not the meta data.
 * @param isdatasync 
 * @return 
 */
int File::op_fsync(int isdatasync)
{
	int res;
#ifndef HAVE_FDATASYNC
	(void) isdatasync;
#else
	if (isdatasync)
		res = fdatasync(fd_remote);
	else
#endif
		res = fsync(fd_remote);
	if (res == -1)
		return -errno;
	return 0;
}

/**
 * Create a directory
 * @param mode 
 * @return 
 */
int File::op_mkdir(mode_t mode)
{
	int res;
	res = mkdir(get_remote_path().c_str(), mode);
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * Create a file node
 *
 * This is called for creation of all non-directory, non-symlink nodes.
 * If the filesystem defines a create() method, then for regular files
 * that will be called instead. 
 * @param mode 
 * @param rdev 
 * @return 
 */
int File::op_mknod(mode_t mode, dev_t rdev)
{
	int res;
	string remotepath = get_remote_path();

	if (S_ISFIFO(mode))
		res = mkfifo(remotepath.c_str(), mode);
	else
		res = mknod(remotepath.c_str(), mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * File open operation
 *
 * No creation, or truncation flags (O_CREAT, O_EXCL, O_TRUNC) will be 
 * passed to open(). Open should check if the operation is permitted for
 * the given flags. Optionally open may also return an arbitrary filehandle 
 * in the fuse_file_info structure, which will be passed
 * to all file operations.
 * @return 
 */
int File::op_open(int flags)
{
	int fd;
	fd = open(get_remote_path().c_str(), flags);
	if (fd == -1)
		return -errno;
	fd_remote = fd;
	
	return 0;
}

/**
 * Open directory
 *
 * @return 
 */
int File::op_opendir()
{
	dh_remote = opendir(get_remote_path().c_str());
	if (dh_remote == NULL)
		return -errno;	
	return 0;
}

/**
 * Read data from an open file
 *
 * Read should return exactly the number of bytes requested except on EOF or
 * error, otherwise the rest of the data will be substituted with zeroes.
 * An exception to this is when the 'direct_io' mount option is specified, in
 * which case the return value of the read system call will reflect the
 * return value of this operation.
 
 * @param buf 
 * @param size 
 * @param offset 
 * @return 
 */
int File::op_read(char *buf, size_t size, off_t offset)
{
	int res;
	res = pread(fd_remote, buf, size, offset);
	if (res == -1)
		res = -errno;
	return res;
}


/**
 * Read directory
 *
 * This supersedes the old getdir() interface.
 * New applications should use this.
 *
 * The filesystem may choose between two modes of operation:
 *
 * 1) The readdir implementation ignores the offset parameter, and passes
 *    zero to the filler function's offset. The filler function will not
 *    return '1' (unless an error happens), so the whole directory is read in
 *    a single readdir operation. This works just like the old getdir() method.
 *
 * 2) The readdir implementation keeps track of the offsets of the directory
 *    entries. It uses the offset parameter and always passes non-zero offset
 *    to the filler function. When the buffer is full (or an error happens)
 *    the filler function will return '1'.
 * @param buf 
 * @param filler 
 * @param offset 
 * @return 
 */
int File::op_readdir(void *buf, fuse_fill_dir_t filler, off_t offset)
{
	if (dh_remote == NULL)
	{
		errno = EBADF;
		return -errno;
	}

	struct dirent *de;

	seekdir(dh_remote, offset);
	while ((de = readdir(dh_remote)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, telldir(dh_remote)))
			break;
	}
	return 0;
}

/**
 * Release an open file
 *
 * Release is called when there are no more references to an open file:
 * all file descriptors are closed and all memory mappings are unmapped.
 * 
 * For every open() call there will be exactly one release() call with the
 * same flags and file descriptor. It is possible to have a file opened more
 * than once, in which case only the last release will mean, that no more
 * reads/writes will happen on the file.
 * The return value of release is ignored.
 * @return 
 */

int File::op_release()
{
	int res;
	
	if(close(fd_remote))
		return -errno;
	fd_remote = 0;
	
	return 0;
}

/**
 * Release directory
 * @return 
 */
int File::op_releasedir()
{
	if (!dh_remote)
	{
		errno = EBADF;
		return -errno;
	}
	
	return closedir(dh_remote);
}

/**
 * Remove the directory
 * @return 
 */
int File::op_rmdir()
{
	int res;
	res = rmdir(get_remote_path().c_str());
	if (res == -1)
		return -errno;
	return 0;
}

/**
 * Get file system statistics
 *
 * The 'f_frsize', 'f_favail', 'f_fsid' and 'f_flag' fields are ignored
 * @param stbuf 
 * @return 
 */
int File::op_statfs(struct statvfs *stbuf)
{
	int res;
	res = statvfs(get_remote_path().c_str(), stbuf);
	if (res == -1)
		return -errno;
	return 0;
}

/**
 * Change the size of a file
 * @param path 
 * @param size 
 * @return 
 */
int File::op_truncate(off_t size)
{
	int res;
	res = truncate(get_remote_path().c_str(), size);
	if (res == -1)
		return -errno;
	return 0;
}

/**
 * Change the size of a file
 * @param size 
 * @return 
 */
int File::op_ftruncate(off_t size)
{
	int res;
	
	if (!fd_remote)
	{
		errno = EBADF;
		return -errno;
	}

	res = ftruncate(fd_remote, size);
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * Remove the file
 * @return 
 */
int File::op_unlink()
{
	int res;
	res = unlink(get_remote_path().c_str());
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * Change the access and modification times of
 * a file with nanosecond resolution
 * @param ts[] 
 * @return 
 */
int File::op_utimens(const struct timespec ts[2])
{
	int res;

	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(get_remote_path().c_str(), tv);
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * Write data to an open file
 *
 * Write should return exactly the number of bytes requested except on 
 * error. An exception to this is when the 'direct_io' mount option is
 * specified (see read operation).
 * @param buf 
 * @param size 
 * @param offset 
 * @return 
 */
int File::op_write(const char *buf, size_t size, off_t offset)
{
	int res;
	res = pwrite(fd_remote, buf, size, offset);
	if (res == -1)
		res = -errno;
	return res;
}



/**
 * Create a symbolic link
 * TODO: I think 'from' is the exact path as specified in the command line.
 *       The 'to' parameter is relative to the current filesystem
 *       Check this!
 * @param from 
 * @return 
 */
int File::op_symlink(const char* from)
{
	int res;

	res = symlink(from, get_remote_path().c_str());
	if (res == -1)
		return -errno;
	return 0;
}

/**
 * Rename a file
 * @param from 
 * @param to 
 * @return 
 */
int File::op_rename(File *to)
{
	int res;
	res = rename(get_remote_path().c_str(), to->get_remote_path().c_str());
	if (res == -1)
		return -errno;

	return 0;
}


/**
 * Create a hard link to a file
 * @param from 
 * @param to 
 * @return 
 */
int File::op_link(File *to)
{
	int res;
	res = link(get_remote_path().c_str(),
		to->get_remote_path().c_str());
	if (res == -1)
		return -errno;
	return 0;
}


/*!
 *  If this file is available on the remote machine
 *  and has changed, update it
 *  TODO: implement exceptions
 *  \fn File::update_local()
 */
void File::update_local()
{
	struct stat fileinfo;
	time_t mtime_remote, mtime_cache;

	if (get_offline_state() && get_availability()) {
		// TODO: throw exception on error
		stat(get_remote_path().c_str(), &fileinfo);
		mtime_remote = fileinfo.st_mtime;
		stat(get_cache_path().c_str(), &fileinfo);
		mtime_cache = fileinfo.st_mtime;
		// if the remote file has changed
		// copy it to the cache
		// TODO: This should be more elegant
		if (mtime_remote > mtime_cache) {
			execl("cp", get_remote_path().c_str(), get_cache_path().c_str());
		}
	}
}
