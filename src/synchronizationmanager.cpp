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
#include "synchronizationmanager.h"

// Initializes the class attributes.
std::auto_ptr<SynchronizationManager> SynchronizationManager::theSynchronizationManagerInstance;
Mutex SynchronizationManager::m_mutex;

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION/ DESTRUCTION
//////////////////////////////////////////////////////////////////////////////

SynchronizationManager::SynchronizationManager()
{
}

SynchronizationManager::~SynchronizationManager()
{
}

SynchronizationManager& SynchronizationManager::Instance()
{
    MutexLocker obtainLock(m_mutex);
    if (theSynchronizationManagerInstance.get() == 0)
    {
        theSynchronizationManagerInstance.reset(new SynchronizationManager);
    }

    return *theSynchronizationManagerInstance;
}

syncstate SynchronizationManager::has_been_modified(string path)
{
    if (get_offline_state() && get_availability())
    {
        struct stat fileinfo_remote;
        struct stat fileinfo_cache;
        struct utimbuf timesRemote;
        struct utimbuf timesCache;

        // Gets modification time of the remote file.
        if (lstat(get_remote_path().c_str(), &fileinfo_remote) < 0)
            throw OFSException(strerror(errno), errno);

        // Gets modification time of the cache file.
        if (lstat(get_cache_path().c_str(), &fileinfo_cache) < 0)
            throw OFSException(strerror(errno), errno);

        // utime can not be used with symbolic links because there
        // is no possibility to prevent it from following the link
        if (!S_ISLNK(fileinfo_remote.st_mode))
        {
//            timesRemote.actime = fileinfo_remote.st_atime;
            timesRemote.modtime = fileinfo_remote.st_mtime;
//            timesCache.actime = fileinfo_cache.st_atime;
            timesCache.modtime = fileinfo_cache.st_mtime;
            if (timesRemote.modtime > timesCache.modtime)
                return changed_on_server;
        }
    }
}

syncstate SynchronizationManager::has_been_deleted(string path)
{
    if (get_offline_state() && get_availability())
    {
        struct stat fileinfo_remote;
        struct utimbuf times;

        if (lstat(get_remote_path().c_str(), &fileinfo_remote) < 0)
        {
            if (errno == ENOENT)
                return deleted_on_server;
            else
                throw OFSException(strerror(errno), errno);
        }
    }
}

void SynchronizationManager::persist() const
{
}

void SynchronizationManager::reinstate() const
{
}


