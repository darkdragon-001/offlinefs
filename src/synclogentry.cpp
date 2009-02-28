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
#include "synclogentry.h"

SyncLogEntry::SyncLogEntry(const string strFilePath,
						   const string strModTime,
						   const char chModType,
						   const int nNumber)
{
	m_strFilePath = strFilePath;
	m_strModTime = strModTime;
	m_chModType = chModType;
	m_nNumber = nNumber;
}


SyncLogEntry::~SyncLogEntry()
{
}

const bool SyncLogEntry::operator==(const string strFilePath)
{
	return (strFilePath.compare(m_strFilePath) == 0);
}

const string SyncLogEntry::GetFilePath() const
{
	return m_strFilePath;
}

const string SyncLogEntry::GetModTime() const
{
	return m_strModTime;
}

const char SyncLogEntry::GetModType() const
{
	return m_chModType;
}
