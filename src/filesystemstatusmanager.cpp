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
#if HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif /* HAVE_UMOUNT2 */
using namespace std;

#include "filesystemstatusmanager.h"
#include "ofsenvironment.h"
#include "ofsconf.h"
#include "ofshash.h"
#include "ofslog.h"
#include "lazywrite.h"

std::auto_ptr<FilesystemStatusManager> FilesystemStatusManager::theFilesystemStatusManagerInstance;
Mutex FilesystemStatusManager::m;

FilesystemStatusManager::FilesystemStatusManager() : available(true), sync(true) {}
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

	// TODO: Handle errors
	seteuid(0);

	//////////////////////////////////////////////////////////////////////////
	// MOUNT
	//////////////////////////////////////////////////////////////////////////
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
		remotemountpoint = shareremote; // FIXME: Has no effect here
	} else
		shareremote = sharepath;

	if(remotefstype != "file") { // FIXME: else missing!
		const char * remotemountpoint_c;

		OFSConf& conf = OFSConf::Instance();

		// set remote path and mount point
		remotemountpoint = conf.GetRemotePath()+"/"+ofs_hash(shareurl);
		remotemountpoint_c = remotemountpoint.c_str();

		// create mount point and check and if it exits check it is actually a directory
		struct stat s;
		int stat_result = stat(remotemountpoint_c, &s);
		if ((stat_result == 0) && !S_ISDIR(s.st_mode)) {
			throw OFSException(remotemountpoint + ": not a directory",
					ENOTDIR,
					true);
		}

		if (stat_result == -1 && errno == ENOENT) {
			if (mkdir(remotemountpoint_c, 0777) == -1) {
				throw OFSException("Failed to create " + remotemountpoint,
						errno,
						true);
			}
		}

		// Mount remote file system
		int childpid = fork();
		int status;
		if(childpid == 0) {
			execlp("mount",
					"mount",
					"-t", remotefstype.c_str(),
					shareremote.c_str(),
					remotemountpoint_c,
					(OFSEnvironment::Instance().getMountOptions().length() > 0 ? NULL : "-o"),
					OFSEnvironment::Instance().getMountOptions().c_str(),
					NULL);
			// TODO: Linux man does not specify return codes (on openSUSE 11.4)
			exit(1);
		}
		if(childpid < 0) {
			throw OFSException("fork failed",
					errno,
					true);
		}

		int childpid2 = wait(&status);
		int exitstatus = WEXITSTATUS(status);
// FIXME: mount exit status is not an errno value in Linux
		if(WIFEXITED(status) && exitstatus) {
			errno = exitstatus;
			throw OFSException("Mount command returned unsuccessfully",
					errno,
					true);
		}
		return;
	}
}


/*!
    \fn FilesystemStatusManager::unmountfs()
 */
void FilesystemStatusManager::unmountfs()
{	
	int status;

	// FIXME: Handle errors
	seteuid(0);

#if HAVE_UMOUNT2
	const char *target = OFSEnvironment::Instance().getRemotePath().c_str();
	status = umount2(target, MNT_FORCE);

	if (status == -1)
	{
		ofslog::error("Unable to unmount the remote file system!");
		ofslog::error(strerror(errno));
	}
	else
	{
		ofslog::debug("File system unmounted");
	}
#else
	int childpid = fork();
	if(childpid == 0) {
		execlp("umount",
				"umount", "-f", const_cast<char *>(OFSEnvironment::Instance().getRemotePath().c_str()), NULL);
		// TODO: Linux man does not specify return codes (on openSUSE 11.4)
		exit(1);
	}
	if(childpid < 0) {
		ofslog::error("Unable to unmount the remote file system!");
		ofslog::error(strerror(errno));
		return;
	}
	int childpid2 = wait(&status);
	int exitstatus = WEXITSTATUS(status);
	if(WIFEXITED(status) && exitstatus) {
		ofslog::error("Unable to unmount the remote file system!");
		ofslog::error(strerror(exitstatus)); // TODO: Interpret return codes properly
		errno = 0;
	}
	else
	{
		ofslog::debug("File system unmounted");
	}
#endif /* HAVE_UMOUNT2 */
// FIXME: handle errors
	seteuid(OFSEnvironment::Instance().getUid());
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
