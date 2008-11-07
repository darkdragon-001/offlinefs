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
//#ifndef GUIMANAGER_H
#define GUIMANAGER_H

/*
#include "file_sync.h"
#include "file.h"
#include "syncstatetype.h"
#include "persistable.h"
#include <iostream>
#include <map>
#include <string>

using namespace std;
*/
#include "mutexlocker.h"

/**
    @author Frank Gsellmann <frank.gsellmann@gmx.de>
*/
class GUIManager
{
public:
    static GUIManager& Instance();
    ~GUIManager();
    /**
     * Runs the Dbus listener which monitors the signals
     * of the OFS daemon
     */
    void StartDbusListener();
    /**
     * Monitors the OFS daemon via DBUS
     * @param 
     * @return 
     */
    static void* DbusListenerRun(void*);
protected:
    GUIManager();
protected:
private:
    static std::auto_ptr<GUIManager> theGUIManagerInstance;
    static Mutex m_mutex;
};

#endif  // !GUIMANAGER_H
