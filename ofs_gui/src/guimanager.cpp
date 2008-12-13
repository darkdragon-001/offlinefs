/***************************************************************************
 *   Copyright (C) 2008 by Frank Gsellmann   *
 *   frank@frank-laptop   *
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
#ifdef BRATWURST

#include "guimanager.h"

// Initializes the class attributes.
std::auto_ptr<GUIManager> GUIManager::theGUIManagerInstance;
Mutex GUIManager::m_mutex;

//////////////////////////////////////////////////////////////////////////////
// CONSTRUCTION/ DESTRUCTION
//////////////////////////////////////////////////////////////////////////////

GUIManager::GUIManager()
{
}

GUIManager::~GUIManager()
{
}

GUIManager& GUIManager::Instance()
{
    MutexLocker obtainLock(m_mutex);
    if (theGUIManagerInstance.get() == 0)
    {
        theGUIManagerInstance.reset(new GUIManager);
    }

    return *theGUIManagerInstance;
}

/*!
    \fn FilesystemStatusManager::StartDbusListener()
 */
void GUIManager::StartDbusListener()
{
    pthread_t *thread = new pthread_t;
    pthread_create(thread, NULL, GUIManager::DbusListenerRun, NULL);
}

/*!
    \fn FilesystemStatusManager::DbusListenerRun
 */
void* GUIManager::DbusListenerRun(void*)
{
    DBusConnection* bus;
    DBusError error;
    DBusMessage* msg;
    string strFilter;
    list<string> listOfInterfaces;
    const char* pszDeviceObj;
    OFSEnvironment& ofeitp = OFSEnvironment::Instance();
    listOfInterfaces = ofeitp.getListenDevices();
    dbus_error_init(&error);
    bus = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
    if (!bus)
    {
        printf("Failed to connect to the D-BUS daemon: %s", error.message);
        dbus_error_free(&error);
        pthread_exit(NULL);
        return NULL;
    }
//  strFilter = string("type='signal',path=/org/freedesktop/NetworkManager,interface=org.freedesktop.NetworkManager,member=DeviceNoLongerActive");
    
    dbus_bus_add_match(bus, strFilter.c_str(), NULL);
    while (true)
    {
        dbus_error_init(&error);
        dbus_connection_read_write(bus, -1);
        msg = dbus_connection_pop_message(bus);

        if (msg == NULL)
        { 
            sleep(1);
            continue;
        }
        else
        {
            if(dbus_message_is_signal(msg, "org.freedesktop.NetworkManager", "DeviceNoLongerActive"))
            {
                dbus_message_get_args(msg, &error, DBUS_TYPE_OBJECT_PATH, &device_obj, DBUS_TYPE_INVALID);
                for (list<string>::iterator it = listOfInterfaces.begin(); it != listOfInterfaces.end(); ++it)
                {
                    string strNetpath = "/org/freedesktop/NetworkManager/Devices/" + (*it);
                    if (strNetpath == pszDeviceObj)
                    { 
                        FilesystemStatusManager::Instance().available = false;
                    }
                }
            }
        }
        dbus_message_unref(msg);
    }
    pthread_exit(NULL);
    return NULL;
}
#endif // BRATWURST
