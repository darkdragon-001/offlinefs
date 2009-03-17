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
#ifndef FILESYSTEMSTATUSMANAGER_H
#define FILESYSTEMSTATUSMANAGER_H
#include "mutex.h"
#include "mutexlocker.h"
#include "synchronizationmanager.h"
#include "ofsenvironment.h"
//#include "filesystem.h"
#include <string>
#include <memory>

#define TESTING_REMOTE_PATH "/usr/bin"
#define TESTING_BACKING_PATH "/tmp/ofsbacking"

/**
	@author Carsten Kolassa <Carsten@Kolassa.de>,
		Tobias Jaehnel <tjaehnel@gmail.com>
	This Class monitors the availability of the mounted fielsystem.
	Note that this class only contains hacks and has to be rewritten.
*/
class FilesystemStatusManager{
public:
    /**
     * Get the singleton instange
     * @return The singleton instance
     */
    static FilesystemStatusManager& Instance();
    /**
     * 
     */
    ~FilesystemStatusManager(); 
    /**
     * Is the remote share currently available or are we offline?
     * @return true if the share is available, false otherwise
     */
    bool isAvailable();
    /**
     * 
     */
    void filesystemError();
    /**
     * Runs the Dbus listener which monitors the network connection
     * in the background
     */
    void startDbusListener();
    /**
     * Monitors the network connection via DBUS
     * @param 
     * @return 
     */
    static void *DbusListenerRun(void *);
    /**
     * Get the directory, the remote share is mounted to
     * @return Mountpoint of remote share
     */
    string getRemoteMountpoint();
    /**
     * Get the location of the given file or directory in the
     * mounted remote share
     * @param path the path, relative to the share root
     * @return the absolute path to the file below the mountpoint
     */
    string getRemote(string path);
    
    void setAvailability(bool value);
    void unmountfs();
    void mountfs();
protected:
    FilesystemStatusManager();
  private:
    static std::auto_ptr<FilesystemStatusManager> 
		theFilesystemStatusManagerInstance;

protected:
    bool available;
    static Mutex m; 
};
#endif
