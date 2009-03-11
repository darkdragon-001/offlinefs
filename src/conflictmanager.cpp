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
#include "conflictmanager.h"
#include "conflictpersistence.h"

// Initializes the class attributes.
std::auto_ptr<ConflictManager> ConflictManager::theConflictManagerInstance;
Mutex ConflictManager::m;

ConflictManager::ConflictManager()
{
    reinstate();
}


ConflictManager::~ConflictManager()
{
}

ConflictManager& ConflictManager::Instance()
{
    MutexLocker obtain_lock(m);
    if (theConflictManagerInstance.get() == 0) {
    	theConflictManagerInstance.reset(new ConflictManager());
    }
    return *theConflictManagerInstance;
}

void ConflictManager::addConflictFile(string relativePath)
{
    conflictfiles.remove(relativePath);
    conflictfiles.push_back(relativePath);
    persist();
}
    
void ConflictManager::removeConflictFile(string relativePath)
{
    conflictfiles.remove(relativePath);
    persist();
}
 
bool ConflictManager::isConflicted(string relativePath)
{
    for( list<string>::iterator iter = conflictfiles.begin();
        iter != conflictfiles.end(); iter++ )
    {
        string filename = *iter;
        if(relativePath.length() <= filename.length()
           && filename.substr(0, relativePath.length()) == relativePath
          )
        {
            return true;
        }
    }
    return false;
}


void ConflictManager::persist() const
{
    ConflictPersistence::Instance().files(conflictfiles);
}

void ConflictManager::reinstate()
{
    conflictfiles = ConflictPersistence::Instance().files();
}