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
#ifndef SYNCHRONIZATIONMANAGER_H
#define SYNCHRONIZATIONMANAGER_H
#include "file_sync.h"
#include "file.h"
#include "syncstatetype.h"
#include "persistable.h"
#include "mutexlocker.h"
#include <iostream>
#include <map>
#include <string>
using namespace std;
/**
	@author Carsten Kolassa <Carsten@Kolassa.de>,Frank Gsellmann <frank.gsellmann@gmx.de>
*/
class SynchronizationManager:public persistable{
public:
    static SynchronizationManager& Instance();
    /**
     * Determines if the file has been modified on the server
     * @param path 
     * @return 
     */
    syncstate has_been_modified(string path);
    /**
     * Determines if the file has been deleted on the server
     * @param path 
     * @return 
     */
    syncstate has_been_deleted(string path);
    /**
     * Determines the local modification state
     * @param path 
     * @return 
     */
    syncstate store_state(string path);
    ~SynchronizationManager();
    /**
     * Writes the local states to the disk
     */
    virtual void persist() const;
    /**
     * Reads the local states from the disk
     */
    virtual void reinstate() const;
protected:
    SynchronizationManager();
private:
    static map<string,file_sync> filesmod;
    static std::auto_ptr<SynchronizationManager> theSynchronizationManagerInstance;
    static Mutex m_mutex;
};

#endif
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
#ifndef SYNCHRONIZATIONMANAGER_H
#define SYNCHRONIZATIONMANAGER_H
#include "file_sync.h"
#include "file.h"
#include "syncstatetype.h"
#include "persistable.h"
#include "mutexlocker.h"
#include <iostream>
#include <map>
#include <string>
using namespace std;
/**
	@author Carsten Kolassa <Carsten@Kolassa.de>,Frank Gsellmann <frank.gsellmann@gmx.de>
*/
class SynchronizationManager:public persistable{
public:
    static SynchronizationManager& Instance();
    /**
     * Determines if the file has been modified on the server
     * @param path 
     * @return 
     */
    syncstate has_been_modified(string path);
    /**
     * Determines if the file has been deleted on the server
     * @param path 
     * @return 
     */
    syncstate has_been_deleted(string path);
    /**
     * Determines the local modification state
     * @param path 
     * @return 
     */
    syncstate store_state(string path);
    ~SynchronizationManager();
    /**
     * Writes the local states to the disk
     */
    virtual void persist() const;
    /**
     * Reads the local states from the disk
     */
    virtual void reinstate() const;
protected:
    SynchronizationManager();
private:
    static map<string,file_sync> filesmod;
    static std::auto_ptr<SynchronizationManager> theSynchronizationManagerInstance;
    static Mutex m_mutex;
};

#endif
