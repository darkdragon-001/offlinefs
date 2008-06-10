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
#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <iostream>
#include <cstdlib>
#include <vector>
#include "file.h"
#include "fileref.h"
#include "mutex.h"
using namespace std;
/**
	@author Carsten Kolassa <Carsten@Kolassa.de>
*/
class Backingtree{
public:
    /**
     * Constructor of Backingtree takes the relative position of the backingtree as argument
     * @param rPath 
     */
    Backingtree(string rPath);
    /**
     * Destructor
     */
    ~Backingtree();
    /**
     * Equals operator to compare 
     * @param b The Backingtree to compare to
     * @return true if equal false otherwise
     */
    const bool operator== (Backingtree const b);
    /**
     * Getter fot the relative path of the backing Tree
     * @return 
     */
    const string get_relative_path();
    /**
     * Getter for the complete path of the backing Tree.
     * @return 
     */
    const string get_complete_path();
protected:
    string relative_path;
private:

};

#endif
