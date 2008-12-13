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
#ifndef SYNCLOGGER_H
#define SYNCLOGGER_H

/*
#include "file_sync.h"
#include "file.h"
#include "syncstatetype.h"
#include "persistable.h"
#include <iostream>
#include <map>
#include <string>

using namespace std;
*/
#include "mutexlocker.h"

/**
	@author Frank Gsellmann <frank.gsellmann@gmx.de>
*/
class SyncLogger
{
public:
    static SyncLogger& Instance();
    ~SyncLogger();
    bool AddEntry(const char* pszHash, const char* pszFilePath, const char chType);
	bool ParseFile(const char* pszHash);
    SyncLogEntry ReadFirstEntry(const char* pszHash);
	void CalcSyncLogName(const char* pszHash, char* pszLogName);
	FILE* OpenSyncLog(const char* pszHash, const char* szMode);
	list<SyncLogEntry> GetEntries(const char* pszHash, const string strFilePath);
	void ReintegrateFile(const char* pszHash, const string strFilePath);
protected:
    SyncLogger();
	SyncLogEntry ReadEntry(cfg_t* pEntryCFG);
protected:
//    FILE* m_pFile;
    int m_nNewIndex;
	char m_szCurShare[1024];
    cfg_t* m_pCFG;
private:
    static std::auto_ptr<SyncLogger> theSyncLoggerInstance;
    static Mutex m_mutex;
};

#endif	// !SYNCLOGGER_H
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
#ifndef SYNCLOGGER_H
#define SYNCLOGGER_H

/*
#include "file_sync.h"
#include "file.h"
#include "syncstatetype.h"
#include "persistable.h"
#include <iostream>
#include <map>
#include <string>

using namespace std;
*/
#include "mutexlocker.h"

/**
	@author Frank Gsellmann <frank.gsellmann@gmx.de>
*/
class SyncLogger
{
public:
    static SyncLogger& Instance();
    ~SyncLogger();
    bool AddEntry(const char* pszFilePath);
protected:
    SyncLogger();
private:
    static std::auto_ptr<SyncLogger> theSyncLoggerInstance;
    static Mutex m_mutex;
    FILE* m_pFile;
};

#endif	// !SYNCLOGGER_H
