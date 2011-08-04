/***************************************************************************
 *   Copyright (C) 2007, 2011 by Carsten Kolassa, Peter Trommler           *
 *   Carsten@Kolassa.de                                                    *
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "ofs_fuse.h"
#include "backingtreepersistence.h"
#include "filesystemstatusmanager.h"
#include "ofsconf.h"
#include "ofsenvironment.h"
#include "ofshash.h"
#include "ofslog.h"
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

/**
 * @TODO: This is very unclean
 * @param argc
 * @param argv[]
 * @return
 */
int main(int argc, char *argv[])
{
	if(! ofslog::init() ){
		std::cerr << argv[0] << ": Failed to init log system" << endl;
		return 2; // system error return code, see mount(8)
	}

	try {
		OFSEnvironment::init(argc, argv);
	} catch (OFSException& e) {
		cerr << e.what() << endl;
		cerr << OFSEnvironment::getUsageString();
		return 1; // see mount(8) for return codes
	}

	// FIXME: Do not terminate if remote file system cannot be mounted because we are offline
	try {
		FilesystemStatusManager::Instance().mountfs();
	} catch (OFSException& e) {
		cerr << e.what() << endl;
		return 1;
	}

	ofs_fuse my_ofs;
	ofslog::info("Starting ofs daemon");
	ofslog::debug("Adopting parameters");
	OFSEnvironment &env = OFSEnvironment::Instance();
	char *fuse_arguments[5];
	int numargs;
	// TODO: Check out fuse option handling function
	// TODO: Pass on rw or ro option?
	// TODO: check if we really need to copy here, if yes use strdup(3)
	fuse_arguments[0] = new char[env.getBinaryPath().length()+1];
	strncpy(fuse_arguments[0], env.getBinaryPath().c_str(),
		env.getBinaryPath().length()+1);
	fuse_arguments[1] = new char[env.getMountPoint().length()+1];
	strncpy(fuse_arguments[1], env.getMountPoint().c_str(),
		env.getMountPoint().length()+1);
#if 0
	if(env.isAllowOther()) {
		fuse_arguments[2] = new char[3];
		strncpy(fuse_arguments[2], "-o", 3);
		fuse_arguments[3] = new char[12];
		strncpy(fuse_arguments[3], "allow_other", 12);
		fuse_arguments[4] = NULL;
		numargs = 4;
	} else {
#endif /* 0 */
// FIXME: How do we start fuse properly so we don't allow_other?
		fuse_arguments[2] = const_cast<char*>("-o");
		fuse_arguments[3] = "allow_other";

		numargs = 4;
#if 0
	}
#endif /* 0 */

	// create cache path - ignore errors if it not exists
	// TODO: check ownership
	struct stat s;
	const char * cache_path = env.getCachePath().c_str();

	int stat_result = stat(cache_path, &s);
	if ((stat_result == 0) && !S_ISDIR(s.st_mode)) {
		throw OFSException(env.getCachePath() + ": not a directory",
				ENOTDIR,
				true);
	}

	if (stat_result == -1 && errno == ENOENT) {
		if (mkdir(cache_path, 0777) == -1) {
			throw OFSException("Failed to create " + env.getCachePath(),
					errno,
					true);
		}
	}

//drop privileges to uid and gid
// TODO: this should probably go into environment, too
	gid_t gid = env.getGid();
	if (gid == -1) {
		struct passwd * userinfo;
		userinfo = getpwuid(env.getUid());
		if (userinfo == NULL) {
			cerr << "Could not determine default group id" << endl;
			exit (EXIT_FAILURE);
		}
		gid = userinfo->pw_gid;
	}

	if (setegid(gid) == -1) {
		cerr << "Failed to set GID\n";
		exit(EXIT_FAILURE);
	}

	if (seteuid(env.getUid()) == -1) {
		cerr << "Failed to set UID\n";
		exit(EXIT_FAILURE);
	}

	return my_ofs.main(numargs, fuse_arguments, NULL, &my_ofs);
}
