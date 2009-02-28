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
#include "ofsfile.h"
#include "synclogger.h"
#include "filestatusmanager.h"
#include "backingtreemanager.h"
#include "ofsbroadcast.h"
#include "ofsenvironment.h"
#include "synchronizationmanager.h"
#include <sys/time.h>
#include <unistd.h>
#include <ofsexception.h>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <utime.h>
#include <cstring>
#include <attr/xattr.h>

OFSFile::OFSFile(const string path) : dh_cache(NULL), dh_remote(NULL),
	fd_cache(0), fd_remote(0),
	fileinfo(Filestatusmanager::Instance().give_me_file(path.c_str()))
{}

OFSFile::OFSFile(const char *path) : dh_cache(NULL), dh_remote(NULL),
	fd_cache(0), fd_remote(0),
	fileinfo(Filestatusmanager::Instance().give_me_file(path))
{}


OFSFile::~OFSFile()
{
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
int OFSFile::op_access(int mask)
{
	int res;
	if(get_availability())
		res = access(get_remote_path().c_str(), mask);
	else
		res = access(get_cache_path().c_str(), mask);
	if (res == -1)
		return -errno;

	return 0;
}


/**
 * Change the permission bits of a file
 * @param mode 
 * @return 
 */
int OFSFile::op_chmod(mode_t mode)
{
	int res;
	try {
		update_cache();
		
		if (get_offline_state())
			res = chmod(get_cache_path().c_str(), mode);
		if (res == -1)
			return -errno;
		if (get_availability())
			res = chmod(get_remote_path().c_str(), mode);
		if (res == -1)
			return -errno;
		update_amtime();
		return 0;
	} catch(OFSException &e) {
		errno = e.get_posixerrno();
		return -errno;
	}
}


/**
 * Get file attributes.
 *
 * Similar to stat(). The 'st_dev' and 'st_blksize' fields are ignored.
 * The 'st_ino' field is ignored except if the 'use_ino' mount option is given.
 * @param stbuf 
 * @return 
 */
int OFSFile::op_getattr(struct stat *stbuf)
{
	int res;
	if(get_availability())
	{
		res = lstat(get_remote_path().c_str(), stbuf);
	} else {
		res = lstat(get_cache_path().c_str(), stbuf);
	}
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
int OFSFile::op_readlink(char *buf, size_t size)
{
	int res;
	try {
		update_cache();
		
		if (get_availability())
			res = readlink(get_remote_path().c_str(), buf, size - 1);
		else
			res = readlink(get_cache_path().c_str(), buf, size - 1);
		if (res == -1)
			return -errno;
		
		update_amtime();
		buf[res] = '\0';
		return 0;
	} catch(OFSException &e) {
		errno = e.get_posixerrno();
		return -errno;
	}
}



/**
 * Change the owner and group of a file
 * @param uid 
 * @param gid 
 * @return 
 */
int OFSFile::op_chown(uid_t uid, gid_t gid)
{
	int res = 0;
	try {
		update_cache();

		if (get_offline_state())
			res = lchown(get_cache_path().c_str(), uid, gid);
		if (res == -1)
			return -errno;
		if (get_availability())
			res = lchown(get_remote_path().c_str(), uid, gid);
		if (res == -1)
			return -errno;
		update_amtime();
		return 0;
		
	} catch(OFSException &e) {
		errno = e.get_posixerrno();
		return -errno;	
	}
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
int OFSFile::op_create(mode_t mode, int flags)
{
	int fdr=0, fdc=0, nRet = 0;
	try {		
		update_cache();

		if (get_offline_state()) {
			fdc = open(get_cache_path().c_str(), flags, mode);
			if (fdc == -1)
			{
				// Sends a signal: Couldn't create file on cache.
				OFSBroadcast::Instance().SendSignal("FileError", "CacheNotWritable", -errno);
				return -errno;
			}
		}
		if (get_availability()) {
			fdr = open(get_remote_path().c_str(), flags, mode);
			if (fdr == -1) {
				close(fdc);
				nRet = -errno;
				// Sends a signal: Couldn't create file on remote share.
				OFSBroadcast::Instance().SendSignal("FileError", "RemoteNotWritable", nRet);
			}
		}
		// Inserts a sync log entry if the file couldn't be created on the remote or if the network is not connected but could be created on the cache.
		if (fdr == -1 || fdr == 0)
		{
			SyncLogger::Instance().AddEntry(OFSEnvironment::Instance().getShareID().c_str(), get_relative_path().c_str(), 'c');
		}
		if (fdr == -1)
		{
			return nRet;
		}
		fd_remote = fdr;
		fd_cache = fdc;
		
		return 0;
	} catch(OFSException &e) {
		errno = e.get_posixerrno();
		return -errno;	
	}
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
int OFSFile::op_fgetattr(struct stat *stbuf)
{
	int res;
	if(get_availability())
		res = fstat(fd_remote, stbuf);
	else
		res = fstat(fd_cache, stbuf);
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
int OFSFile::op_flush()
{
	// TODO: Implement this
}

/**
 * Synchronize file contents
 *
 * If the datasync parameter is non-zero, then only the user data should be
 * flushed, not the meta data.
 * TODO: Implement this!!
 * @param isdatasync 
 * @return 
 */
int OFSFile::op_fsync(int isdatasync)
{
/*	int res;
#ifndef HAVE_FDATASYNC
	(void) isdatasync;
#else
	if (isdatasync)
		res = fdatasync(fd_cache);
	else
#endif
		res = fsync(fd_cache);
	if (res == -1)
		return -errno;
	return 0;*/
	return 0;
}

/**
 * Create a directory
 * @param mode 
 * @return 
 */
int OFSFile::op_mkdir(mode_t mode)
{
	int res, nRet = 0;
	try {
		update_cache();
		
		bool bOK = true;
		if(get_availability()) {
			res = mkdir(get_remote_path().c_str(), mode);
			if (res == -1)
			{
				bOK = false;
				nRet = -errno;
				// Sends a signal: Couldn't create folder on remote share.
				OFSBroadcast::Instance().SendSignal("FileError", "RemoteNotWritable", nRet);
			}
		}
		if (bOK && get_offline_state()) {
			res = mkdir(get_cache_path().c_str(), mode);
			if (res == -1)
			{
				// Sends a signal: Couldn't create folder on cache.
				OFSBroadcast::Instance().SendSignal("FileError", "CacheNotWritable", -errno);
				return -errno;
			}
		}
		// Inserts a sync log entry if the folder couldn't be created on the remote or if the network is not connected but could be created on the cache.
		if (!(get_availability() && bOK))
		{
			SyncLogger::Instance().AddEntry(OFSEnvironment::Instance().getShareID().c_str(), get_relative_path().c_str(), 'c');
		}
		if (bOK)
			update_amtime();
	} catch(OFSException &e) {
		errno = e.get_posixerrno();
		return -errno;
	}
	return nRet;
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
int OFSFile::op_mknod(mode_t mode, dev_t rdev)
{
	int res;
	try {
		bool bOK = true;
		bool bCacheOK = false;
		int nErrNo = 0;
		update_cache();
		string remotepath = get_remote_path();
		string cachepath = get_cache_path();
		if (S_ISFIFO(mode)) {
			if(get_availability()) {
				res = mkfifo(remotepath.c_str(), mode);
				if (res == -1)
				{
					nErrNo = -errno;
					bOK = false;
//					return -errno;
				}
			}
			if(bOK && get_offline_state()) {
				bCacheOK = true;
				res = mkfifo(cachepath.c_str(), mode);
				if(res == -1)
				{
					nErrNo = -errno;
					bCacheOK = false;
//					return -errno;
				}
			}			
		}
		else
		{
			if(get_availability()) {
				res = mknod(remotepath.c_str(), mode, rdev);
				if (res == -1)
				{
					nErrNo = -errno;
					bOK = false;
//					return -errno;
				}
			}
			if(bOK && get_offline_state()) {
				bCacheOK = true;
				res = mknod(cachepath.c_str(), mode, rdev);
				if(res == -1)
				{
					nErrNo = -errno;
					bCacheOK = false;
//					return -errno;
				}
			}
		}
		if (!bOK)
		{
			// Sends a signal: Couldn't create file on remote share.
			OFSBroadcast::Instance().SendSignal("FileError", "RemoteNotWritable", 0);
		}
		// Inserts a sync log entry if the file couldn't be created on the remote or if the network is not connected but could be created on the cache.
		if (!(get_availability() && bOK) && bCacheOK)
		{
			SyncLogger::Instance().AddEntry(OFSEnvironment::Instance().getShareID().c_str(), get_relative_path().c_str(), 'c');
		}
		if (!bOK || !bCacheOK)
		{
			// Sends a signal: Couldn't create file on cache.
			OFSBroadcast::Instance().SendSignal("FileError", "CacheNotWritable", nErrNo);
			return nErrNo;
		}
		update_amtime();
		return 0;
	} catch(OFSException &e) {
		errno = e.get_posixerrno();
		return -errno;
	}
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
int OFSFile::op_open(int flags)
{
	int fdc=0;
	int fdr=0;
	try {
		update_cache();

		if (get_offline_state()) {
			fdc = open(get_cache_path().c_str(), flags);
			if (fdc == -1)
				return -errno;
		}
		if (get_availability()) {
			fdr = open(get_remote_path().c_str(), flags);
			if (fdr == -1) {
				close(fdc);
				return -errno;
			}
		}
		fd_remote = fdr;
		fd_cache = fdc;
		
		return 0;
	} catch(OFSException &e) {
		errno = e.get_posixerrno();
		return -errno;
	}
}

/**
 * Open directory
 *
 * @return 
 */
int OFSFile::op_opendir()
{
	try {
		update_cache();
		if(get_availability()) {
			dh_remote = opendir(get_remote_path().c_str());
			if (dh_remote == NULL)
				return -errno;	
		}
		if(get_offline_state()) {
			dh_cache = opendir(get_cache_path().c_str());
			if (dh_cache == NULL)
			{
				closedir(dh_remote);
				return -errno;	
			}
		}
		update_amtime();
		return 0;
	} catch(OFSException &e) {
		errno = e.get_posixerrno();
		return -errno;
	}
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
int OFSFile::op_read(char *buf, size_t size, off_t offset)
{
	int res=0;
	if(fd_remote && !SynchronizationManager::Instance().has_been_modified(fileinfo))
		res = pread(fd_remote, buf, size, offset);
	else
		res = pread(fd_cache, buf, size, offset);
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
int OFSFile::op_readdir(void *buf, fuse_fill_dir_t filler, off_t offset)
{
	bool cache;
	long loc;
	
	if (dh_remote)
		cache = false;
	else if(dh_cache)
		cache = true;
	else {
		errno = ENOENT;
		return -errno;
	}
	
	struct dirent *de;
	if (cache) {
		seekdir(dh_cache, offset);
		de = readdir(dh_cache);
	} else	{
		seekdir(dh_remote, offset);
		de = readdir(dh_remote);
	}
	while (de != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if(cache)
			loc = telldir(dh_cache);
		else
			loc = telldir(dh_remote);
		if (filler(buf, de->d_name, &st, loc))
			break;
		if (cache)
			de = readdir(dh_cache);
		else
			de = readdir(dh_remote);
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

int OFSFile::op_release()
{
	if(!fd_remote && !fd_cache) {
		errno = EBADF;
		return -errno;
	}
	if(fd_remote)
		if(close(fd_remote) < 0)
			return -errno;
	if(fd_cache)
		if(close(fd_cache) < 0)
			return -errno;

	fd_remote = 0;
	fd_cache = 0;
	update_amtime();
	return 0;
}

/**
 * Release directory
 * @return 
 */
int OFSFile::op_releasedir()
{
	if (!dh_remote && !dh_cache)
	{
		errno = EBADF;
		return -errno;
	}
	if(dh_remote)
		if(closedir(dh_remote))
			return -errno;
	if(dh_cache)
		if(closedir(dh_cache))
			return -errno;
	
	update_amtime();
	return 0;
}

/**
 * Remove the directory
 * @return 
 */
int OFSFile::op_rmdir()
{
	int res, nRet = 0;
	try {
		update_cache();
		bool bOK = true;
		if(get_availability()) {
			res = rmdir(get_remote_path().c_str());
			if (res == -1)
			{
				bOK = false;
				nRet = -errno;
				// Sends a signal: Couldn't delete folder from remote share.
				OFSBroadcast::Instance().SendSignal("FileError", "RemoteNotWritable", nRet);
			}
		}
		if(get_offline_state()) {
			res = rmdir(get_cache_path().c_str());
			if (res == -1)
			{
				// Sends a signal: Couldn't delete folder from cache.
				OFSBroadcast::Instance().SendSignal("FileError", "CacheNotWritable", -errno);
				return -errno;
			}
		}
		// Inserts a sync log entry if the folder couldn't be deleted on the remote or if the network is not connected but could be deleted on the cache.
		if (!(get_availability() && bOK))
		{
			SyncLogger::Instance().AddEntry(OFSEnvironment::Instance().getShareID().c_str(), get_relative_path().c_str(), 'd');
		}
		if (bOK)
			update_amtime();
		return nRet;
	} catch(OFSException &e) {
		errno = e.get_posixerrno();
		return -errno;
	}

}

/**
 * Get file system statistics
 *
 * The 'f_frsize', 'f_favail', 'f_fsid' and 'f_flag' fields are ignored
 * @param stbuf 
 * @return 
 */
int OFSFile::op_statfs(struct statvfs *stbuf)
{
	int res;
	if(get_availability())
		res = statvfs(get_remote_path().c_str(), stbuf);
	else
		res = statvfs(get_cache_path().c_str(), stbuf);
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
int OFSFile::op_truncate(off_t size)
{
	int res;
	try {
		update_cache();
		
		if(get_availability()) {
			res = truncate(get_remote_path().c_str(), size);
			if (res == -1)
				return -errno;
		}
		if(get_offline_state()) {
			res = truncate(get_cache_path().c_str(), size);
			if (res == -1)
				return -errno;
		}
		update_amtime();
		return 0;
	} catch(OFSException &e) {
		errno = e.get_posixerrno();
		return -errno;
	}
}

/**
 * Change the size of a file
 * @param size 
 * @return 
 */
int OFSFile::op_ftruncate(off_t size)
{
	int res;
	
	if (!fd_remote && !fd_cache)
	{
		errno = EBADF;
		return -errno;
	}
	
	if (fd_remote) {
		res = ftruncate(fd_remote, size);
		if (res == -1)
			return -errno;
	}
	if (fd_cache) {
		res = ftruncate(fd_cache, size);
		if (res == -1)
			return -errno;
	}

	return 0;
}

/**
 * Remove the file
 * @return 
 */
int OFSFile::op_unlink()
{
	int res, nRet = 0;
	try {
		update_cache();

		bool bOK = true;
		if(get_availability()) {
			res = unlink(get_remote_path().c_str());
			if (res == -1)
			{
				bOK = false;
				nRet = -errno;
				// Sends a signal: Couldn't delete file from remote share.
				OFSBroadcast::Instance().SendSignal("FileError", "RemoteNotWritable", nRet);
			}
		}
		if(bOK && get_offline_state()) {
			res = unlink(get_cache_path().c_str());
			if (res == -1)
			{
				// Sends a signal: Couldn't delete file from cache.
				OFSBroadcast::Instance().SendSignal("FileError", "CacheNotWritable", -errno);
				return -errno;
			}
		}
		// Inserts a sync log entry if the file couldn't be deleted on the remote or if the network is not connected but could be deleted on the cache.
		if (!(get_availability() && bOK))
		{
			SyncLogger::Instance().AddEntry(OFSEnvironment::Instance().getShareID().c_str(), get_relative_path().c_str(), 'd');
		}
		return nRet;
	} catch(OFSException &e) {
		errno = e.get_posixerrno();
		return -errno;
	}
}





/**
 * Change the access and modification times of
 * a file with nanosecond resolution
 * @param ts[] 
 * @return 
 */
int OFSFile::op_utimens(const struct timespec ts[2])
{
	int res;

	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;
	
	try {
		update_cache();
		if(get_availability()) {
			res = utimes(get_remote_path().c_str(), tv);
			if (res == -1)
				return -errno;
		}
		if(get_offline_state()) {
			res = utimes(get_cache_path().c_str(), tv);
			if (res == -1)
				return -errno;
		}	
		return 0;
	} catch(OFSException &e) {
		errno = e.get_posixerrno();
		return -errno;
	}

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
int OFSFile::op_write(const char *buf, size_t size, off_t offset)
{
	int res;
	int nNumberOfWrittenBytes = -1;
	if(!fd_remote && !fd_cache) {
		errno = EBADF;
		// Sends a signal: Couldn't write file due to missing network connection.
		OFSBroadcast::Instance().SendSignal("FileError", "NeitherRemoteNorCacheAvailable", -EBADF);
		return -errno;
	}
	if(fd_remote) {
		nNumberOfWrittenBytes = res = pwrite(fd_remote, buf, size, offset);
		if (res == -1)
		{
			res = -errno;
			// Sends a signal: Couldn't write file to remote share.
			OFSBroadcast::Instance().SendSignal("FileError", "RemoteNotWritable", res);
		}
	}
	if(fd_cache) {
		res = pwrite(fd_cache, buf, size, offset);
		// Inserts a sync log entry if a file was successfully written to the cache but not or incompletely written to the remote.
		if (nNumberOfWrittenBytes != size - offset &&
			res == size - offset)
			SyncLogger::Instance().AddEntry(OFSEnvironment::Instance().getShareID().c_str(), get_relative_path().c_str(), 'm');
		if (res == -1)
		{
			res = -errno;
			// Sends a signal: Couldn't write file to cache.
			OFSBroadcast::Instance().SendSignal("FileError", "CacheNotWritable", res);
		}
	}
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
int OFSFile::op_symlink(const char* from)
{
	int res;
	
	if(get_availability()) {
		res = symlink(from, get_remote_path().c_str());
		if (res == -1)
			return -errno;
	}
	if(get_offline_state()) {
		res = symlink(from, get_cache_path().c_str());
		if (res == -1)
			return -errno;
	}
	return 0;
}

/**
 * Rename a file
 * @param from 
 * @param to 
 * @return 
 */
int OFSFile::op_rename(OFSFile *to)
{
	int res, nRet = 0;
	try {
		update_cache();
		bool bOK = true;
		if(get_availability()) {
			res = rename(get_remote_path().c_str(), 
				to->get_remote_path().c_str());
			if (res == -1)
			{
				bOK = false;
				nRet = -errno;
				// Sends a signal: Couldn't rename file on remote share.
				OFSBroadcast::Instance().SendSignal("FileError", "RemoteNotWritable", nRet);
			}
		}
		if(get_offline_state()) {
			res = rename(get_cache_path().c_str(), 
				to->get_cache_path().c_str());
			if (res == -1)
			{
				// Sends a signal: Couldn't rename file on cache.
				OFSBroadcast::Instance().SendSignal("FileError", "CacheNotWritable", -errno);
				return -errno;
			}
		}
		// Inserts a sync log entry if the link couldn't be renamed on the remote or if the network is not connected but could be renamed on the cache.
		if (!(get_availability() && bOK))
		{
			// Creates the new file.
			SyncLogger::Instance().AddEntry(OFSEnvironment::Instance().getShareID().c_str(), to->get_relative_path().c_str(), 'c');
			// Copies the file content.
			SyncLogger::Instance().AddEntry(OFSEnvironment::Instance().getShareID().c_str(), to->get_relative_path().c_str(), 'm');
			// Deletes the old file.
			SyncLogger::Instance().AddEntry(OFSEnvironment::Instance().getShareID().c_str(), get_relative_path().c_str(), 'd');
		}
		if (bOK)
			update_amtime();

		return nRet;
	} catch(OFSException &e) {
		errno = e.get_posixerrno();
		return -errno;
	}
}


/**
 * Create a hard link to a file
 * @param from 
 * @param to 
 * @return 
 */
int OFSFile::op_link(OFSFile *to)
{
	int res, nRet = 0;
	try {
		update_cache();
		
		bool bOK = true;
		if(get_availability()) {
			res = link(get_remote_path().c_str(),
				to->get_remote_path().c_str());
			if(res == -1)
			{
				bOK = false;
				nRet = -errno;
				// Sends a signal: Couldn't create link on remote share.
				OFSBroadcast::Instance().SendSignal("FileError", "RemoteNotWritable", nRet);
			}
		}
		if(bOK && get_offline_state()) {
			res = link(get_cache_path().c_str(),
				to->get_cache_path().c_str());
			if(res == -1)
			{
				// Sends a signal: Couldn't create link on cache.
				OFSBroadcast::Instance().SendSignal("FileError", "CacheNotWritable", -errno);
				return -errno;
			}
		}
		// Inserts a sync log entry if the link couldn't be created on the remote or if the network is not connected but could be created on the cache.
		if (!(get_availability() && bOK))
		{
			SyncLogger::Instance().AddEntry(OFSEnvironment::Instance().getShareID().c_str(), get_relative_path().c_str(), 'c');
		}
		if (bOK)
			update_amtime();

		return nRet;
	} catch(OFSException &e) {
		errno = e.get_posixerrno();
		return -errno;
	}
}


/*!
 *  If this file is available on the remote machine
 *  and has changed, update it
 *  TODO: attributes (ctime, atime etc.) have to be set
 *        on the cache file and all directories in path
 *  \fn OFSFile::update_local()
 */
void OFSFile::update_cache()
{
	struct stat fileinfo_cache;
	struct stat fileinfo_remote;
	bool file_exists = true;
	int ret;

	// only if the file is marked as offline and the remote
	// file is available we do something
	if (get_offline_state() && get_availability()) {
		// get info of remote file
		ret = lstat(get_remote_path().c_str(), &fileinfo_remote);
		if (ret < 0 && errno == ENOENT) {
			errno = 0;
			// if the remote file does not exist, we only make sure,
			// the parent directory is current
			OFSFile *parent = get_parent_directory();
			if (parent)
				parent->update_cache();
			delete parent;
			return;
		}
		else if (ret < 0)
			throw OFSException(strerror(errno), errno);

		// receive file information
		ret = lstat(get_cache_path().c_str(), &fileinfo_cache);
		if (ret < 0 && errno == ENOENT)
		{
			errno = 0;
			// make sure the parent directory is current
			OFSFile *parent = get_parent_directory();
			if(parent)
				parent->update_cache();
			delete parent;
			file_exists = false;
		}
		else if (ret < 0 )
			throw OFSException(strerror(errno), errno);
		
		// if the remote file is not in cache or has changed 
		// we have to copy it to the cache
		// TODO: If the file gets openend for overwriting, we may skip copying it from
		// the remote location
		if (!file_exists || fileinfo_remote.st_mtime > fileinfo_cache.st_mtime) {
			// if this is a directory, we only create it in the cache if necessary
			if(S_ISDIR(fileinfo_remote.st_mode) && !file_exists) {
				if(mkdir(get_cache_path().c_str(),S_IRWXU) < 0)
					throw OFSException(strerror(errno), errno);
			} else if (S_ISREG(fileinfo_remote.st_mode)) {
				int fdl = open(get_cache_path().c_str(),
					O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
				if (fdl < 0)
					throw OFSException(strerror(errno), errno);
				int fdr = open(get_remote_path().c_str(), O_RDONLY);
				if (fdr < 0)
					throw OFSException(strerror(errno), errno);
				char buf[1024];
				ssize_t bytesread;
				while((bytesread = read(fdr, buf, sizeof(buf))) > 0)
				{
					if (write(fdl, buf, bytesread) < 0)
						throw OFSException(strerror(errno), errno);
				}
				if(bytesread < 0)
					throw new OFSException(strerror(errno), errno);
				close(fdr);
				close(fdl);
			} else if (S_ISLNK(fileinfo_remote.st_mode)) {
				char buf[1024];
				ssize_t len;
				// remove the old link if it exists
				unlink(get_cache_path().c_str());
				errno = 0;
				// create the new link
				len = readlink(get_remote_path().c_str(), buf, sizeof(buf)-1);
				if (len < 0)
					throw OFSException(strerror(errno), errno);
				buf[len] = '\0';
				if (symlink(buf, get_cache_path().c_str()) < 0)
					throw OFSException(strerror(errno), errno);
			} // TODO: Other file types 

			// set atime and mtime
			//update_amtime();
		}
	}
}


/*!
    \fn OFSFile::get_parent_directory()
	Get the File object for the parent directory
 */
OFSFile * OFSFile::get_parent_directory()
{
	size_t slashpos;
	// find the last '/' - start searching on the second-last position
	// because we have to ignore the '/' at the end if there is one
	slashpos = get_relative_path().find_last_of('/',get_relative_path().length()-2);
	if (slashpos == string::npos)
		return NULL;
	return new OFSFile(get_relative_path().substr(0, slashpos));
	
}


/*!
    \fn OFSFile::update_amtime()
	Update the access and modify times of the cache file
	by using the info from the remote file (if possible)
 */
void OFSFile::update_amtime()
{
	if (get_offline_state() && get_availability()) {
		struct stat fileinfo_remote;
		struct utimbuf times;
	
		if(lstat(get_remote_path().c_str(), &fileinfo_remote) < 0)
			throw OFSException(strerror(errno), errno);
		
		// utime can not be used with symbolic links because there
		// is no possibility to prevent it from following the link
		if(!S_ISLNK(fileinfo_remote.st_mode)) {
			times.actime = fileinfo_remote.st_atime;
			times.modtime = fileinfo_remote.st_mtime;
			if(utime(get_cache_path().c_str(), &times) < 0)
				throw OFSException(strerror(errno), errno);
		}
	}
}




/*!
    \fn OFSFile::op_getxattr(const char *name, char *value,
size_t size)
 */
int OFSFile::op_getxattr(const char *name, char *value,
size_t size)
{
	int res = 0;
	// offline attribute
	if (strncmp(name, OFS_ATTRIBUTE_OFFLINE,
			strlen(OFS_ATTRIBUTE_OFFLINE)+1) == 0) {
		if (get_offline_state()) {
			res = strlen(OFS_ATTRIBUTE_VALUE_YES);
			if (size >= res) {
				strncpy(value, OFS_ATTRIBUTE_VALUE_YES,
					strlen(OFS_ATTRIBUTE_VALUE_YES) );
			}
		} else {
			res = strlen(OFS_ATTRIBUTE_VALUE_NO);
			if (size >= res) {
				strncpy(value, OFS_ATTRIBUTE_VALUE_NO,
					strlen(OFS_ATTRIBUTE_VALUE_NO) );
			}
		}
	// availability attribute
	} else if(strncmp(name, OFS_ATTRIBUTE_AVAILABLE,
			strlen(OFS_ATTRIBUTE_AVAILABLE + 1)) == 0 ) {
		if (get_availability()) {
			res = strlen(OFS_ATTRIBUTE_VALUE_YES);
			if (size >= res) {
				strncpy(value, OFS_ATTRIBUTE_VALUE_YES,
					strlen(OFS_ATTRIBUTE_VALUE_YES) );
			}
		} else {
			res = strlen(OFS_ATTRIBUTE_VALUE_NO);
			if (size >= res) {
				strncpy(value, OFS_ATTRIBUTE_VALUE_NO,
					strlen(OFS_ATTRIBUTE_VALUE_NO) );
			}
		}
 	// unknown attribute - delegate to the underlying filesystem
	} else { // TODO: By now this is only for remote files
		res = lgetxattr(get_remote_path().c_str(),
			name, value, size);
		// do not return "unsupported" but "unknown attribute"
		if(errno == ENOTSUP)
			errno = ENOATTR;
	} 
	if (res == -1)
		return -errno;
	return res;
}


/*!
    \fn OFSFile::op_setxattr(const char *value, size_t size, int flags)
 */
int OFSFile::op_setxattr(const char *name, const char *value, size_t size, int flags)
{
	int res = 0;
	// offline attribute
	if (strncmp(name, OFS_ATTRIBUTE_OFFLINE,
			strlen(OFS_ATTRIBUTE_OFFLINE)+1) == 0) {
		BackingtreeManager::Instance().register_Backingtree(get_relative_path());
	// availability attribute
	} else if(strncmp(name, OFS_ATTRIBUTE_AVAILABLE,
			strlen(OFS_ATTRIBUTE_AVAILABLE + 1)) == 0 ) {
		// readonly -> error
		res = -1;
		errno = EACCES;
	} else { // other attribute - delegate to underlying filesystem
		res = lsetxattr(get_remote_path().c_str(), name,
			value, size, flags);
	}
	if (res == -1)
		return -errno;
	return 0;
}


/*!
    \fn OFSFile::op_listxattr(char *list, size_t size)
 */
int OFSFile::op_listxattr(char *list, size_t size)
{
	int res = 0;
	int fsres = 0;
	char *fslist = NULL;
	
	res += strlen(OFS_ATTRIBUTE_OFFLINE) + 1;
	res += strlen(OFS_ATTRIBUTE_AVAILABLE) + 1;
	if (size > 0) { // copy available attributes into the buffer
		strncpy(list, OFS_ATTRIBUTE_OFFLINE,
			strlen(OFS_ATTRIBUTE_OFFLINE)+1);
		strncpy(list+strlen(OFS_ATTRIBUTE_OFFLINE)+1,
			OFS_ATTRIBUTE_AVAILABLE,
			strlen(OFS_ATTRIBUTE_AVAILABLE)+1);
		fslist = new char[size-res];		
		fsres = llistxattr(get_remote_path().c_str(),
			fslist, size-res);
		if (fsres > 0)
			res += fsres;
	} else { // no buffer - only calculate length
		fsres = llistxattr(get_remote_path().c_str(), fslist, 0);
		if (fsres > 0)
			res += fsres;		
	}
	return res;
}


/*!
    \fn OFSFile::op_removexattr(const char *name)
	TODO: Use local file is remote file is not available
	      no dot allow changing cache status when offline
 */
int OFSFile::op_removexattr(const char *name)
{
	int res = 0;
	// offline attribute
	if (strncmp(name, OFS_ATTRIBUTE_OFFLINE,
			strlen(OFS_ATTRIBUTE_OFFLINE)+1) == 0) {
		BackingtreeManager::Instance().remove_Backingtree(get_relative_path());
	// availability attribute
	} else if(strncmp(name, OFS_ATTRIBUTE_AVAILABLE,
			strlen(OFS_ATTRIBUTE_AVAILABLE + 1)) == 0 ) {
		// readonly -> error
		res = -1;
		errno = EACCES;
	} else {
		res = lremovexattr(get_remote_path().c_str(), name);
	}
	if (res == -1)
		return -errno;
	return 0;
}
