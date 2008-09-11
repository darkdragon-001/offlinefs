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

#include "SyncLogger.h"

// Initializes the class attributes.
std::auto_ptr<SyncLogger> SyncLogger::theSyncLoggerInstance;
Mutex SyncLogger::m_mutex;

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION/ DESTRUCTION
//////////////////////////////////////////////////////////////////////////////

SyncLogger::SyncLogger()
{
    m_pFile = fopen("C:\\Sync.log", "a");
}

SyncLogger::~SyncLogger()
{
    fclose(m_pFile);
}

SyncLogger& SyncLogger::Instance()
{
    MutexLocker obtainLock(m_mutex);
    if (theSyncLoggerInstance.get() == 0)
    {
        theSyncLoggerInstance.reset(new SyncLogger);
    }

    return *theSyncLoggerInstance;
}


bool SyncLogger::AddEntry(const char* pszFilePath)
{
    // +-----------------------------+
    // | Hash (8 bytes) base64       |
    // +-----------------------------+
    // | File time (8 bytes)         |
    // +-----------------------------+
    // | Reserved (16 bytes)         |
    // +-----------------------------+
    // | File path (variable length) |
    // +-----------------------------+

    const int nLen = strlen(pszFilePath);
    time_t aktTime;
    time(&aktTime);
    char szEntry[32 + _MAX_PATH + 1];
    memset(szEntry, 0, 32);
    strncpy(&szEntry[8], (char*)&aktTime, sizeof(time_t)); // sizeof(FILETIME));
    strncpy(&szEntry[32], pszFilePath, _MAX_PATH);

    // TODO: Calculate hash and copy it to &szEntry[0]

    fwrite(szEntry, sizeof(char), 32 + strlen(pszFilePath), m_pFile);
    fwrite("\r\n", sizeof(char), 2, m_pFile);

    return true;
}
