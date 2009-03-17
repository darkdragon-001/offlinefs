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
#include "file.h"
#include "filestatusmanager.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

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

bool ConflictManager::resolve(string relativePath, string direction)
{
    bool success = false;
    for(list<string>::iterator iter = conflictfiles.begin();
        iter != conflictfiles.end(); iter++)
    {
        if(*iter == relativePath)
        {
            success = true;
            break;
        }
    }
    
    if(!success)
        return false;
    
    File fileinfo = Filestatusmanager::Instance().give_me_file(relativePath);
    
    // use the local file
    if(direction == "local")
    {
        struct stat localinfo;
        struct stat remoteinfo;
        int res = lstat(fileinfo.get_cache_path().c_str(), &localinfo);
        if(res < 0 && errno != ENOENT)
            return false;
        if(res < 0)
        { // local file does not exist - remove remote one as well
            unlink(fileinfo.get_remote_path().c_str());
        }
        else
        { // copy local file to remote
            int res = lstat(fileinfo.get_remote_path().c_str(), &remoteinfo);
            // delete remote file if exist
            if(S_ISDIR(remoteinfo.st_mode))
                rmdir(fileinfo.get_remote_path().c_str());
            else
                unlink(fileinfo.get_remote_path().c_str());
            // copy file
            if ( S_ISREG ( localinfo.st_mode ) )
            {
                    int fdd = open ( fileinfo.get_remote_path().c_str(),
                                        O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU );
                    if ( fdd < 0 )
                            return false;
                    int fds = open ( fileinfo.get_cache_path().c_str(), O_RDONLY );
                    if ( fds < 0 )
                            return false;
                    char buf[1024];
                    ssize_t bytesread;
                    while ( ( bytesread = read ( fds, buf, sizeof ( buf ) ) ) > 0 )
                    {
                            if ( write ( fdd, buf, bytesread ) < 0 )
                                    return false;
                    }
                    if ( bytesread < 0 )
                            return false;
                    close ( fds );
                    close ( fdd );
            }
            else if ( S_ISLNK ( localinfo.st_mode ) )
            {
                    char buf[1024];
                    ssize_t len;
                    // create the new link
                    len = readlink ( fileinfo.get_cache_path().c_str(), buf, sizeof ( buf )-1 );
                    if ( len < 0 )
                        return false;
                    buf[len] = '\0';
                    if ( symlink ( buf, fileinfo.get_remote_path().c_str() ) < 0 )
                        return false;
            }

        }
    }
    // use remote file
    else if(direction == "remote")
    {
        struct stat localinfo;
        struct stat remoteinfo;
        int res = lstat(fileinfo.get_remote_path().c_str(), &remoteinfo);
        if(res < 0 && errno != ENOENT)
            return false;
        if(res < 0)
        { // remote file does not exist - remove local one as well
            unlink(fileinfo.get_cache_path().c_str());
        }
        else
        { // copy remote file to cache
            int res = lstat(fileinfo.get_cache_path().c_str(), &localinfo);
            // delete local file if exist
            if(S_ISDIR(localinfo.st_mode))
                rmdir(fileinfo.get_cache_path().c_str());
            else
                unlink(fileinfo.get_cache_path().c_str());
            // copy file
            if ( S_ISREG ( remoteinfo.st_mode ) )
            {
                    int fdd = open ( fileinfo.get_cache_path().c_str(),
                                        O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU );
                    if ( fdd < 0 )
                            return false;
                    int fds = open ( fileinfo.get_remote_path().c_str(), O_RDONLY );
                    if ( fds < 0 )
                            return false;
                    char buf[1024];
                    ssize_t bytesread;
                    while ( ( bytesread = read ( fds, buf, sizeof ( buf ) ) ) > 0 )
                    {
                            if ( write ( fdd, buf, bytesread ) < 0 )
                                    return false;
                    }
                    if ( bytesread < 0 )
                            return false;
                    close ( fds );
                    close ( fdd );
            }
            else if ( S_ISLNK ( remoteinfo.st_mode ) )
            {
                    char buf[1024];
                    ssize_t len;
                    // create the new link
                    len = readlink ( fileinfo.get_remote_path().c_str(), buf, sizeof ( buf )-1 );
                    if ( len < 0 )
                        return false;
                    buf[len] = '\0';
                    if ( symlink ( buf, fileinfo.get_cache_path().c_str() ) < 0 )
                        return false;
            }

        }
    }
    removeConflictFile(relativePath);
    return true;
}