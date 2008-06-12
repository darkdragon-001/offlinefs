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
Mutex BackingtreeManager::m;
BackingtreeManager::BackingtreeManager(){}
BackingtreeManager::~BackingtreeManager(){}
BackingtreeManager& BackingtreeManager::Instance()
{
    MutexLocker obtain_lock(m);
    if (theBackingtreeManagerInstance.get() == 0)
      theBackingtreeManagerInstance.reset(new BackingtreeManager());
    return *theBackingtreeManagerInstance;
}
void BackingtreeManager::register_Backingtree(string relative_Path){
	MutexLocker obtain_lock(m);
	if(!Is_in_Backingtree(relative_Path)) {
		// if there are backingtrees below this one, remove them,
		// because the new one is the new root
		list<Backingtree> subtrees = getBackingtreesBelow(relative_Path);
		for (list<Backingtree>::iterator it = subtrees.begin();
        		it != subtrees.end(); ++it) {
			// I do not call the remove_Backingtree method here
			// because this would always trigger persistation
			backinglist.remove(*it);
		}
		// add the new backingtree and make list persistent
		backinglist.push_back(Backingtree(relative_Path));
		persist();
 	}
}

void BackingtreeManager::remove_Backingtree(string Relative_Path) {
	backinglist.remove(Relative_Path);
	persist();
}

string BackingtreeManager::get_Remote_Path()
{
return Remote_Path;
}
bool BackingtreeManager::Is_in_Backingtree(string path){
 for (list<Backingtree>::iterator it = backinglist.begin();
        it != backinglist.end(); ++it) {
	if(it->is_in_backingtree(path))
		return true;
   }
   return false;
}
string BackingtreeManager::get_Cache_Path()
{
return Cache_path;
}

void BackingtreeManager::persist() const
{
	BackingtreePersistence &btp = BackingtreePersistence::Instance();
	
	btp.backingtrees(backinglist);
}

void BackingtreeManager::reinstate()
{
	BackingtreePersistence &btp = BackingtreePersistence::Instance();
	backinglist=btp.backingtrees();
}

list<Backingtree> BackingtreeManager::getBackingtreesBelow(string path)
{
	list<Backingtree> trees;
	for (list<Backingtree>::iterator it = backinglist.begin();
		it != backinglist.end(); ++it) {
		if(it->backingtree_is_in(path))
			trees.push_back(*it);
	}
	return trees;
}
