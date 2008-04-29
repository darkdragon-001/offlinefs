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
#ifndef BACKINGTREEMANAGER_H
#define BACKINGTREEMANAGER_H
#include "mutexlocker.h"
#include "backingtree.h"
#include <string>
#include <list>
/**
	@author Carsten Kolassa <Carsten@Kolassa.de>
*/
class BackingtreeManager{
public:
    static BackingtreeManager& Instance();
    ~BackingtreeManager(); 
    void register_Backingtree(string relative_Path);
    void remove_Backingtree(string relative_Path);
    static string set_Remote_Path(string Remote_Path);
    static string set_Cache_Path(string Cache_path);
    static Backingtree Search_Backingtree_via_Path(string Relative_Path);
protected:
    BackingtreeManager();
  private:
    static string Remote_Path;
    static string Cache_path;
    static std::auto_ptr<BackingtreeManager> theBackingtreeManagerInstance;
    static list<Backingtree> backinglist;
    static bool is_already_registered(Backingtree Relative_Path);
    static Mutex m; 
};
#endif
