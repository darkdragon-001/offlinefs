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
// #include "fileref.h"
#include "mutex.h"
using namespace std;
/**
 *	@author Carsten Kolassa <Carsten@Kolassa.de>
 *
 *	Represents one Tree in the filesystem, the user marked as offline.
 *      Note: This class also holds the absolute path to the tree in cache
 *            This enables us to save the backingtrees in different paths
 *            and avoid rebuilding the complete remote tree in the cache later
 */
class Backingtree{
public:
    enum estatus { online, offline, updating, reintegrating };
    estatus status;
    inline estatus getStatus();
    /**
     * Constructor of Backingtree takes the relative position
     * of the backingtree as argument
     * @param rPath 
     */
    Backingtree(string rPath, string cPath);
    /**
     * Destructor
     */
    ~Backingtree();
    /**
     * Equals operator to compare 
     * @param b The Backingtree to compare to
     * @return true if equal false otherwise
     */
    const bool operator== (Backingtree b) const;
    /**
     * Getter for the relative path of the backing Tree
     * @return path relative to the mounted share
     */
    const string get_relative_path();
    /**
     * Getter for the cache path of the backing Tree.
     * @return absolute path to the cache on harddisk
     */
    const string get_cache_path();
    /**
     * Check if the given (relative) path is below the root of this backing tree
     * @param path file or directory path to check
     * @return true if path is in backingtree, false if not
     */
    bool is_in_backingtree(string path);
    /**
     * Check if the given (relative) path is a parent of the backingtree
     * @param path directory path to check
     * @return true if backingtree is below path, false if not
     */
    bool backingtree_is_in(string path);
    /**
     * Get the absolute path to the given file in cache
     * @param path the path relative to the remote share root
     * @return the absolute path in the cache or NULL if not in this backingtree
     */
    string get_cache_path(string path);
    /**
     * Start a thread updating the cache for this backing tree
     */
    void updateCache();
    void updateCacheRunner(string str);
    static void *updateCacheThread(void *);
protected:
    string relative_path;
    string cache_path;
private:
    int recurs_rmdir(const string absolutePath);
};

#endif
	  	 
