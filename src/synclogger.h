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


#include "logger.h"

#include <string>
#include <list>
using namespace std;

struct cfg_t;
class SyncLogEntry;
/**
    @author Frank Gsellmann <frank.gsellmann@gmx.de>
*/
class SyncLogger : public Logger
{
public:
    static SyncLogger& Instance();
    ~SyncLogger();
    virtual bool AddEntry(const char* pszHash, const char* pszFilePath, const char chType);
    virtual bool ParseFile(const char* pszHash);
    virtual SyncLogEntry ReadFirstEntry(const char* pszHash);
    virtual void CalcLogFileName(const char* pszHash, char* pszLogName);
    virtual list<SyncLogEntry> GetEntries(const char* pszHash, const string strFilePath);
    virtual bool RemoveEntry(const char* pszHash, SyncLogEntry& sle);
    virtual char getModDependingOnOtherEntries(const char* pszHash, const string strFilePath,const char chType); //oreiche	
    virtual bool deleteOtherEntries(const char* pszHash);

protected:
    SyncLogger();
    SyncLogEntry ReadEntry(cfg_t* pEntryCFG);
protected:
//    FILE* m_pFile;
private:
    static std::auto_ptr<SyncLogger> theSyncLoggerInstance;
    static Mutex m_mutex;
};

#include "synclogentry.h"

#endif	// !SYNCLOGGER_H
