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
#include "conflictlogger.h"
#include "ofsenvironment.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <string>

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
            throw OFSException(strerror(errno), errno);

        // Gets modification time of the cache file.
        if (lstat(fileInfo.get_cache_path().c_str(), &fileinfo_cache) < 0)
            throw OFSException(strerror(errno), errno);

        // utime can not be used with symbolic links because there
        // is no possibility to prevent it from following the link
        if (!S_ISLNK(fileinfo_remote.st_mode))
        {
//            timesRemote.actime = fileinfo_remote.st_atime;
            timesRemote = fileinfo_remote.st_mtime;
//            timesCache.actime = fileinfo_cache.st_atime;
            timesCache = fileinfo_cache.st_mtime;
            if (timesRemote > timesCache)
                return changed_on_server;
        }
    }
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
                throw OFSException(strerror(errno), errno);
        }
    }
}

void SynchronizationManager::persist() const
{
}

void SynchronizationManager::reinstate()
{
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
	for (list<SyncLogEntry>::iterator it = listOfEntries.begin(); it != listOfEntries.end(); it++)
//	const int nCount = (int)listOfEntries.size();
//	for (int i = 0; i < nCount; i++)
	{
		// TODO: Reintegrate the file.
		SyncLogEntry& sle = *it;
//		OFSFile file(sle.GetFilePath());
		File fileInfo(Filestatusmanager::Instance().give_me_file(sle.GetFilePath().c_str()));
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
		assert(false);
	}
	else if (nRet < 0)
	{
		throw OFSException(strerror(errno), errno);
	}

	// get info of remote file
	nRet = lstat(fileInfo.get_remote_path().c_str(), &fsRemote);
	if (nRet < 0 && errno == ENOENT)
	{
		// Creates the file only if it doesn't already exist.
		if (S_ISDIR(fsCache.st_mode))
		{
			if (mkdir(fileInfo.get_cache_path().c_str(), S_IRWXU) < 0)
				throw OFSException(strerror(errno), errno);
		}
		else if (S_ISREG(fsCache.st_mode))
		{
			int fdr = open(fileInfo.get_remote_path().c_str(),
				O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
			if (fdr < 0)
				throw OFSException(strerror(errno), errno);
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
				throw OFSException(strerror(errno), errno);
			buf[len] = '\0';
			if (symlink(buf, fileInfo.get_remote_path().c_str()) < 0)
				throw OFSException(strerror(errno), errno);
		} // TODO: Other file types

		// set atime and mtime
		//update_amtime();
	}
	else if (S_ISDIR(fsCache.st_mode) != S_ISDIR(fsRemote.st_mode) || S_ISREG(fsCache.st_mode) != S_ISREG(fsRemote.st_mode) || S_ISLNK(fsCache.st_mode) != S_ISLNK(fsRemote.st_mode))
	{
		// Conflict!!!
		// Sends a signal: File type mismatch.
		OFSBroadcast::Instance().SendSignal("Conflict", "RemoteAndCacheTypeMismatch", 0);
		ConflictLogger::Instance().AddEntry(OFSEnvironment::Instance().getShareID().c_str(), fileInfo.get_relative_path().c_str(), 't');
		return 1;
	}
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
		assert(false);
	}
	else if (nRet < 0)
	{
		throw OFSException(strerror(errno), errno);
	}

	// get info of remote file
	nRet = lstat(fileInfo.get_remote_path().c_str(), &fsRemote);
	if (nRet < 0 && errno == ENOENT)
	{
		// Conflict!!!
		// Sends a signal: Modified file has been deleted on remote.
		OFSBroadcast::Instance().SendSignal("Conflict", "ModifiedFileHasBeenDeleted", 0);
		ConflictLogger::Instance().AddEntry(OFSEnvironment::Instance().getShareID().c_str(), fileInfo.get_relative_path().c_str(), 'd');
		return 1;
	}
	else
	{
		if (has_been_modified(fileInfo) == changed_on_server)
		{
			// Conflict!!!
			// Sends a signal: Modified file has been modified on remote.
			OFSBroadcast::Instance().SendSignal("Conflict", "ModifiedFileHasBeenModified", 0);
			ConflictLogger::Instance().AddEntry(OFSEnvironment::Instance().getShareID().c_str(), fileInfo.get_relative_path().c_str(), 'c');
			return 2;
		}
		else
		{
			if (S_ISDIR(fsCache.st_mode))
			{
				// TODO: Copy attributes
			}
			else if (S_ISREG(fsCache.st_mode))
			{
				int fdr = open(fileInfo.get_remote_path().c_str(),
					O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
				if (fdr < 0)
					throw OFSException(strerror(errno), errno);
				int fdl = open(fileInfo.get_cache_path().c_str(), O_RDONLY);
				if (fdl < 0)
					throw OFSException(strerror(errno), errno);
				char szBuf[1024];
				ssize_t nBytesRead;
				while((nBytesRead = read(fdr, szBuf, sizeof(szBuf))) > 0)
				{
					if (write(fdr, szBuf, nBytesRead) < 0)
						throw OFSException(strerror(errno), errno);
				}
				if (nBytesRead < 0)
					throw new OFSException(strerror(errno), errno);
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
				len = readlink(fileInfo.get_cache_path().c_str(), buf, sizeof(buf)-1);
				if (len < 0)
					throw OFSException(strerror(errno), errno);
				buf[len] = '\0';
				if (symlink(buf, fileInfo.get_remote_path().c_str()) < 0)
					throw OFSException(strerror(errno), errno);
			} // TODO: Other file types
	
			// set atime and mtime
			//update_amtime();
		}
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
			OFSBroadcast::Instance().SendSignal("Conflict", "FileToDeleteHasBeenModified", 0);
			ConflictLogger::Instance().AddEntry(OFSEnvironment::Instance().getShareID().c_str(), fileInfo.get_relative_path().c_str(), 'm');
		}
		else
		{
			try {
				int res = rmdir(fileInfo.get_remote_path().c_str());
				if (res == -1)
					return -errno;
//				update_amtime();
				return 0;
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
