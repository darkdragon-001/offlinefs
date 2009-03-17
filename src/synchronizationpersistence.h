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
#ifndef SYNCHRONIZATIONPERSISTENCE_H
#define SYNCHRONIZATIONPERSISTENCE_H
#include "persistencemanager.h"
#include "mutexlocker.h"
#include <map>
#include <memory>
using namespace std;

#define CONFIGKEY_MTIMES "mtimes"
#define PERSISTENCE_MODULE_NAME "synchronization"

/**
	@author Tobias Jaehnel <tjaehnel@gmail.com>
*/
class SynchronizationPersistence : public PersistenceManager {
public:
    /**
     * Get singleton instance
     * @return singleton instance
     */
    static SynchronizationPersistence& Instance();
    ~SynchronizationPersistence();
    /**
     * make modification times persistent
     * @param trees list of modification times
     */
    void mtimes(const map<string, time_t> modtimes);
    /**
     * load modification times
     * @return list of modification times per file
     */
    map<string,time_t> mtimes();
protected:
    SynchronizationPersistence();
    virtual cfg_opt_t * init_parser();
    virtual string get_persistence();
    virtual void read_values();

private:
    map<string,time_t> mtimemap;

    static std::auto_ptr<SynchronizationPersistence> 
        theSynchronizationPersistenceInstance;
    static Mutex m;


};

#endif
