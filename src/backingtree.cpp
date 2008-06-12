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

Backingtree::Backingtree(string rPath)
{
this->relative_path=rPath;

}


Backingtree::~Backingtree()
{
}

const bool Backingtree::operator==(Backingtree const b)
{
if (this->relative_path==b.relative_path){
return true;
}
else
return false;
}




const string Backingtree::get_relative_path()
{
return this->relative_path;
}


bool Backingtree::is_in_backingtree(string path)
{
	if(path.length() >= relative_path.length() && path.substr(0,relative_path.length()) == relative_path) {
		return true;
	}
	return false;
}


bool Backingtree::backingtree_is_in(string path)
{
    /// @todo implement me
}
