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
#ifndef CONFLICTPERSISTENCE_H
#define CONFLICTPERSISTENCE_H

#include "persistencemanager.h"
#include <list>
#include "mutexlocker.h"
#include <memory>
using namespace std;

#define CONFIGKEY_CFILES "cfiles"
#define PERSISTENCE_MODULE_NAME "conflicts"

/**
	@author Tobias Jaehnel <tjaehnel@gmail.com>
*/
class ConflictPersistence : public PersistenceManager
{
public:
    ~ConflictPersistence();
    
    /**
     * Get singleton instance
     * @return singleton instance
     */
    static ConflictPersistence& Instance();
    /**
     * make conflicted files persistent
     * @param cfiles Conflicted files
     */
    void files(const list<string> cfiles);
    /**
     * load conflicted files
     * @return Conflicted files
     */
    list<string> files();

protected:
    ConflictPersistence();
    
    virtual cfg_opt_t *init_parser();
    virtual string get_persistence();
    virtual void read_values();

private:
    list<string> conflictfiles;
    
    static std::auto_ptr<ConflictPersistence> 
        theConflictPersistenceInstance;
    static Mutex m;

};

#endif
