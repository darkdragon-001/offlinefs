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
#ifndef OFSCONF_H
#define OFSCONF_H

#include <string>

struct cfg_t;
class Mutex;
using namespace std;

/**
	@author Frank Gsellmann,,, <frank.gsellmann@gmx.de>
*/
class OFSConf
{
protected:
    OFSConf();

public:
    ~OFSConf();

public:
    static OFSConf& Instance();

    bool ParseFile();

    int GetNumberOfRemoteShares();
    string GetRemoteShareName(const int nIndex);
    string GetRemotePathByIndex(const int nIndex);
    string GetRemotePathByName(const string& strName);
    string GetBackingTreePathByIndex(const int nIndex);
    string GetBackingTreePathByName(const string& strName);

protected:
    int GetIndexOfRemoteShare(const string& strName);

    bool m_bFileParsed;
    cfg_t* m_pCFG;

    //! Automatischer Zeiger auf das einzige OFSConf-Objekt.<br>
    //! (Wird nach dem Singleton-Pattern verwendet.)
    static std::auto_ptr<OFSConf> theOFSConfInstance;

private:
    string* m_pRemoteShareList;
    static Mutex m_mutex;
};

#endif
