/*
 * loggerfs: a virtual file system to store logs in a database
 * Copyright (C) 2007 John C. Matherly
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#ifndef FUSEXX_H_
#define FUSEXX_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define FUSE_USE_VERSION 26 // earlier versions have deprecated functions

// C++ Headers
#include <string> // memset

// C Headers
#include <fuse.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ofslog.h"

namespace fusexx {
    /*
     * fuse
     * Manages all the fuse operations. A very simple interface to the C fuse_ops struct.
     */
    template <class T>
	class fuse {
		/*
		 * Public Methods
		 */
		public:
			/*
			 * Main function of fusexx::fuse.
			 * 
			 * Calls the fuse 'fuse_main' macro.
			 * 
			 * @param argc the argument counter passed to the main() function
 			 * @param argv the argument vector passed to the main() function
 			 * @param user_data user data set in context for init() method
 			 * @return 0 on success, nonzero on failure
 			 */
			static int main (int argc, char **argv, void *user_data, T *t) {
				// Zero the operations struct
				memset (&T::operations, 0, sizeof (struct fuse_operations));
				
				// Load the operations struct w/ pointers to the respective member functions
				T::loadOperations ();
				
				// The 'self' variable will be the equivalent of the 'this' pointer
				if (t == NULL)
					return -1;
				T::self = t;
				

				// Execute fuse_main
				ofslog::info("Starting FUSE");
				return fuse_main (argc, argv, &T::operations, user_data);
			}
			
    		static struct fuse_operations operations;
			
		/*
		 * Overload these functions
		 */
		 public:
			static int fuse_readlink (const char *, char *, size_t) { return 0; }
		    static int fuse_getattr (const char *, struct stat * ) { return 0; }
		    static int fuse_getdir (const char *, fuse_dirh_t, fuse_dirfil_t) { return 0; }
		    static int fuse_mknod (const char *, mode_t, dev_t) { return 0; }
		    static int fuse_mkdir (const char *, mode_t) { return 0; }
		    static int fuse_unlink (const char *) { return 0; }
		    static int fuse_rmdir (const char *) { return 0; }
		    static int fuse_symlink (const char *, const char *) { return 0; }
		    static int fuse_rename (const char *, const char *) { return 0; }
		    static int fuse_link (const char *, const char *) { return 0; }
		    static int fuse_chmod (const char *, mode_t) { return 0; }
		    static int fuse_chown (const char *, uid_t, gid_t) { return 0; }
		    static int fuse_truncate (const char *, off_t) { return 0; }
		    static int fuse_utime (const char *, struct utimbuf *) { return 0; }
		    static int fuse_utimens(const char *path, const struct timespec ts[2]) { return 0; }
		    static int fuse_open (const char *, struct fuse_file_info *) { return 0; }
		    static int fuse_read (const char *, char *, size_t, off_t, struct fuse_file_info *) { return 0; }
		    static int fuse_write (const char *, const char *, size_t, off_t,struct fuse_file_info *) { return 0; }
		    static int fuse_statfs (const char *, struct statvfs *) { return 0; }
		    static int fuse_flush (const char *, struct fuse_file_info *) { return 0; }
		    static int fuse_release (const char *, struct fuse_file_info *) { return 0; }
		    static int fuse_fsync (const char *, int, struct fuse_file_info *) { return 0; }
#ifdef FUSE_XATTR_ADD_OPT
                    static int fuse_setxattr (const char *, const char *, const char *, size_t, int, uint32_t) {return 0; }
#else
                    static int fuse_setxattr (const char *, const char *, const char *, size_t, int) { return 0; }
#endif /* __FreeBSD__ >= 10 */
#ifdef FUSE_XATTR_ADD_OPT
                    static int fuse_getxattr (const char *, const char *, char *, size_t, uint32_t) {return 0;}
#else
		    static int fuse_getxattr (const char *, const char *, char *, size_t) { return 0; }
#endif /* __FreeBSD__ >= 10 */
		    static int fuse_listxattr (const char *, char *, size_t) { return 0; }
		    static int fuse_removexattr (const char *, const char *) { return 0; }
		    static int fuse_readdir (const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *) { return 0; }
		    static int fuse_opendir (const char *, struct fuse_file_info *) { return 0; }
		    static int fuse_releasedir (const char *, struct fuse_file_info *) { return 0; }
		    static int fuse_fsyncdir (const char *, int, struct fuse_file_info *) { return 0; }
		    static void *fuse_init (struct fuse_conn_info *conn) { return NULL; }
		    static void  fuse_destroy (void *) { }
		    static int fuse_access (const char *, int) { return 0; }
		    static int fuse_create (const char *, mode_t, struct fuse_file_info *) { return 0; }
		    static int fuse_ftruncate (const char *, off_t, struct fuse_file_info *) { return 0; }
		    static int fuse_fgetattr (const char *, struct stat *, struct fuse_file_info *) { return 0; }
		    		    
		    static void loadOperations () {
				operations.readlink = T::fuse_readlink;
			    operations.getattr = T::fuse_getattr;
			    operations.getdir = T::fuse_getdir;
			    operations.mknod = T::fuse_mknod;
			    operations.mkdir = T::fuse_mkdir;
			    operations.unlink = T::fuse_unlink;
			    operations.rmdir = T::fuse_rmdir;
			    operations.symlink = T::fuse_symlink;
			    operations.rename = T::fuse_rename;
			    operations.link = T::fuse_link;
			    operations.chmod = T::fuse_chmod;
			    operations.chown = T::fuse_chown;
			    operations.truncate = T::fuse_truncate;
			    operations.utime = 0; // utime is deprecated
			    operations.utimens = T::fuse_utimens;
			    operations.open = T::fuse_open;
			    operations.read = T::fuse_read;
			    operations.write = T::fuse_write;
			    operations.statfs = T::fuse_statfs;
			    operations.flush = T::fuse_flush;
			    operations.release = T::fuse_release;
			    operations.fsync = T::fuse_fsync;
			    operations.setxattr = T::fuse_setxattr;
			    operations.getxattr = T::fuse_getxattr;
			    operations.listxattr = T::fuse_listxattr;
			    operations.removexattr = T::fuse_removexattr;
			    operations.readdir = T::fuse_readdir;
			    operations.opendir = T::fuse_opendir;
			    operations.releasedir = T::fuse_releasedir;
			    operations.fsyncdir = T::fuse_fsyncdir;
			    operations.init = T::fuse_init;
			    operations.destroy = T::fuse_destroy;
			    operations.access = T::fuse_access;
			    operations.create = T::fuse_create;
			    operations.ftruncate = T::fuse_ftruncate;
			    operations.fgetattr = T::fuse_fgetattr;
			}
			
			/*
			 * Protected variables
			 */
		protected:
			// allow static methods to access object methods/ variables using 'self' instead of 'this'
			static T *self;
	};
	
	template <class T> struct fuse_operations fuse<T> ::operations;
	template <class T> T * fuse<T> ::self;
}

#endif /*FUSEXX_H_*/
