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
#include "ofsexception.h"
#include "synclogger.h"
#include "filestatusmanager.h"
#include "synclogentry.h"
#include "ofsbroadcast.h"
#include "conflictmanager.h"
#include "ofsenvironment.h"
#include "synchronizationpersistence.h"
#include "ofsfile.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <string>
#include <cstring>

// Initializes the class attributes.
std::auto_ptr<SynchronizationManager> SynchronizationManager::theSynchronizationManagerInstance;
Mutex SynchronizationManager::m_mutex;

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION/ DESTRUCTION
//////////////////////////////////////////////////////////////////////////////

SynchronizationManager::SynchronizationManager()
{
    reinstate();
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

//syncstate SynchronizationManager::has_been_modified(string path)
syncstate SynchronizationManager::has_been_modified(const File& fileInfo)
{
    if (fileInfo.get_offline_state() && fileInfo.get_availability())
    {
        struct stat fileinfo_remote;
        struct stat fileinfo_cache;
        time_t timesRemote;
        time_t timesCache;

        // Gets modification time of the remote file.
        if (lstat(fileInfo.get_remote_path().c_str(), &fileinfo_remote) < 0)
            return deleted_on_server;
        timesRemote = fileinfo_remote.st_mtime;

        // Fetch saved modification time, or the mtime of the local file is there is none
        timesCache = SynchronizationManager::Instance().getmtime(fileInfo.get_relative_path());
        if(timesCache == 0)
        {
            if (lstat(fileInfo.get_cache_path().c_str(), &fileinfo_cache) < 0)
                return no_state_avail;
            timesCache = fileinfo_cache.st_mtime;
        }

        // utime can not be used with symbolic links because there
        // is no possibility to prevent it from following the link
        if (!S_ISLNK(fileinfo_remote.st_mode))
        {
            if (timesRemote > timesCache)
                return changed_on_server;
        }
    }
    return not_changed;
}

//syncstate SynchronizationManager::has_been_deleted(string path)
syncstate SynchronizationManager::has_been_deleted(const File& fileInfo)
{
    if (fileInfo.get_offline_state() && fileInfo.get_availability())
    {
        struct stat fileinfo_remote;
        time_t times;

        if (lstat(fileInfo.get_remote_path().c_str(), &fileinfo_remote) < 0)
        {
            if (errno == ENOENT)
                return deleted_on_server;
            else
                throw OFSException(strerror(errno), errno,true);
        }
    }
    // FIXME Some cases not covered.
    throw OFSException("Missing error handling in SynchronizationManager::has_been_deleted",0,true);
}

void SynchronizationManager::persist() const
{
    SynchronizationPersistence::Instance().mtimes(mtimes);
}

void SynchronizationManager::reinstate()
{
    mtimes = SynchronizationPersistence::Instance().mtimes();
}


void SynchronizationManager::ReintegrateFile(const char* pszHash, const string& strFilePath)
{
	ReintegrateFiles(pszHash, SyncLogger::Instance().GetEntries(pszHash, strFilePath));
}

void SynchronizationManager::ReintegrateAll(const char* pszHash)
{
	ReintegrateFiles(pszHash, SyncLogger::Instance().GetEntries(pszHash, ""));
}

void SynchronizationManager::ReintegrateFiles(const char* pszHash, list<SyncLogEntry> listOfEntries)
{
	bool bOK;
	for (list<SyncLogEntry>::iterator it = listOfEntries.begin();
	   it != listOfEntries.end(); it++)
//	const int nCount = (int)listOfEntries.size();
//	for (int i = 0; i < nCount; i++)
	{
		// TODO: Reintegrate the file.
		SyncLogEntry& sle = *it;
//		OFSFile file(sle.GetFilePath());
		File fileInfo ( Filestatusmanager::Instance()
		                  .give_me_file(sle.GetFilePath().c_str())
                              );
		switch (sle.GetModType())
		{
		case 'c':
			CreateFile(fileInfo);
			bOK = true; ///\todo What's that?
			break;
		case 'm':
			ModifyFile(fileInfo);
			bOK = true; ///\todo What's that?
			break;
		case 'd':
			DeleteFile(fileInfo);
			bOK = true; ///\todo What's that?
			break;
		}
		if (bOK)
		{
                    removemtime(fileInfo.get_relative_path());
                    SyncLogger::Instance().RemoveEntry(pszHash, sle);
		}
	}
}

int SynchronizationManager::CreateFile(const File& fileInfo)
{
	if (!fileInfo.get_availability() || !fileInfo.get_offline_state())
		return 0;	// Nothing to do

	struct stat fsCache;
	struct stat fsRemote;
	int nRet;

	// receive file information
	nRet = lstat(fileInfo.get_cache_path().c_str(), &fsCache);
	if (nRet < 0 && errno == ENOENT)
	{
	   // file does not exist - nothing to do
	   return 0;
	}
	else if (nRet < 0)
	{
	   throw OFSException(strerror(errno), errno,true);
	}

	// get info of remote file
	nRet = lstat(fileInfo.get_remote_path().c_str(), &fsRemote);
	if (nRet < 0 && errno == ENOENT)
	{
            // Creates the file only if it doesn't already exist.
            if (S_ISDIR(fsCache.st_mode))
            {
                if (mkdir(fileInfo.get_remote_path().c_str(), S_IRWXU) < 0)
                    throw OFSException(strerror(errno), errno,true);
            }
            else if (S_ISREG(fsCache.st_mode))
            {
                int fdr = open(fileInfo.get_remote_path().c_str(),
                    O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
                if (fdr < 0)
                    throw OFSException(strerror(errno), errno,true);
                close(fdr);
            }
            else if (S_ISLNK(fsCache.st_mode))
            {
                // TODO: Could be buggy!!!
                char buf[1024];
                ssize_t len;
		// remove the old link if it exists
                unlink(fileInfo.get_remote_path().c_str());
                errno = 0;
                // create the new link
                len = readlink(fileInfo.get_cache_path().c_str(), buf, sizeof(buf)-1);
                if (len < 0)
                    throw OFSException(strerror(errno), errno,true);
                buf[len] = '\0';
                if (symlink(buf, fileInfo.get_remote_path().c_str()) < 0)
                    throw OFSException(strerror(errno), errno,true);
            } // TODO: Other file types

            // set atime and mtime
            //update_amtime();
            OFSFile(fileInfo.get_relative_path()).update_amtime();
	}
	// if file has been created at the meantime and
	// local as well as remote file are directories, we can merge them
	// otherwise -> conflict
	else if (!S_ISDIR(fsCache.st_mode) || !S_ISDIR(fsRemote.st_mode))
	{
		// Conflict!!!
		// Sends a signal: File type mismatch.
		OFSBroadcast::Instance().SendError("Conflict",
		  "RemoteAndCacheTypeMismatch","Conflict: File type mismatch.", 0);
		ConflictManager::Instance().addConflictFile(fileInfo.get_relative_path());
		return 1;
	} // else both have created directories -> we can merge
	// FIXME Return code for "merge"
	return 2;
}

int SynchronizationManager::ModifyFile(const File& fileInfo)
{
	if (!fileInfo.get_availability() || !fileInfo.get_offline_state())
		return 0;	// Nothing to do

	struct stat fsCache;
	struct stat fsRemote;
	int nRet;

	// receive file information
	nRet = lstat(fileInfo.get_cache_path().c_str(), &fsCache);
	if (nRet < 0 && errno == ENOENT)
	{
	   // file does not exist - nothing to do
	   return 0;
	}
	else if (nRet < 0)
	{
		throw OFSException(strerror(errno), errno,true);
	}

	// get info of remote file
	nRet = lstat(fileInfo.get_remote_path().c_str(), &fsRemote);
	if (nRet < 0 && errno == ENOENT)
	{ // remote file has been deleted
		// Conflict!!!
		// Sends a signal: Modified file has been deleted on remote.
		OFSBroadcast::Instance().SendError("Conflict",
		  "ModifiedFileHasBeenDeleted","Conflict: Modified file has been deleted on remote.",0);
		ConflictManager::Instance().addConflictFile(fileInfo.get_relative_path());
		return 1;
	}
	else
	{
		if (has_been_modified(fileInfo) == changed_on_server)
		{
			// Conflict!!!
			// Sends a signal: Modified file has been modified on remote.
			OFSBroadcast::Instance().SendError("Conflict",
			 "ModifiedFileHasBeenModified","Conflict: Modified file has been modified on remote.",0);
			ConflictManager::Instance().addConflictFile(fileInfo.get_relative_path());
			return 2;
		}
		else
		{ // everything OK - reintegrate
			if (S_ISDIR(fsCache.st_mode))
			{
				// TODO: Copy attributes
			}
			else if (S_ISREG(fsCache.st_mode))
			{
				int fdr = open(fileInfo.get_remote_path().c_str(),
					O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
				if (fdr < 0)
					throw OFSException(strerror(errno), errno,true);
				int fdl = open(fileInfo.get_cache_path().c_str(),
				 O_RDONLY);
				if (fdl < 0)
					throw OFSException(strerror(errno), errno,true);
				char szBuf[1024];
				ssize_t nBytesRead;
				while((nBytesRead = read(fdl, szBuf, sizeof(szBuf))) > 0)
				{
					if (write(fdr, szBuf, nBytesRead) < 0)
						throw OFSException(strerror(errno),
						errno,true);
				}
				if (nBytesRead < 0)
					throw new OFSException(strerror(errno), errno,true);
				close(fdl);
				close(fdr);
			}
			else if (S_ISLNK(fsCache.st_mode))
			{
				// TODO: Could be buggy!!!
				char buf[1024];
				ssize_t len;
				// remove the old link if it exists
				unlink(fileInfo.get_remote_path().c_str());
				errno = 0;
				// create the new link
				len = readlink(fileInfo.get_cache_path().c_str(), buf,
				 sizeof(buf)-1);
				if (len < 0)
					throw OFSException(strerror(errno), errno,true);
				buf[len] = '\0';
				if (symlink(buf, fileInfo.get_remote_path().c_str()) < 0)
					throw OFSException(strerror(errno), errno,true);
			} // TODO: Other file types

			// set atime and mtime
			//update_amtime();
		}
        	OFSFile(fileInfo.get_relative_path()).update_amtime();
	}

	return 0;
}

int SynchronizationManager::DeleteFile(const File& fileInfo)
{
	if (!fileInfo.get_availability() || !fileInfo.get_offline_state())
		return 0;	// Nothing to do

	struct stat fsRemote;
	int nRet;

	// get info of remote file
	nRet = lstat(fileInfo.get_remote_path().c_str(), &fsRemote);
	// Deletes the file only if it hasn't already been deleted.
	if (nRet >= 0)
	{
		if (has_been_modified(fileInfo) == changed_on_server)
		{
			// Conflict!!!
			// Sends a signal: Couldn't delete a file that has been modified on the remote.
			OFSBroadcast::Instance().SendError("Conflict", "FileToDeleteHasBeenModified",
				       "Conflict: Could not delete a file that has been modified on the remote.",0);
			ConflictManager::Instance().addConflictFile(fileInfo.get_relative_path());
			return 1;
		}
		else
		{
			try {
                            if(S_ISDIR(fsRemote.st_mode))
                            {
                                int res = rmdir(fileInfo.get_remote_path().c_str());
                                if (res == -1)
                                    return -errno;
//				update_amtime();
                            }
                            else
                            {
                                int res = unlink(fileInfo.get_remote_path().c_str());
                                if (res == -1)
                                    return -errno;
                            }
			}
			catch (OFSException& e)
			{
				errno = e.get_posixerrno();
				return -errno;
			}
		}
	}
	return 0;
}

void SynchronizationManager::addmtime(string path, time_t mtime)
{
    if(getmtime(path) == 0)
        mtimes[path] = mtime;
    persist();
}

time_t SynchronizationManager::getmtime(string path)
{
    map<string, time_t>::iterator iter = mtimes.find(path);
    if(iter == mtimes.end())
        return 0;
    return iter->second;
}

void SynchronizationManager::removemtime(string path)
{
    map<string, time_t>::iterator iter = mtimes.find(path);
    if(iter != mtimes.end())
    {
        mtimes.erase(iter);
        persist();
    }
}
