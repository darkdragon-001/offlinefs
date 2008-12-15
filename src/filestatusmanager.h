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
#ifndef FILESTATUSMANAGER_H
#define FILESTATUSMANAGER_H
#include "file.h"
#include <string>
#include <map>
#include <memory>
#include "mutex.h"
#include "mutexlocker.h"

using namespace std;

/**
	@author Carsten Kolassa <Carsten@Kolassa.de>, 
		Tobias Jaehnel <tjaehnel@gmail.com>

	Singleton which gives us the corresponding File-Object to a 
	given file path
*/
class Filestatusmanager{
public:
	/**
	 * Function to query the status of a File
	 * @param Path The Path to the File as string
	 * @return File Object that corresponds to the given Path
	 */
	File give_me_file(string Path);
	/**
	 * Returns a Pointer to the Instance of the Filestatusmanager
	 * part of the Singleton Pattern
	 * @return Pointer to the Filestatusmanager
	 */
	static Filestatusmanager& Instance();
	/**
	 * 
	 */
	~Filestatusmanager();
protected:
	Filestatusmanager();
    
private:

    static std::auto_ptr<Filestatusmanager> theFilestatusmanagerInstance;
    static Mutex m; 
};

#endif
