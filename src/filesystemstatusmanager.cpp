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
	DBusMessage *message;
	string filter_string;

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
/////////////////////////////////////////////////////
	dbus_connection_read_write(bus, -1);      
	message = dbus_connection_pop_message(bus);
	dbus_message_unref(message);
   	while (1) {

      		// non blocking read of the next available message
	      	dbus_connection_read_write(bus, -1);
	      	message = dbus_connection_pop_message(bus);

	      	// loop again if we haven't read a message
	      	if (NULL == message) { 
        		sleep(1);
         		continue;
	      	} else {
			const char*  Msg_Path;
			char* senderid;
			Msg_Path=dbus_message_get_path(message);
			dbus_message_get_args (message, &error, DBUS_TYPE_STRING, &senderid,DBUS_TYPE_INVALID);
			FilesystemStatusManager::Instance().available=false;
		}

	    	DBusError error;

    		dbus_error_init (&error);

	      	dbus_message_unref(message);
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
