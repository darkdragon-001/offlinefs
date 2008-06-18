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
#include <pthread.h>
#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <list>
using namespace std;

#include "filesystemstatusmanager.h"
#include "ofsenvironment.h"

std::auto_ptr<FilesystemStatusManager> FilesystemStatusManager::theFilesystemStatusManagerInstance;
Mutex FilesystemStatusManager::m;

FilesystemStatusManager::FilesystemStatusManager() : available(true) {}
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
		printf ("Failed to connect to the D-BUS daemon: %s", error.message);
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
