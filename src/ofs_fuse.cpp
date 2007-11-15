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
#include "ofs_fuse.h"

#include <string>
#include <pthread.h>
#include "ofs_daemon.h"
#include <errno.h>
#include <stdio.h>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/file.h>
#include <ulockmgr.h>
#include <sys/time.h>

#include "filestatusmanager.h"

using namespace std;
//ofs_fuse::fuse_ofs_fuse()
// : fusexx::fuse<ofs_fuse> ()
//{
//}


ofs_fuse::~ofs_fuse()
{
}

ofs_fuse::ofs_fuse () :
                m_strHelloWorld ("Hello World!\n"),
                m_strPath ("/hello"), cntnr (0),
		basepath ("/usr") { 
        // all we're doing is initialize the member variables
}


/**
 * Get file attributes.
 *
 * Similar to stat(). The 'st_dev' and 'st_blksize' fields are ignored.
 * The 'st_ino' field is ignored except if the 'use_ino' mount option is given.
 * @param path 
 * @param stbuf 
 * @return 
 */
int ofs_fuse::fuse_getattr(const char *path, struct stat *stbuf)
{
	int res;
	File file = Filestatusmanager::Instance().give_me_file(path);
	
	res = lstat(file.get_remote_path().c_str(), stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * Get attributes from an open file
 *
 * This method is called instead of the getattr() method if the file 
 * information is available.
 * Currently this is only called after the create() method if that
 * is implemented (see above). Later it may be called for invocations of fstat() too.
 * @param path 
 * @param stbuf 
 * @param fi 
 * @return 
 */
int ofs_fuse::fuse_fgetattr(const char *path, struct stat *stbuf,
                        struct fuse_file_info *fi)
{
	int res;
	(void) path;

	openfile_info *fileinfo = (openfile_info *)fi->fh;
	if (!fileinfo)
	{
		errno = EBADF;
		return -errno;
	}
	res = fstat(fileinfo->get_fd_remote(), stbuf);
	if (res == -1)
		return -errno;
	return 0;
}

/**
 * Check file access permissions
 *
 * This will be called for the access() system call.
 * If the 'default_permissions' mount option is given,
 * this method is not called.
 *
 * This method is not called under Linux kernel versions 2.4.x
 * @param path 
 * @param mask 
 * @return 
 */
int ofs_fuse::fuse_access(const char *path, int mask)
{
	int res;
	File file = Filestatusmanager::Instance().give_me_file(path);

	res = access(file.get_remote_path().c_str(), mask);
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
 * @param path 
 * @param buf 
 * @param size 
 * @return 
 */
int ofs_fuse::fuse_readlink(const char *path, char *buf, size_t size)
{
	int res;
	File file = Filestatusmanager::Instance().give_me_file(path);

	res = readlink(file.get_remote_path().c_str(), buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}


/**
 * Open directory
 *
 * This method should check if the open operation is
 * permitted for this directory
 * @param path 
 * @param fi 
 * @return 
 */
int ofs_fuse::fuse_opendir(const char *path, struct fuse_file_info *fi)
{
	File file = Filestatusmanager::Instance().give_me_file(path);

	DIR *dp = opendir(file.get_remote_path().c_str());
	if (dp == NULL)
		return -errno;
	
	opendir_info *dirinfo = new opendir_info(dp, NULL, file);
	fi->fh = (unsigned long)dirinfo;
	
	return 0;
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
 * @param path 
 * @param buf 
 * @param filler 
 * @param offset 
 * @param fi 
 * @return 
 */
int ofs_fuse::fuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
	(void) path;
	opendir_info *dirinfo = (opendir_info *)fi->fh;
	if (!dirinfo)
	{
		errno = EBADF;
		return -errno;
	}

	DIR *dp = dirinfo->get_dirptr_remote();
	struct dirent *de;

	seekdir(dp, offset);
	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, telldir(dp)))
			break;
	}

	return 0;
}

/**
 * Release directory
 * @param path 
 * @param fi 
 * @return 
 */
int ofs_fuse::fuse_releasedir(const char *path, struct fuse_file_info *fi)
{
	(void) path;
	opendir_info *dirinfo = (opendir_info *)fi->fh;
	if (!dirinfo)
	{
		errno = EBADF;
		return -errno;
	}
	DIR *dp = dirinfo->get_dirptr_remote();

	closedir(dp);
	delete dirinfo;
	fi->fh = 0;

	return 0;
}

/**
 * Create a file node
 *
 * This is called for creation of all non-directory, non-symlink nodes.
 * If the filesystem defines a create() method, then for regular files
 * that will be called instead. 
 * @param path 
 * @param mode 
 * @param rdev 
 * @return 
 */
int ofs_fuse::fuse_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;
	File file = Filestatusmanager::Instance().give_me_file(path);

	string remotepath = file.get_remote_path();
	if (S_ISFIFO(mode))
		res = mkfifo(remotepath.c_str(), mode);
	else
		res = mknod(remotepath.c_str(), mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * Create a directory
 * @param path 
 * @param mode 
 * @return 
 */
int ofs_fuse::fuse_mkdir(const char *path, mode_t mode)
{
	int res;
	File file = Filestatusmanager::Instance().give_me_file(path);

	res = mkdir(file.get_remote_path().c_str(), mode);
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * Remove a file
 * @param path 
 * @return 
 */
int ofs_fuse::fuse_unlink(const char *path)
{
	int res;
	File file = Filestatusmanager::Instance().give_me_file(path);

	res = unlink(file.get_remote_path().c_str());
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * Remove a directory
 * @param path 
 * @return 
 */
int ofs_fuse::fuse_rmdir(const char *path)
{
	int res;
	File file = Filestatusmanager::Instance().give_me_file(path);

	res = rmdir(file.get_remote_path().c_str());
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * Create a symbolic link
 * TODO: I think 'from' is the exact path as specified in the command line.
 *       The 'to' parameter is relative to the current filesystem
 *       Check this!
 * @param from 
 * @param to 
 * @return 
 */
int ofs_fuse::fuse_symlink(const char *from, const char *to)
{
	int res;
	File file_to = Filestatusmanager::Instance().give_me_file(to);

	res = symlink(from, file_to.get_remote_path().c_str());
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
int ofs_fuse::fuse_rename(const char *from, const char *to)
{
	int res;
	File file_from = Filestatusmanager::Instance().give_me_file(from);
	File file_to = Filestatusmanager::Instance().give_me_file(to);

	res = rename(file_from.get_remote_path().c_str(), file_to.get_remote_path().c_str());
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
int ofs_fuse::fuse_link(const char *from, const char *to)
{
	int res;
	File file_from = Filestatusmanager::Instance().give_me_file(from);
	File file_to = Filestatusmanager::Instance().give_me_file(to);

	res = link(file_from.get_remote_path().c_str(), file_to.get_remote_path().c_str());
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * Change the permission bits of a file
 * @param path 
 * @param mode 
 * @return 
 */
int ofs_fuse::fuse_chmod(const char *path, mode_t mode)
{
	int res;
	File file = Filestatusmanager::Instance().give_me_file(path);

	res = chmod(file.get_remote_path().c_str(), mode);
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * Change the owner and group of a file
 * @param path 
 * @param uid 
 * @param gid 
 * @return 
 */
int ofs_fuse::fuse_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
	File file = Filestatusmanager::Instance().give_me_file(path);

	res = lchown(file.get_remote_path().c_str(), uid, gid);
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
int ofs_fuse::fuse_truncate(const char *path, off_t size)
{
	int res;
	File file = Filestatusmanager::Instance().give_me_file(path);

	res = truncate(file.get_remote_path().c_str(), size);
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * Change the size of an open file
 *
 * This method is called instead of the truncate() method if the truncation
 * was invoked from an ftruncate() system call.
 *
 * If this method is not implemented or under Linux kernel versions earlier
 * than 2.6.15, the truncate() method will be called instead.
 * @param path 
 * @param size 
 * @param fi 
 * @return 
 */
int ofs_fuse::fuse_ftruncate(const char *path, off_t size,
                         struct fuse_file_info *fi)
{
	(void) path;
	int res;

	openfile_info *fileinfo = (openfile_info *)fi->fh;
	if (!fileinfo)
	{
		errno = EBADF;
		return -errno;
	}

	res = ftruncate(fileinfo->get_fd_remote(), size);
	if (res == -1)
		return -errno;

	return 0;
}

/**
 * Change the access and modification times of
 * a file with nanosecond resolution
 * @param path 
 * @param ts[] 
 * @return 
 */
int ofs_fuse::fuse_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	File file = Filestatusmanager::Instance().give_me_file(path);

	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(file.get_remote_path().c_str(), tv);
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
 * @param path 
 * @param mode 
 * @param fi 
 * @return 
 */
int ofs_fuse::fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	int fd;
	File file = Filestatusmanager::Instance().give_me_file(path);

	fd = open(file.get_remote_path().c_str(), fi->flags, mode);
	if (fd == -1)
		return -errno;
	
	openfile_info *fileinfo = new openfile_info(fd, -1, file);
	fi->fh = (unsigned long)fileinfo;
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
 * @param path 
 * @param fi 
 * @return 
 */
int ofs_fuse::fuse_open(const char *path, struct fuse_file_info *fi)
{
	int fd;
	File file = Filestatusmanager::Instance().give_me_file(path);

	fd = open(file.get_remote_path().c_str(), fi->flags);
	if (fd == -1)
		return -errno;

	openfile_info *fileinfo = new openfile_info(fd, -1, file);
	fi->fh = (unsigned long)fileinfo;
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
 * @param path 
 * @param buf 
 * @param size 
 * @param offset 
 * @param fi 
 * @return 
 */
int ofs_fuse::fuse_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
	int res;
	(void) path;
	openfile_info *fileinfo = (openfile_info *)fi->fh;
	if (!fileinfo)
	{
		errno = EBADF;
		return -errno;
	}
	
	res = pread(fileinfo->get_fd_remote(), buf, size, offset);
	if (res == -1)
		res = -errno;

	return res;
}

/**
 * Write data to an open file
 *
 * Write should return exactly the number of bytes requested except on 
 * error. An exception to this is when the 'direct_io' mount option is
 * specified (see read operation).
 * @param path 
 * @param buf 
 * @param size 
 * @param offset 
 * @param fi 
 * @return 
 */
int ofs_fuse::fuse_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi)
{
	int res;
	(void) path;
	openfile_info *fileinfo = (openfile_info *)fi->fh;
	if (!fileinfo)
	{
		errno = EBADF;
		return -errno;
	}

	res = pwrite(fileinfo->get_fd_remote(), buf, size, offset);
	if (res == -1)
		res = -errno;

	return res;
}

/**
 * Get file system statistics
 *
 * The 'f_frsize', 'f_favail', 'f_fsid' and 'f_flag' fields are ignored
 * @param path 
 * @param stbuf 
 * @return 
 */
int ofs_fuse::fuse_statfs(const char *path, struct statvfs *stbuf)
{
	int res;
	File file = Filestatusmanager::Instance().give_me_file(path);

	res = statvfs(file.get_remote_path().c_str(), stbuf);
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
 * @param path 
 * @param fi 
 * @return 
 */
int ofs_fuse::fuse_flush(const char *path, struct fuse_file_info *fi)
{
	int res;
	(void) path;
	openfile_info *fileinfo = (openfile_info *)fi->fh;
	/* This is called from every close on an open file, so call the
	close on the underlying filesystem.  But since flush may be
	called multiple times for an open file, this must not really
	close the file.  This is important if used on a network
	filesystem like NFS which flush the data/metadata on close() */
// 	if (!fileinfo)
// 	{
// 		errno = EBADF;
// 		return -errno;
// 	}
// 	res = close(dup(fileinfo->get_fd_remote()));
// 	if (res == -1)
// 		return -errno;

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
 * @param path 
 * @param fi 
 * @return 
 */
int ofs_fuse::fuse_release(const char *path, struct fuse_file_info *fi)
{
	int res;
	(void) path;
	openfile_info *fileinfo = (openfile_info *)fi->fh;
	if (!fileinfo)
	{
		errno = EBADF;
		return -errno;
	}

	if(close(fileinfo->get_fd_remote()))
		return -errno;
	delete fileinfo;
	fi->fh = 0;
	
	return 0;
}

/**
 * Synchronize file contents
 *
 * If the datasync parameter is non-zero, then only the user data should be
 * flushed, not the meta data.
 * @param path 
 * @param isdatasync 
 * @param fi 
 * @return 
 */
int ofs_fuse::fuse_fsync(const char *path, int isdatasync,
                     struct fuse_file_info *fi)
{
	int res;
	(void) path;
	openfile_info *fileinfo = (openfile_info *)fi->fh;
	if (!fileinfo)
	{
		errno = EBADF;
		return -errno;
	}

#ifndef HAVE_FDATASYNC
	(void) isdatasync;
#else
	if (isdatasync)
		res = fdatasync(fileinfo->get_fd_remote());
	else
#endif
		res = fsync(fileinfo->get_fd_remote());
	if (res == -1)
		return -errno;

	return 0;
}

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented 
  TODO: This has to be implemented in order to set the 'offline' flag*/
/*int ofs_fuse::fuse_setxattr(const char *path, const char *name, const char *value,
                        size_t size, int flags)
{
    int res = lsetxattr(path, name, value, size, flags);
    if (res == -1)
        return -errno;
    return 0;
}

int ofs_fuse::fuse_getxattr(const char *path, const char *name, char *value,
                    size_t size)
{
    int res = lgetxattr(path, name, value, size);
    if (res == -1)
        return -errno;
    return res;
}

int ofs_fuse::fuse_listxattr(const char *path, char *list, size_t size)
{
    int res = llistxattr(path, list, size);
    if (res == -1)
        return -errno;
    return res;
}

int ofs_fuse::fuse_removexattr(const char *path, const char *name)
{
    int res = lremovexattr(path, name);
    if (res == -1)
        return -errno;
    return 0;
}*/
#endif /* HAVE_SETXATTR */

/**
 * Perform POSIX file locking operation
 *
 * The cmd argument will be either F_GETLK, F_SETLK or F_SETLKW.
 *
 * For the meaning of fields in 'struct flock' see the man page for fcntl(2).
 * The l_whence field will always be set to SEEK_SET.
 *
 * For checking lock ownership, the 'fuse_file_info->owner'
 * argument must be used.
 *
 * For F_GETLK operation, the library will first check currently held locks,
 * and if a conflicting lock is found it will return information without
 * calling this method. This ensures, that for local locks the l_pid field is
 * correctly filled in. The results may not be accurate in case of race
 * conditions and in the presence of hard links, but it's unlikly that an
 * application would rely on accurate GETLK results in these cases. If a
 * conflicting lock is not found, this method will be called, and the
 * filesystem may fill out l_pid by a meaningful value, or it may leave this
 * field zero.
 *
 * For F_SETLK and F_SETLKW the l_pid field will be set to the pid of the
 * process performing the locking operation.
 *
 * Note: if this method is not implemented, the kernel will still allow file
 * locking to work locally. Hence it is only interesting for network
 * filesystems and similar.
 * TODO: Implement this
 * @param path 
 * @param fi 
 * @param cmd 
 * @param lock 
 * @return 
 */
// int ofs_fuse::fuse_lock(const char *path, struct fuse_file_info *fi, int cmd,
//                     struct flock *lock)
// {
// 	(void) path;
// 	openfile_info *fileinfo = (openfile_info *)fi->fh;
// 	if (!fileinfo)
// 	{
// 		errno = EBADF;
// 		return -errno;
// 	}
// 	return ulockmgr_op(fileinfo->get_fd_remote(), cmd, lock, &fi->lock_owner,
//                        sizeof(fi->lock_owner));
// 	return 0;
// }

/**
 * Initialize filesystem
 *
 * The return value will passed in the private_data field of fuse_context
 * to all file operations and as a parameter to the destroy() method.
 * @param conn 
 * @return 
 */
void *ofs_fuse::fuse_init (struct fuse_conn_info *conn) { 
/*	pthread_t *thread = new pthread_t();
	if (!pthread_create(thread, NULL, ofs_daemon::start_daemon, (void *)self))
		perror(strerror(errno));*/
	return NULL;
}

/*!
    \fn ofs_fuse::fuse_self->get_absolute_path(string rel_path)
 */
string ofs_fuse::get_absolute_path(string rel_path)
{
	return basepath + rel_path;
}
