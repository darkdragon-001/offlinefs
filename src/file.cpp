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
#include "file.h"

File::~File()
{
}

File::File(const bool offline_state, const bool availability,
		const string remote_path, const string cache_path) :
	offline_state(offline_state), availability(availability),
	remote_path(remote_path), cache_path(cache_path)
{}

string File::get_cache_path() const
{
	return cache_path;
}

bool File::get_offline_state() const
{
	return offline_state;
}

bool File::get_availability() const
{
	return availability;
}

string File::get_remote_path() const
{
	return remote_path;
}

File::File(const File &copy)
{
	operator =(copy);
}

File & File::operator =(const File &copy)
{
	offline_state = copy.offline_state;
	availability = copy.availability;
	remote_path = copy.remote_path;
	cache_path = copy.cache_path;

	return *this;
}
