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
#ifndef CONFLICTMANAGER_H
#define CONFLICTMANAGER_H

#include "persistable.h"
#include "mutexlocker.h"
#include <string>
#include <list>
using namespace std;

/**
	@author Tobias Jaehnel <tjaehnel@gmail.com>
*/
class ConflictManager : public persistable {
public:
    /**
     * Get singleton instance
     * @return singleton instance
     */
    static ConflictManager& Instance();    

    ~ConflictManager();

    void addConflictFile(string relativePath);
    
    void removeConflictFile(string relativePath);
    
    bool isConflicted(string relativePath);
    
    /**
     * Writes the local states to the disk
     */
    virtual void persist() const;
    /**
     * Reads the local states from the disk
     */
    virtual void reinstate();

protected:
    ConflictManager();
    
private:
    list<string> conflictfiles;
    
    static std::auto_ptr<ConflictManager> 
        theConflictManagerInstance;
    static Mutex m;

};

#endif
