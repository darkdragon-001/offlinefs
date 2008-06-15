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
#include "ofsenvironment.h"
#include "ofsexception.h"
#include "ofsconf.h"

std::auto_ptr<OFSEnvironment> OFSEnvironment::theOFSEnvironmentInstance;
Mutex OFSEnvironment::m;
Mutex OFSEnvironment::initm;
bool OFSEnvironment::initialized = false;

OFSEnvironment& OFSEnvironment::Instance()
{
    if(!initialized)
        throw new OFSException("OFS Environment not initialized", 1);
    MutexLocker obtain_lock(m);
    if (theOFSEnvironmentInstance.get() == 0)
      theOFSEnvironmentInstance.reset(new OFSEnvironment());
    return *theOFSEnvironmentInstance;
}

OFSEnvironment::OFSEnvironment()
{
}


OFSEnvironment::~OFSEnvironment()
{
}

void OFSEnvironment::init()
{
	MutexLocker obtain_lock(initm);
	initialized = true;
	OFSEnvironment &env = Instance();
	// config file
	OFSConf &ofsconf = OFSConf::Instance();
	env.remotePath = ofsconf.GetRemotePath();
	env.cachePath = ofsconf.GetBackingTreePath();
	// TODO: command line (via parameter)
	// TODO: cache-and remote path have to be custom for each share
}

string OFSEnvironment::getRemotePath()
{
	return remotePath;
}

string OFSEnvironment::getCachePath()
{
	return cachePath;
}
