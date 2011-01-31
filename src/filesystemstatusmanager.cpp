/***************************************************************************
 *   Copyright (C) 2007 by Carsten Kolassa   *
 *   Carsten@Kolassa.de   *
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <pthread.h>
#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <list>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;

#include "filesystemstatusmanager.h"
#include "ofsenvironment.h"
#include "ofsconf.h"
#include "ofshash.h"
#include "ofslog.h"
#include "lazywrite.h"

std::auto_ptr<FilesystemStatusManager> FilesystemStatusManager::theFilesystemStatusManagerInstance;
Mutex FilesystemStatusManager::m;

FilesystemStatusManager::FilesystemStatusManager() : available(true) {sync=true;}
FilesystemStatusManager::~FilesystemStatusManager(){}
FilesystemStatusManager& FilesystemStatusManager::Instance()
{
    MutexLocker obtain_lock(m);
    if (theFilesystemStatusManagerInstance.get() == 0)
      theFilesystemStatusManagerInstance.reset(new FilesystemStatusManager);
    return *theFilesystemStatusManagerInstance;
}



/*!
    \fn FilesystemStatusManager::isAvailable()
    Is the current remote filesystem available?
 */
bool FilesystemStatusManager::isAvailable()
{
	return available;
}

bool FilesystemStatusManager::issync()
{
	return sync;
}

/*!
    \fn FilesystemStatusManager::filesystemError()
 */
void FilesystemStatusManager::filesystemError()
{
	available=false;
}

/*!
    \fn FilesystemStatusManager::startDbusListener()
 */
void FilesystemStatusManager::startDbusListener()
{
	pthread_t *thread = new pthread_t;
	pthread_create(thread, NULL, FilesystemStatusManager::DbusListenerRun, NULL);
}

/*!
    \fn FilesystemStatusManager::DbusListenerRun
 */
void *FilesystemStatusManager::DbusListenerRun(void *)
{
	DBusConnection *bus;
	DBusError error;
	DBusMessage *msg;
	string filter_string;
        list<string> interfacelist;
        const char* device_obj;
        OFSEnvironment &ofeitp = OFSEnvironment::Instance();
	interfacelist=ofeitp.getListenDevices();
	dbus_error_init (&error);
	bus = dbus_bus_get (DBUS_BUS_SYSTEM, &error);
	if (!bus) {
		ofslog::error("Failed to connect to the D-BUS daemon: %s", error.message);
		dbus_error_free (&error);
		pthread_exit(NULL);
		return NULL;
	}
	filter_string=string("type='signal',path=/org/freedesktop/NetworkManager,interface=org.freedesktop.NetworkManager,member=DeviceNoLongerActive");
	
	dbus_bus_add_match (bus, filter_string.c_str(), NULL);
	while (1) {
		dbus_error_init (&error);
        	dbus_connection_read_write(bus, -1);
        	msg = dbus_connection_pop_message(bus);

      
      		if (NULL == msg) { 
         		sleep(1);
         		continue;
      		}
		else{
			if(dbus_message_is_signal(msg,"org.freedesktop.NetworkManager","DeviceNoLongerActive")){
				dbus_message_get_args (msg, &error,DBUS_TYPE_OBJECT_PATH, &device_obj,DBUS_TYPE_INVALID);
                                for (list<string>::iterator it=interfacelist.begin();it!=interfacelist.end();++it){
				string Netpath= "/org/freedesktop/NetworkManager/Devices/"+ (*it);
                                if(Netpath==device_obj){
					FilesystemStatusManager::Instance().available=false;
				}
}
     
				
			
		}
}
    dbus_message_unref(msg);
   }
	pthread_exit(NULL);
	return NULL;
}

string FilesystemStatusManager::getRemoteMountpoint()
{
	return OFSEnvironment::Instance().getRemotePath();
}

string FilesystemStatusManager::getRemote(string path)
{
	return OFSEnvironment::Instance().getRemotePath()+path;
}


/*!
    \fn FilesystemStatusManager::mountfs()
 */
void FilesystemStatusManager::mountfs()
{
    string shareurl = OFSEnvironment::Instance().getShareURL();
    ofslog::debug("shareURL: %s", shareurl.c_str());
    string sharepath;
    string remotefstype;
    string shareremote;
    string remotemountpoint;
    
    //////////////////////////////////////////////////////////////////////////
    // MOUNT
    //////////////////////////////////////////////////////////////////////////

    char* pMountArgumente[8];
    pMountArgumente[0] = new char[6];
    strncpy(pMountArgumente[0], "mount", 6);
    pMountArgumente[1] = new char[3];
    strncpy(pMountArgumente[1], "-t", 3);
    pMountArgumente[2] = NULL;
    pMountArgumente[3] = NULL;
    pMountArgumente[4] = NULL;
    pMountArgumente[5] = new char[3];
    strncpy(pMountArgumente[5], "-o", 3);
    pMountArgumente[6] = NULL;
    pMountArgumente[7] = NULL; // terminator

    // Ermittelt die Remote-Pfade.
    //cout << argv[0] << endl;
    const char* cshareurl = shareurl.c_str();
    const char* pchDoppelPunktPos = strchr(cshareurl, ':');
    assert(pchDoppelPunktPos != NULL);
    int nDoppelPunktIndex = int (pchDoppelPunktPos - cshareurl);

    remotefstype = shareurl.substr(0,nDoppelPunktIndex);
    sharepath = shareurl.substr(nDoppelPunktIndex+3);
    // handle special protocols
    if(remotefstype == "smb" || remotefstype == "smbfs" || remotefstype == "cifs")
        shareremote = string("//") + sharepath;
    else if(remotefstype == "file") {
	shareremote = string("/") + sharepath;
	remotemountpoint = shareremote;
    } else
        shareremote = sharepath;

    char* pArgumente[8];

    if(remotefstype != "file") {
    	// Legt das Dateisystem fest.
    	pMountArgumente[2] = (char*)remotefstype.c_str();

    	// Oeffnet die Konfigurationsdatei.
    	OFSConf& conf = OFSConf::Instance();
    	//conf.ParseFile(); //obsolete

    	// Legt die Server-Share fest.
    	pMountArgumente[3] = (char*)shareremote.c_str();
    	remotemountpoint = conf.GetRemotePath()+"/"+ofs_hash(shareurl);
    	pMountArgumente[4] = (char*)remotemountpoint.c_str();

    	//    my_options(argc - 2, &argv[2], &pszOptions);
    	if(OFSEnvironment::Instance().getMountOptions().length() > 0)
    	{
    		pMountArgumente[6] = (char *)OFSEnvironment::Instance().getMountOptions().c_str();
    	}
    	else
    	{
    		pMountArgumente[5] = NULL;
    	}
    	// filesystem options
	//    pArgumente[2] = szOptions;
    
    	// Mountet die Share, die vom Benutzer übergeben wurde.
    	int childpid = fork();
    	int status;
        ofslog::info("Mounting filesystem");
    	if(childpid == 0) {
    		/*cout << endl << "mount" << " ";
		cout << pMountArgumente[1] << " ";
    		cout << pMountArgumente[2] << " ";
    		cout << pMountArgumente[3] << " ";
    		cout << pMountArgumente[4] << " ";
    		cout << pMountArgumente[5] << " ";
    		cout << pMountArgumente[6] << " ";
    		cout << endl;*/
    		// make the mount point and ignore errors of it does exits
       		//ofslog::debug("mkdir %s", pMountArgumente[4]);
       		mkdir(pMountArgumente[4], 0777);
       		/*ofslog::debug("mount %s %s %s %s %s %s %s",
       				pMountArgumente[0], pMountArgumente[1], pMountArgumente[2],
       				pMountArgumente[3], pMountArgumente[4], pMountArgumente[5],
       				pMountArgumente[6]);*/
       		execvp("mount", pMountArgumente);
       		ofslog::error(strerror(errno));
       		return;
    	}
    	if(childpid < 0) {
    		ofslog::error(strerror(errno));
        	return;
    	}
    	//waitpid(childpid, &status, 0);
    	int childpid2 = wait(&status);
	//    cout << status << " - (" << childpid << "/" << childpid2 << ") - " << WEXITSTATUS(status) << endl;
    	int exitstatus = WEXITSTATUS(status);
    	ofslog::debug("Mount finished");
    	if(WIFEXITED(status) && exitstatus) {
       		errno = exitstatus;
    		ofslog::error("Unable to mount the remote filesystem!");
    		ofslog::error(strerror(exitstatus));
       		return;
    	}
    
    }
}


/*!
    \fn FilesystemStatusManager::unmountfs()
 */
void FilesystemStatusManager::unmountfs()
{	
	int status;
	char *arguments[4];
	arguments[0] = new char[7];
	strncpy(arguments[0], "umount", 7);
	arguments[1] = new char[3];
	strncpy(arguments[1], "-f", 3);
	arguments[2] = (char *)OFSEnvironment::Instance().getRemotePath().c_str();
	arguments[3] = NULL;
	int childpid = fork();
	if(childpid == 0) {
		execvp(arguments[0], arguments);
		errno = 0;
		return;
	}
	if(childpid < 0) {
		perror(strerror(errno));
		errno = 0;
		return;
	}
	int childpid2 = wait(&status);
	int exitstatus = WEXITSTATUS(status);
	if(WIFEXITED(status) && exitstatus) {
		ofslog::error("Unable to unmount the remote filesystem!");
		ofslog::error(strerror(exitstatus));
		errno = 0;
	}
	else
	{
	    ofslog::debug("Filesystem unmounted");
	}
}

void FilesystemStatusManager::setAvailability(bool value)
{
    if(available != value)
    {
        available = value;
        if(available)
        { // mount share and reintegrate
            mountfs();
            SynchronizationManager::Instance().ReintegrateAll(
                OFSEnvironment::Instance().getShareID().c_str());
            //Remote==Cache
            sync=true;
        }
        else
        { // unmount fs
            unmountfs();
        }
    }
}

void FilesystemStatusManager::setsync(bool value)
{
	sync=value;
}
