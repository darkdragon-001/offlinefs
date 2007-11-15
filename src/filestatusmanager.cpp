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
#include "filestatusmanager.h"

std::auto_ptr<Filestatusmanager> Filestatusmanager::theFilestatusmanagerInstance;
Mutex Filestatusmanager::m;
Filestatusmanager::Filestatusmanager(){}
Filestatusmanager::~Filestatusmanager(){}
Filestatusmanager& Filestatusmanager::Instance()
{
    MutexLocker obtain_lock(m);
    if (theFilestatusmanagerInstance.get() == 0)
      theFilestatusmanagerInstance.reset(new Filestatusmanager);
    return *theFilestatusmanagerInstance;
}

/**
 * Create a File object, which holds all information about the requested file
 * @param Path The file path, relative to the current ofs mountpoint
 * @return Information about the requested file
 */
File Filestatusmanager::give_me_file(string Path)
{
	return File(false, false, string(TESTING_REMOTE_PATH)+Path,
		string(TESTING_BACKING_PATH)+Path);
}
