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
#include "persistable.h"
#include "backingtreepersistence.h"
#include <string>
#include <list>
#include <memory>
/**
 *	@author Carsten Kolassa <Carsten@Kolassa.de>,
 *		Tobias Jaehnel <tjaehnel@gmail.com>
 *
 *	Manages the list of #Backingtree s
 */
class BackingtreeManager: public persistable {
public:
    /**
     * Returns a Pointer to the Instance of the Backingtree Manager
     * part of the Singleton Pattern
     * @return Pointer to the Instance of the Backingtree Manager
     */
    static BackingtreeManager& Instance();
    /**
     * Destructor of the Backingtree Manager
     */
    ~BackingtreeManager();
    /**
     * Registers a relative path in the managed directory tree as backing tree
     * @param relative_Path The relative path that should be registered
     */
    void register_Backingtree(string relative_Path);
    /**
     * Removes a relative Path from the BackingtreeManager
     * @param relative_Path 
     */
    void remove_Backingtree(string relative_Path);
    /**
     *  Getter to retrieve the Cache Path
     * @return 
     */
    string get_Cache_Path();
    /**
     * Function to get the Backingtree to a given Path
     * @param Relative_Path Path of the Backingtree
     * @return Backingtree that manages the Path given or NULL of none
     */
    Backingtree *Search_Backingtree_via_Path(string Relative_Path);
    /**
     * Determines if a given file is in a Backingpath or not
     * @param path Path of the file
     * @return true if the file is in a backingtree false if it is not
     */
    bool Is_in_Backingtree(string path);
    /**
     * Writes all the stored Backingtrees to the disk
     */
    virtual void persist() const;
    /**
     * Reads all the stored Backingtrees from the disk
     */
    virtual void reinstate();
    /**
     * Get all registered backingtrees, which have its roots below path
     * @param path the directory
     * @return A list of backingtrees - may be an empty list but not NULL
     */
    list<Backingtree> getBackingtreesBelow(string path);
protected:
    BackingtreeManager();
  private:
    static std::auto_ptr<BackingtreeManager> theBackingtreeManagerInstance;
    list<Backingtree> backinglist;
    static Mutex m; 
};
#endif
