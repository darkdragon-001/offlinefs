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
#include "backingtreemanager.h"


std::auto_ptr<BackingtreeManager> BackingtreeManager::theBackingtreeManagerInstance;
list<Backingtree> BackingtreeManager::backinglist;
Mutex BackingtreeManager::m;
BackingtreeManager::BackingtreeManager(){}
BackingtreeManager::~BackingtreeManager(){}
BackingtreeManager& BackingtreeManager::Instance()
{
    MutexLocker obtain_lock(m);
    if (theBackingtreeManagerInstance.get() == 0)
      theBackingtreeManagerInstance.reset(new BackingtreeManager);
    return *theBackingtreeManagerInstance;
}
void BackingtreeManager::register_Backingtree(string relative_Path){
Backingtree b=Backingtree::Backingtree(relative_Path);
bool already_an_included_path=false;
MutexLocker obtain_lock(m);
if(!is_already_registered(b)){
this->backinglist.push_back(b);
}
}

void BackingtreeManager::remove_Backingtree(string Relative_Path){
Backingtree b=Backingtree::Backingtree(Relative_Path);
for (list<Backingtree>::iterator it = backinglist.begin();
        it != backinglist.end(); ++it) {
      if((*it)==Relative_Path){
	backinglist.erase(it);
}
   }
}

bool BackingtreeManager::is_already_registered(Backingtree Relative_Path)
{
 for (list<Backingtree>::iterator it = backinglist.begin();
        it != backinglist.end(); ++it) {
      return (*it)==Relative_Path;
   }
}

string BackingtreeManager::get_Remote_Path()
{
return Remote_Path;
}
bool BackingtreeManager::Is_in_Backingtree(string path){
 for (list<Backingtree>::iterator it = backinglist.begin();
        it != backinglist.end(); ++it) {
	string s=(*it).get_relative_path();
      return (strcmp(s.c_str(), path.c_str())==strlen(s.c_str()));
   }
}
string BackingtreeManager::get_Cache_Path()
{
return Cache_path;
}

void BackingtreeManager::persist() const
{
	BackingtreePersistence btp = BackingtreePersistence::Instance();
	
	//btp.backingtrees(backinglist);
}

void BackingtreeManager::reinstate() const
{
BackingtreePersistence btp = BackingtreePersistence::Instance();
	//backinglist=btp.backingtrees();
}