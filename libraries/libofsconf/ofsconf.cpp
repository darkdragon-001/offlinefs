/***************************************************************************
 *   Copyright (C) 2008 by Tobias JÃ¤hnel,,,   *
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

#define BACKING_TREE_PATH_VARNAME "backingTreePath"
#define MOUNT_REMOTE_PATHS_TO_VARNAME "mountRemotePathsTo"
#define REMOTE_SHARE_VARNAME "remoteShare"

OFSConf* OFSConf::m_pInstance = 0;
#ifdef ENABLE_SINGLETON_DESTRUCTION
    #ifdef _DEBUG
        bool OFSConf::m_bFirstInstance = true;
    #endif  // _DEBUG
#endif  // ENABLE_SINGLETON_DESTRUCTION

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
//! Gibt einen Zeiger auf das einzige OFSConf-Objekt zurück.<p>
//! Beim ersten Aufruf wird eine Instanz des Objekts erzeugt.<p>
//! Es wird nur ein Objekt erzeugt, d.h. bei einem erneuten Aufruf,
//! wird ein Zeiger auf die vorhandene Instanz zurückgegeben.
//!
//! \result Zeiger auf die vorhandene Instanz
//
//////////////////////////////////////////////////////////////////////////////
OFSConf* OFSConf::GetInstance()
{
#ifdef ENABLE_SINGLETON_DESTRUCTION
    assert(m_bFirstInstance);
#endif  // ENABLE_SINGLETON_DESTRUCTION

    if (m_pInstance == 0)
    {
        m_pInstance = new OFSConf;
    }

    return m_pInstance;
}

#ifdef ENABLE_SINGLETON_DESTRUCTION
//////////////////////////////////////////////////////////////////////////////
//
//! Zerstört das einzige OFSConf-Objekt.
//!
//! \remark Diese Funktion darf erst beim Beenden des Hauptprogramms
//!     aufgerufen werden, da sonst beim nächsten Aufruf von GetInstance()
//!     ein neues Objekt erzeugt wird.
//
//////////////////////////////////////////////////////////////////////////////
void OFSConf::DestroyInstance()
{
    if (m_pInstance != 0)
    {
#ifdef _DEBUG
        m_bFirstInstance = false;
#endif  // _DEBUG

        // Zerstört das OFSConf-Objekt.
        delete m_pInstance;
        m_pInstance = 0;
    }
}
#endif  // ENABLE_SINGLETON_DESTRUCTION

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
        CFG_STR(BACKING_TREE_PATH_VARNAME, "", CFGF_NONE),
        CFG_STR(MOUNT_REMOTE_PATHS_TO_VARNAME, "", CFGF_NONE),
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
//        return m_pRemoteShareList[nIndex];
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
