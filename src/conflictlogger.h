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
#ifndef CONFLICTLOGGER_H
#define CONFLICTLOGGER_H

#include "logger.h"

#include "mutexlocker.h"

#include "synclogentry.h"

#include <string>
#include <list>
using namespace std;

struct cfg_t;

/**
    @author Frank Gsellmann <frank.gsellmann@gmx.de>
*/
class ConflictLogger : public Logger
{
public:
    static ConflictLogger& Instance();
    ~ConflictLogger();
    virtual bool AddEntry(const char* pszHash, const char* pszFilePath, const char chType);
    virtual bool ParseFile(const char* pszHash);
    virtual SyncLogEntry ReadFirstEntry(const char* pszHash);
    virtual void CalcLogFileName(const char* pszHash, char* pszLogName);
    virtual list<SyncLogEntry> GetEntries(const char* pszHash, const string strFilePath);
    virtual bool RemoveEntry(const char* pszHash, SyncLogEntry& sle);
protected:
    ConflictLogger();
    SyncLogEntry ReadEntry(cfg_t* pEntryCFG);
protected:
//    FILE* m_pFile;
private:
    static std::auto_ptr<ConflictLogger> theConflictLoggerInstance;
    static Mutex m_mutex;
};

#endif	// !CONFLICTLOGGER_H
