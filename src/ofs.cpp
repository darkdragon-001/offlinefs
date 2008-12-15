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
#include "ofs_fuse.h"
#include "backingtreepersistence.h"
#include "filesystemstatusmanager.h"
#include "ofsenvironment.h"
#include <string>
#include <cstring>

using namespace std;

int main(int argc, char *argv[])
{
	ofs_fuse my_ofs;
	cout << "Starting" << endl;
	try {
		OFSEnvironment::init(argc, argv);
	} catch (OFSException e) {
		cout << OFSEnvironment::getUsageString();
		return 0;
	}
	OFSEnvironment &env = OFSEnvironment::Instance();
	char *args[5];
	int numargs;
	args[0] = new char[env.getBinaryPath().length()+1];
	strncpy(args[0], env.getBinaryPath().c_str(),
		env.getBinaryPath().length()+1);
	args[1] = new char[env.getMountPoint().length()+1];
	strncpy(args[1], env.getMountPoint().c_str(),
		env.getMountPoint().length()+1);
	if(env.isAllowOther()) {
		args[2] = "-o";
		args[3] = "allow_other";
		args[4] = NULL;
		numargs = 4;
	} else {
		args[2] = NULL;
		numargs = 2;
	}
//
return my_ofs.main(numargs, args, NULL, &my_ofs);

  //return EXIT_SUCCESS;
}
