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

#include "synclogger.h"

#include "synclogentry.h"

#define FILE_PATH_VARNAME "filePath"
#define MOD_TIME_VARNAME "modTime"
#define MOD_TYPE_VARNAME "modType"
#define MOD_NUMBER_VARNAME "modNumber"

#define FILE_PATH_DEFAULT "C:\\WINDOWS\\SYSTEM\\win.ini"
#define MOD_TIME_DEFAULT "0000/00/00 25:00:00"
#define MOD_TYPE_DEFAULT "e"

// Initializes the class attributes.
std::auto_ptr<SyncLogger> SyncLogger::theSyncLoggerInstance;
Mutex SyncLogger::m_mutex;

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION/ DESTRUCTION
//////////////////////////////////////////////////////////////////////////////

SyncLogger::SyncLogger()
{
//    m_pFile = fopen("C:\\Sync.log", "a");
	m_pCFG = NULL;
}

SyncLogger::~SyncLogger()
{
//    fclose(m_pFile);
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

bool SyncLogger::AddEntry(const char* pszHash,
						  const char* pszFilePath,
						  const char chType)
{
    // modNumber 123
	// {
	//     filePath = ABC/XYZ.xyz
	//     modTime = 00/00/00 25:00:00
	//     modType = ?
	// }

	// Creates the new entry.
    char szEntry[MAX_PATH + 1024];
	char szIndex[15];
	char szDate[9];
	char szTime[9];

	itoa(m_nNewIndex, szIndex, 10);

	strdate(szDate);
	strtime(szTime);

	// Writes the new entry.
    strcpy(szEntry, MOD_NUMBER_VARNAME);
    strcat(szEntry, " ");
    strcat(szEntry, szIndex);
    strcat(szEntry, "\n{\n\t");
    strcat(szEntry, FILE_PATH_VARNAME);
    strcat(szEntry, " = ");
    strcat(szEntry, pszFilePath);
    strcat(szEntry, "\n\t");
    strcat(szEntry, MOD_TIME_VARNAME);
    strcat(szEntry, " = ");
    strcat(szEntry, szDate);
    strcat(szEntry, " ");
    strcat(szEntry, szTime);
    strcat(szEntry, "\n\t");
    strcat(szEntry, MOD_TYPE_VARNAME);
    strcat(szEntry, " = ");
    strcat(szEntry, &chType);
    strcat(szEntry, "\n}\n\n");
    strncpy(&szEntry[8], (char*)&aktTime, sizeof(time_t)); // sizeof(FILETIME));
/*
	// Creates the file name of the sync log.
	char szLogName[MAX_PATH];
	strcpy(szLogName, "Sync_");
	strcat(szLogName, pszHash);
	strcat(szLogName, ".log");

	// Opens the sync log file.
	FILE* pFile = fopen(szLogName, "a");
	if (pFile == 0)
		return false;
*/
	// Opens the sync log file.
	FILE* pFile = OpenSyncLog(pszHash, "a");
	if (pFile == NULL)
		return false;

	// Writes the new entry into the sync log.
    fwrite(szEntry, sizeof(char), 32 + strlen(szEntry), pFile);
	fclose(pFile);

    return true;
}

//bool SyncLogger::ReadFirstEntry(const char* pszHash, char* pszFilePath, char& chType)
SyncLogEntry SyncLogger::ReadFirstEntry(const char* pszHash)
{
	ParseFile(pszHash);

	// Assures the correct parsing of the file.
	assert(m_pCFG != NULL);

//	cfg_t* pEntryCFG = cfg_getnsec(m_pCFG, MOD_NUMBER_VARNAME, 0);
	return ReadEntry(cfg_getnsec(m_pCFG, MOD_NUMBER_VARNAME, 0));
/*
	// Assures that the section has been found, which means there is at least one section.
    assert(pEntryCFG != NULL);

	// TODO: Copy values
	SyncLogEntry sle;
	sle.m_strFilePath = cfg_getstr(pEntryCFG, FILE_PATH_VARNAME);
	sle.m_strModTime = cfg_getstr(pEntryCFG, MOD_TIME_VARNAME);
	string strModType = cfg_getstr(pEntryCFG, MOD_TYPE_VARNAME);
	if (strModType.length() != 1)
	{
		// The log entry does not meet the standard.
		return NULL;
	}
	sle.m_chModType = strModType.c_str()[0];

    return sle;
*/
}

SyncLogEntry SyncLogger::ReadEntry(cfg_t* pEntryCFG)
{
	// Assures that the section has been found, which means there is at least one section.
    assert(pEntryCFG != NULL);

	// TODO: Copy values
	string strModType = cfg_getstr(pEntryCFG, MOD_TYPE_VARNAME);
	if (strModType.length() != 1)
	{
		// The log entry does not meet the standard.
		return NULL;
	}
	SyncLogEntry sle(cfg_getstr(pEntryCFG, FILE_PATH_VARNAME),
		cfg_getstr(pEntryCFG, MOD_TIME_VARNAME), strModType.c_str()[0]);

    return sle;
}

bool SyncLogger::ParseFile(const char* pszHash)
{
	if (strcmp(m_szCurShare, pszHash) == 0)
	{
		// File is currently parsed, there is no need to parse it again.
		return true;

    cfg_opt_t modEntry[] =
    {
        // Parses within the group.
        CFG_STR(FILE_PATH_VARNAME, FILE_PATH_DEFAULT, CFGF_NONE),
        CFG_STR(MOD_TIME_VARNAME, MOD_TIME_DEFAULT, CFGF_NONE),
        CFG_STR(MOD_TYPE_VARNAME, MOD_TYPE_DEFAULT, CFGF_NONE),
        CFG_END()
    };
    cfg_opt_t entries[] =
    {
        // Parses the single groups.
        CFG_SEC(MOD_NUMBER_VARNAME, modEntry, CFGF_TITLE | CFGF_MULTI),
        CFG_END()
    };

    // Initializes the parser.
//    cfg_t* pCFG = cfg_init(entries, CFGF_NONE);
    m_pCFG = cfg_init(entries, CFGF_NONE);

    // Parses the file.
	char szLogName[MAX_PATH];
	CalcSyncLogName(pszHash, szLogName);
//    if (cfg_parse(pCFG, szLogName) == CFG_PARSE_ERROR)
    if (cfg_parse(m_pCFG, szLogName) == CFG_PARSE_ERROR)
        return false;
}

void SyncLogger::CalcSyncLogName(const char* pszHash, char* pszLogName)
{
	strcpy(pszLogName, "Sync_");
	strcat(pszLogName, pszHash);
	strcat(pszLogName, ".log");
}

FILE* SyncLogger::OpenSyncLog(const char* pszHash, const char* szMode)
{
	// Creates the file name of the sync log.
	char szLogName[MAX_PATH];
	CalcSyncLogName(pszHash, szLogName);
/*
	strcpy(szLogName, "Sync_");
	strcat(szLogName, pszHash);
	strcat(szLogName, ".log");
*/
	// Opens the sync log file.
	return fopen(szLogName, szMode);
}

list<SyncLogEntry> SyncLogger::GetEntries(const char* pszHash, const string strFilePath)
{
	ParseFile(pszHash);

	// Assures the correct parsing of the file.
	assert(m_pCFG != NULL);

	list<SyncLogEntry> listOfEntries;
	const int nCount = cfg_size(m_pCFG, MOD_NUMBER_VARNAME);
	for (int i = 0; i < nCount; i++)
	{
		SyncLogEntry sle = ReadEntry(cfg_getnsec(m_pCFG, MOD_NUMBER_VARNAME, i));
		if (sle == strFilePath)
			listOfEntries.push_back(sle);
	}
	return listOfEntries;
}

void SyncLogger::ReintegrateFile(const char* pszHash, const string strFilePath)
{
	list<SyncLogEntry> listOfEntries = GetEntries(pszHash, strFilePath);
	const int nCount = (int)listOfEntries.size();
	for (int i = 0; i < nCount; i++)
	{
		// TODO: Reintegrate the file.
	}
}
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
