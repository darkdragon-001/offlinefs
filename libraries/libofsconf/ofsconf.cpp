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
    m_pRemoteShareList = 0;
}

OFSConf::~OFSConf()
{
    if (m_bFileParsed && m_pCFG != 0)
        cfg_free(m_pCFG);
    if (m_pRemoteShareList != 0)
        delete[] m_pRemoteShareList;
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
OFSConf& OFSConf::GetInstance()
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
    cfg_opt_t opts[] =
    {
        // Parst die einzelnen Gruppen.
        CFG_SEC(REMOTE_SHARE_VARNAME, shareOpts, CFGF_TITLE | CFGF_MULTI),
        CFG_END()
    };

    // Initialisiert den Parser.
    m_pCFG = cfg_init(opts, CFGF_NONE);

    // Parst die Datei.
    if (cfg_parse(m_pCFG, "/etc/ofs.conf") == CFG_PARSE_ERROR)
        return false;

    m_bFileParsed = true;
    m_pRemoteShareList = new string[GetNumberOfRemoteShares()];

    return true;
}

int OFSConf::GetNumberOfRemoteShares()
{
    return cfg_size(m_pCFG, REMOTE_SHARE_VARNAME);
}

string OFSConf::GetRemoteShareName(const int nIndex)
{
    if (m_pRemoteShareList[nIndex] == "")
    {
        cfg_t* pcfgShare = cfg_getnsec(m_pCFG, REMOTE_SHARE_VARNAME, nIndex);
        assert(pcfgShare != 0);
        m_pRemoteShareList[nIndex] = cfg_title(pcfgShare);
    }

    return m_pRemoteShareList[nIndex];
}

string OFSConf::GetRemotePathByIndex(const int nIndex)
{
    cfg_t* pcfgShare = cfg_getnsec(m_pCFG, REMOTE_SHARE_VARNAME, nIndex);
    assert(pcfgShare != 0);
    return cfg_getstr(pcfgShare, MOUNT_REMOTE_PATHS_TO_VARNAME);
}

string OFSConf::GetRemotePathByName(const string& strName)
{
    int nIndex = GetIndexOfRemoteShare(strName);
    return GetRemotePathByIndex(nIndex);
}

string OFSConf::GetBackingTreePathByIndex(const int nIndex)
{
    cfg_t* pcfgShare = cfg_getnsec(m_pCFG, REMOTE_SHARE_VARNAME, nIndex);
    assert(pcfgShare != 0);
    return cfg_getstr(pcfgShare, BACKING_TREE_PATH_VARNAME);
}

string OFSConf::GetBackingTreePathByName(const string& strName)
{
    int nIndex = GetIndexOfRemoteShare(strName);
    return GetBackingTreePathByIndex(nIndex);
}

//////////////////////////////////////////////////////////////////////////////
// GESCHUETZTE METHODEN
//////////////////////////////////////////////////////////////////////////////

int OFSConf::GetIndexOfRemoteShare(const string& strName)
{
    int nIndex = -1;
    const int nCount = GetNumberOfRemoteShares();
    for (int i = 0; i < nCount; i++)
    {
        if (strName == GetRemoteShareName(i))
        {
            nIndex = i;
            break;
        }
    }
    assert(nIndex != -1);
    return nIndex;
}
