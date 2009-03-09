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

#include "logger.h"

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION/ DESTRUCTION
//////////////////////////////////////////////////////////////////////////////
char* itoa(int val, int base) {
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
		
	return &buf[i+1];
}

Logger::Logger()
{
	m_pCFG = NULL;
	m_nNewIndex = 0;
	m_szCurShare[0] = '\0';
}

FILE* Logger::OpenLogFile(const char* pszHash, const char* szMode)
{
	// Creates the file name of the sync log.
	char szLogName[MAX_PATH];
	CalcLogFileName(pszHash, szLogName);
/*
	strcpy(szLogName, "Sync_");
	strcat(szLogName, pszHash);
	strcat(szLogName, ".log");
*/
	// Opens the sync log file.
	return fopen(szLogName, szMode);
}
