/***************************************************************************
 *   Copyright (C) 2008, 2011 by Frank Gsellmann, Peter Trommler           *
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

#include <cstdlib>
#include <stdio.h>
#include <ofsconf.h>
#include "options.h"
#include <assert.h>
#include <ofshash.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <cstring>
using namespace std;

#define MAX_PATH 1024

/**
 * @TODO: This is very unclean
 * @param argc 
 * @param argv[] 
 * @return 
 */
int main(int argc, char *argv[])
{
	// No! see --help! assert(argc > 2);
	//////////////////////////////////////////////////////////////////////////
	// MOUNT
	//////////////////////////////////////////////////////////////////////////

	string shareurl = argv[1];
	string sharepath;
	string remotefstype;
	string shareremote;
	string remotemountpoint;
	string ofsmountpoint = argv[2];
	bool sloppy;

	// get remote path
	char* pchDoppelPunktPos = strchr(argv[1], ':');
	assert(pchDoppelPunktPos != NULL);
	int nDoppelPunktIndex = int (pchDoppelPunktPos - argv[1]);

	remotefstype = shareurl.substr(0,nDoppelPunktIndex);
	sharepath = shareurl.substr(nDoppelPunktIndex+3);
	// handle special protocols
	if(remotefstype == "smb" ||
			remotefstype == "smbfs" ||
			remotefstype == "cifs")
		shareremote = string("//") + sharepath;
	else if(remotefstype == "file") {
		shareremote = string("/") + sharepath;
		remotemountpoint = shareremote;
	} else
		shareremote = sharepath;

	// TODO: treat options properly
	char* pszOptions;

	// FIXME: Treat options properly. Need a way to pass options to both OFS and remote FS
	// look at cryptfs and how they pass multiple name value pairs under one "key"
	my_options(argc, argv, &pszOptions, &sloppy);

	char const* pArgumente[9];

	if(remotefstype != "file") {
		OFSConf& conf = OFSConf::Instance();

		// set mount point
		remotemountpoint = conf.GetRemotePath()+"/"+ofs_hash(shareurl);
		const char * remote = remotemountpoint.c_str();

		// create mount point and check and if it exits check it is actually a directory
		struct stat s;
		int stat_result = stat(remote, &s);
		if ((stat_result == 0) && !S_ISDIR(s.st_mode)) {
			return -ENOTDIR;
		}

		if (stat_result == -1 && errno == ENOENT) {
			if (mkdir(remote, 0777) == -1) {
				return -errno;
			}
		}

		// Mount remote filesystem
		int childpid = fork();
		int status;
		if(childpid == 0) {
			execlp("mount", "mount",
					"-t", remotefstype.c_str(),
					shareremote.c_str(),
					remotemountpoint.c_str(),
					"-o", pszOptions,
					sloppy ? "-s" : NULL,
					NULL);
			return -errno;
		}
		if(childpid < 0) {
			perror("mount.ofs");
			return -errno;
		}

		int childpid2 = wait(&status);
		int exitstatus = WEXITSTATUS(status);
		if(WIFEXITED(status) && exitstatus) {
			errno = exitstatus;
			perror("mount.ofs: sub mount: ");
			exit(exitstatus);
		}

		//////////////////////////////////////////////////////////////////////////
		// OFS
		//////////////////////////////////////////////////////////////////////////


		pArgumente[0] = "ofs";
		pArgumente[1] = ofsmountpoint.c_str();
		pArgumente[2] = shareurl.c_str();
		pArgumente[3] = "-t"; // allow all users access to file system

		pArgumente[4] = "-p"; // mount options
		pArgumente[5] = pszOptions;
		pArgumente[6] = "-o";
		pArgumente[7] = pszOptions;
		pArgumente[8] = NULL;
	} else {
		pArgumente[0] = "ofs";
		pArgumente[1] = ofsmountpoint.c_str();
		pArgumente[2] = shareurl.c_str();
		pArgumente[3] = "-r";
		pArgumente[4] = sharepath.c_str();
		pArgumente[5] = "-n";
		if(geteuid() == 0) {
			pArgumente[6] = "-t";
			pArgumente[7] = NULL;
		} else
			pArgumente[6] = NULL;
	}

	// let ofs do the rest
	execvp(OFS_BINARY, (char* const*)pArgumente);
	perror("mount.ofs: exec: ");

	return -errno;
}
