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
#ifndef LOGGER_H
#define LOGGER_H

#include "synclogentry.h"

#include <string>
#include <list>
using namespace std;

#define MAX_PATH 512
/**
 * This method converts from integer to char array
 * This method is not available on linux
 * taken from http://www.jb.man.ac.uk/~slowe/cpp/itoa.html
 */
char* itoa(int val, char *ret, int base);

struct cfg_t;

class SyncLogEntry;
/**
    @author Frank Gsellmann <frank.gsellmann@gmx.de>
*/
class Logger
{
public:
    Logger();

    virtual bool AddEntry(const char* pszHash, const char* pszFilePath, const char chType) = 0;
    virtual bool ParseFile(const char* pszHash) = 0;
    virtual SyncLogEntry ReadFirstEntry(const char* pszHash) = 0;
    virtual void CalcLogFileName(const char* pszHash, char* pszLogName) = 0;
    virtual list<SyncLogEntry> GetEntries(const char* pszHash, const string strFilePath) = 0;
    virtual bool RemoveEntry(const char* pszHash, SyncLogEntry& sle) = 0;

    FILE* OpenLogFile(const char* pszHash, const char* szMode);

protected:
    int m_nNewIndex;
    char m_szCurShare[1024];
    cfg_t* m_pCFG;
};

#endif	// !LOGGER_H
