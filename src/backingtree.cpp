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
#include "filesystemstatusmanager.h"
#include "ofsfile.h"
#include "ofslog.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <map>
using namespace std;

Backingtree::Backingtree(string rPath, string cPath) : status(online)
{
	this->relative_path=rPath;
	this->cache_path=cPath;
}


Backingtree::~Backingtree()
{
}

const bool Backingtree::operator==(Backingtree b) const
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
		return "";
	}
	if(path.length() == get_relative_path().length())
		abspath = get_cache_path();
	else 
		abspath = get_cache_path()+"/"
			+path.substr(relative_path.length(),
				path.length()-relative_path.length());
	return abspath;
}

void Backingtree::updateCache()
{
	ofslog::debug("updateCache");
	pthread_t *thread = new pthread_t;
	pthread_create(thread, NULL, Backingtree::updateCacheThread, (void *)this);
}

void *Backingtree::updateCacheThread(void *arg)
{
    Backingtree *back = (Backingtree *)arg;
    back->status = updating;
    back->updateCacheRunner(back->get_relative_path());
    back->status = online;
    return NULL;
}

void Backingtree::updateCacheRunner(string relativeDir)
{
    struct dirent *entry;
    struct stat fileinfo;
    map<string, struct stat> subdirs;
    map<string, struct stat> regularfiles;    
    // update this directory
    OFSFile file(relativeDir);
    ///\todo What to do if there are local modifications regarding stat information
    file.update_cache();
    string absoluteRemoteDir = file.get_remote_path();
    string absoluteCacheDir = file.get_cache_path();;

    // first traverse remote directory 
    // and make sure all files and directories in the cache are current
    DIR *dir = opendir(absoluteRemoteDir.c_str());
    if(dir == NULL)
        return; ///\todo do something on error
    
    while( (entry = readdir(dir)) != NULL)
    {
        string filename = entry->d_name;
        if(filename == "." || filename == "..")
            continue;
        string absolutePath = absoluteRemoteDir+"/"+filename;
        string relativePath = relativeDir+"/"+filename;
        int ret = lstat(absolutePath.c_str(), &fileinfo);
        if(ret == 0)
        {
            if(S_ISDIR(fileinfo.st_mode))
                subdirs[filename] = fileinfo;
            else
            {
                regularfiles[filename] = fileinfo;
                // make sure the file is current
                OFSFile file(relativePath);
                ///\todo Only update the file if it has no local modifications
                file.update_cache();
            }
        }
    }
    closedir(dir);
    
    // second traverse the cache directory and remove files and directories
    // which have been removed from the remote directory
    dir = opendir(absoluteCacheDir.c_str());
    if( dir == NULL )
    {
        return; ///\todo do something on error
    }
    while( (entry = readdir(dir) ) != NULL)
    {
        string filename = entry->d_name;
        string absolutePath = absoluteCacheDir+"/"+filename;
        string relativePath = relativeDir+"/"+filename;
        ///\todo Only delete the file if it has no local modifications
        int ret = lstat(absolutePath.c_str(), &fileinfo);
        if(ret == 0)
        {
            if(S_ISDIR(fileinfo.st_mode))
            {
                map<string, struct stat>::iterator iter = subdirs.find(filename);
                if(iter == subdirs.end())
                {
                    rmdir(absolutePath.c_str());
                }
            }
            else
            {
                map<string, struct stat>::iterator iter = regularfiles.find(filename);
                if(iter == regularfiles.end())
                {
                    unlink(absolutePath.c_str());
                }
            }
        }
    }
    closedir(dir);
    
    // recurse into sub-directories
    map<string, struct stat>::iterator iter = subdirs.begin();
    while(iter != subdirs.end())
    {
        updateCacheRunner(relativeDir+"/"+iter->first);
        iter++;
    }
}
