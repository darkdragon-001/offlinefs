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
#include "../../src/mutexlocker.h"
#include <confuse.h>

#define BACKING_TREE_PATH_VARNAME "backingTreePath"
#define MOUNT_REMOTE_PATHS_TO_VARNAME "mountRemotePathsTo"
#define REMOTE_SHARE_VARNAME "remoteShare"

#define BACKING_TREE_PATH_DEFAULT "/var/ofs/backing"
#define MOUNT_REMOTE_PATHS_TO_DEFAULT "/var/ofs/remote"

// Initialisiert die Klassenattribute.
std::auto_ptr<OFSConf> OFSConf::theOFSConfInstance;
Mutex OFSConf::m_mutex;

//////////////////////////////////////////////////////////////////////////////
// KONSTRUKTION/ DESTRUKTION
//////////////////////////////////////////////////////////////////////////////

OFSConf::OFSConf()
{
    m_bFileParsed = false;
    m_pCFG = 0;
}

OFSConf::~OFSConf()
{
    if (m_bFileParsed && m_pCFG != 0)
        cfg_free(m_pCFG);
}

//////////////////////////////////////////////////////////////////////////////
//
//! Gibt einen autom. Zeiger auf das einzige OFSConf-Objekt zur�ck.<p>
//! Beim ersten Aufruf wird eine Instanz des Objekts erzeugt.<p>
//! Es wird nur ein Objekt erzeugt, d.h. bei einem erneuten Aufruf,
//! wird ein Zeiger auf die vorhandene Instanz zur�ckgegeben.
//!
//! \result Zeiger auf die vorhandene Instanz
//
//////////////////////////////////////////////////////////////////////////////
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
// OEFFENTLICHE METHODEN
//////////////////////////////////////////////////////////////////////////////

bool OFSConf::ParseFile()
{
    if (m_bFileParsed)
    {
        // Datei ist bereits geparst.
        return true;
    }

    cfg_opt_t shareOpts[] =
    {
        // Parst innerhalb der Gruppe.
        CFG_STR(BACKING_TREE_PATH_VARNAME, BACKING_TREE_PATH_DEFAULT, CFGF_NONE),
        CFG_STR(MOUNT_REMOTE_PATHS_TO_VARNAME, MOUNT_REMOTE_PATHS_TO_DEFAULT, CFGF_NONE),
        CFG_END()
    };

    // Initialisiert den Parser.
    m_pCFG = cfg_init(shareOpts, CFGF_NONE);

    // Parst die Datei.
    if (cfg_parse(m_pCFG, "/etc/ofs.conf") == CFG_PARSE_ERROR)
        return false;

    m_bFileParsed = true;

    return true;
}

string OFSConf::GetRemotePath()
{
    assert(m_pCFG != 0);
    return cfg_getstr(m_pCFG, MOUNT_REMOTE_PATHS_TO_VARNAME);
}

string OFSConf::GetBackingTreePath()
{
    assert(m_pCFG != 0);
    return cfg_getstr(m_pCFG, BACKING_TREE_PATH_VARNAME);
}
