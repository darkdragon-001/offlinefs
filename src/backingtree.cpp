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
#include "backingtree.h"

Backingtree::Backingtree(string rPath, string cPath)
{
	this->relative_path=rPath;
	this->cache_path=cPath;
}


Backingtree::~Backingtree()
{
}

const bool Backingtree::operator==(Backingtree const b)
{
	if (this->relative_path==b.relative_path)
		return true;
	else
		return false;
}

const string Backingtree::get_cache_path()
{
	return this->cache_path;
}

const string Backingtree::get_relative_path() {
	return relative_path;
}

bool Backingtree::is_in_backingtree(string path)
{
	if(path.length() >= relative_path.length() &&
		path.substr(0,relative_path.length()) == relative_path) {
		return true;
	}
	return false;
}


bool Backingtree::backingtree_is_in(string path)
{
	if(relative_path.length() >= path.length() &&
		relative_path.substr(0,path.length()) == path) {
		return true;
	}
	return false;
}

string Backingtree::get_cache_path(string path)
{
	string abspath;
	if(!is_in_backingtree(path)) {
		return NULL;
	}
	if(path.length() == get_relative_path().length())
		abspath =  get_cache_path();
	else 
		abspath = get_cache_path()+"/"
			+path.substr(relative_path.length(),
				path.length()-relative_path.length());
	return abspath;
}
