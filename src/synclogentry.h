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
#ifndef SYNCLOGENTRY_H
#define SYNCLOGENTRY_H

#include <string>

using namespace std;

/**
	@author Frank Gsellmann <frank.gsellmann@gmx.de>
*/
class SyncLogEntry
{
public:
    /**
     * Constructor of SyncLogEntry takes the relative position
     * of the backingtree as argument
     * @param rPath 
     */
    SyncLogEntry(const string strFilePath, const string strModTime,
		const char chModType, const int nNumber);
    ~SyncLogEntry();

    /**
     * Compares the file path of the entry with the given file path.
     * @param strFilePath (in): File path to compare with
     * @return true if equal false otherwise
     */
    const bool operator==(const string strFilePath);

    /**
     * Returns the relative path of the file.
     * @return relative path of the file
     */
	const string GetFilePath() const;

    /**
     * Returns the time stamp of the modification.
     * @return time stamp of the modification
     */
	const string GetModTime() const;

    /**
     * Returns the type of the modification.
     * @return type of the modification
     */
	const char GetModType() const;

    inline int GetNumber() { return m_nNumber; };
//    friend bool SyncLogger::RemoveEntry(SyncLogEntry& sle);
//    friend bool ConflictLogger::RemoveEntry(SyncLogEntry& sle);

protected:
    string m_strFilePath;
    string m_strModTime;
	char m_chModType;
    int m_nNumber;
};

#endif	// !SYNCLOGENTRY_H
