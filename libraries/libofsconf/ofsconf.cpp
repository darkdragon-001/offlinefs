/***************************************************************************
 *   Copyright (C) 2008 by Tobias Jähnel,,,   *
 *   tobias@gmail.com   *
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
#include "ofsconf.h"
#include <assert.h>
#include <mutexlocker.h>
#include <confuse.h>

#define BACKING_TREE_PATH_VARNAME "backingTreePath"
#define MOUNT_REMOTE_PATHS_TO_VARNAME "mountRemotePathsTo"
#define REMOTE_SHARE_VARNAME "remoteShare"
#define LISTEN_DEVICES_VARNAME "listen"
#define LOGLEVEL_VARNAME "loglevel"

// TODO: Do not hardcode paths here. See Makefile.am in mounthelper!
#define BACKING_TREE_PATH_DEFAULT "/var/ofs/backing"
#define MOUNT_REMOTE_PATHS_TO_DEFAULT "/var/ofs/remote"
#define LISTEN_DEVICES_DEFAULT "{eth0}"
#define LOGLEVEL_DEFAULT 6 // LOG INFO

// Initializes the class attributes.
std::auto_ptr<OFSConf> OFSConf::theOFSConfInstance;
Mutex OFSConf::m_mutex;

//////////////////////////////////////////////////////////////////////////////
// KONSTRUKTION/ DESTRUKTION
//////////////////////////////////////////////////////////////////////////////

OFSConf::OFSConf()
{
    m_bFileParsed = false;
    m_pCFG = 0;
    m_logLvl = LOGLEVEL_DEFAULT; // LOG_INFO; 
    // set default config values
    remotePath = MOUNT_REMOTE_PATHS_TO_DEFAULT;
    backingPath = BACKING_TREE_PATH_DEFAULT;
    listendevices.push_back("eth0"); // TODO: do not hardcode
    ParseFile();
}

OFSConf::~OFSConf()
{
    if (m_bFileParsed && m_pCFG != 0)
        cfg_free(m_pCFG);
}

OFSConf& OFSConf::Instance()
{
    MutexLocker obtainLock(m_mutex);
    if (theOFSConfInstance.get() == 0)
    {
        theOFSConfInstance.reset(new OFSConf);
    }

    return *theOFSConfInstance;
}

//////////////////////////////////////////////////////////////////////////////
// PUBLIC METHODS
//////////////////////////////////////////////////////////////////////////////

bool OFSConf::ParseFile()
{
    if (m_bFileParsed)
    {
        // File is already parsed.
        return true;
    }

    cfg_opt_t shareOpts[] =
    {
        // Parses within the group.
        CFG_STR(BACKING_TREE_PATH_VARNAME,
		BACKING_TREE_PATH_DEFAULT, CFGF_NONE),
        CFG_STR(MOUNT_REMOTE_PATHS_TO_VARNAME,
		MOUNT_REMOTE_PATHS_TO_DEFAULT, CFGF_NONE),
	CFG_STR(LISTEN_DEVICES_VARNAME,
		LISTEN_DEVICES_DEFAULT, CFGF_NONE),
	CFG_INT(LOGLEVEL_VARNAME,LOGLEVEL_DEFAULT,CFGF_NONE),
        CFG_END()
    };

    // Initializes the parser.
    m_pCFG = cfg_init(shareOpts, CFGF_NONE);

    // Parses the file.
    if (cfg_parse(m_pCFG, "/etc/ofs.conf") == CFG_PARSE_ERROR)
        return false;

    m_bFileParsed = true;
    // remote path
    remotePath = cfg_getstr(m_pCFG, MOUNT_REMOTE_PATHS_TO_VARNAME);
    // backing path
    backingPath = cfg_getstr(m_pCFG, BACKING_TREE_PATH_VARNAME);
    // log level
    m_logLvl = cfg_getint(m_pCFG,LOGLEVEL_VARNAME);
    // listening devices
    listendevices.clear();
    for(int i=0; i < cfg_size(m_pCFG, LISTEN_DEVICES_VARNAME); i++) {
        listendevices.push_back(
            cfg_getnstr(m_pCFG, LISTEN_DEVICES_VARNAME, i));
    }

    return true;
}

string OFSConf::GetRemotePath()
{
	return remotePath;
}

string OFSConf::GetBackingTreePath()
{
	return backingPath;
}

list<string> OFSConf::GetListenDevices()
{
	return listendevices;
}
