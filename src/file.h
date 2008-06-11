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
#ifndef FILE_H
#define FILE_H
#include <string>

using namespace std;
/**
	@author Carsten Kolassa <Carsten@Kolassa.de>,
		Tobias Jaehnel <tjaehnel@gmail.com>
	The file Object represents one file in the filesystem. It holds all
	ofs-specific infomation and is responsible for performing the
	operations on this file.
	Most of the methods are called by the ofs_fuse callback functionss
	
	Instances are created and filled by the filestatusmanager.
	
	Immutable
*/
class File {
public:
	/**
	 * ctor - initialize the values
	 * @param offline_state should be available offline
	 * @param availability  remote share available
	 * @param relative_path path relative to mountpoint
	 * @param remote_path absolute path in mountpoint
	 * @param cache_path absolute path in cache
	 */
	File(bool offline_state, bool availability, string relative_path,
		string remote_path, string cache_path);
	/**
	 * copy ctor
	 * @param copy object to copy
	 */
	File(const File &copy);
	/**
	 * assignment operator
	 * @param copy object to copy
	 * @return a reference to the current object
	 */
	File& operator =(const File &copy);
	/**
	 * is available offline
	 * @return offline flag
	 */
	bool get_offline_state() const;
	/**
	 * remote share available
	 * @return availability
	 */
	bool get_availability() const;
	/**
	 * 
	 * @return 
	 */
	bool get_sync_state();
	/**
	 * get absolute path in mountpoint
	 * @return path
	 */
	string get_remote_path() const;
	/**
	 * get absolute path in cache
	 * @return path
	 */
	string get_cache_path() const;
        /**
         * get path relative to mountpoint
         * @return path
         */
        string get_relative_path() const;
	~File();

private:
	bool offline_state;
	bool availability;
	string relative_path;
	string remote_path;
	string cache_path;
};

#endif
