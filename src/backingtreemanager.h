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
/**
 *	@author Carsten Kolassa <Carsten@Kolassa.de>
 *
 *	Manages the list of #Backingtree s
 */
class BackingtreeManager: public persistable {
public:
    /**
     * Returns a Pointer to the Instance of the Backingtree Manager part of the Singleton Pattern
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
     * 
     * @param Remote_Path 
     * @return 
     */
    string set_Remote_Path(string Remote_Path);
    /**
     *  Setter to set the remote Path
     * @param Cache_path 
     * @return 
     */
    string set_Cache_Path(string Cache_path);
    /**
     *  Getter to retrieve the Remote Path
     * @return 
     */
    string get_Remote_Path();
    /**
     *  Getter to retrieve the Cache Path
     * @return 
     */
    string get_Cache_Path();
    /**
     * Function to get the Backingtree to a given Path
     * @param Relative_Path Path of the Backingtree
     * @return Backingtree that manages the Path given
     */
    static Backingtree Search_Backingtree_via_Path(string Relative_Path);
    /**
     * Determines if a given file is in a Backingpath or not
     * @param path Path of the file
     * @return Boolean variable true if the file is in the backingtree false if it isn not
     */
    bool Is_in_Backingtree(string path);
    /**
     * Writes all the stored Backingtrees to the disk
     */
    virtual void persist() const;
    /**
     * Reads all the stored Backingtrees from the disk
     */
    virtual void reinstate() const;
protected:
    BackingtreeManager();
  private:
    string Remote_Path;
    string Cache_path;
    static std::auto_ptr<BackingtreeManager> theBackingtreeManagerInstance;
    static list<Backingtree> backinglist;
    static bool is_already_registered(Backingtree Relative_Path);
    static Mutex m; 
};
#endif
