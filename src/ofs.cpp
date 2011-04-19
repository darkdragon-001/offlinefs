/***************************************************************************
 *   Copyright (C) 2007 by Carsten Kolassa   *
 *   Carsten@Kolassa.de   *
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
#include <cstdlib>
#include <string>
#include <cstring>
#include <assert.h>
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

/**
 * @TODO: This is very unclean
 * @param argc
 * @param argv[]
 * @return
 */
int main(int argc, char *argv[])
{
	if(! ofslog::init() ){
		std::cerr << argv[0] << ": Failed to init log system\n";
		return 2; // system error return code, see mount(8)
	}

	try {
		OFSEnvironment::init(argc, argv);
	} catch (OFSException& e) {
		cout << OFSEnvironment::getUsageString();
		return 0;
	}

	FilesystemStatusManager::Instance().mountfs();

	ofs_fuse my_ofs;
	ofslog::info("Starting ofs daemon");
	ofslog::debug("Adopting parameters");
	OFSEnvironment &env = OFSEnvironment::Instance();
	char *fuse_arguments[5];
	int numargs;
	// TODO: check if we really need to copy here, if yes use strdup(3)
	fuse_arguments[0] = new char[env.getBinaryPath().length()+1];
	strncpy(fuse_arguments[0], env.getBinaryPath().c_str(),
		env.getBinaryPath().length()+1);
	fuse_arguments[1] = new char[env.getMountPoint().length()+1];
	strncpy(fuse_arguments[1], env.getMountPoint().c_str(),
		env.getMountPoint().length()+1);
	if(env.isAllowOther()) {
		fuse_arguments[2] = new char[3];
		strncpy(fuse_arguments[2], "-o", 3);
		fuse_arguments[3] = new char[12];
		strncpy(fuse_arguments[3], "allow_other", 12);
		fuse_arguments[4] = NULL;
		numargs = 4;
	} else {
		fuse_arguments[2] = NULL;
		numargs = 2;
	}

	// create cache path - ignore errors if it not exists
	// FIXME: check if directory exists and mkdir if not
	mkdir(env.getCachePath().c_str(), 0777);

	return my_ofs.main(numargs, fuse_arguments, NULL, &my_ofs);
}
