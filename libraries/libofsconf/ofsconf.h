/***************************************************************************
 *   Copyright (C) 2008 by Tobias Jähnel,,,   *
 *   tobias@gmail.com   *
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
#ifndef OFSCONF_H
#define OFSCONF_H

#include <string>

class Mutex;
struct cfg_t;

using namespace std;

/**
	@author Frank Gsellmann,,, <frank.gsellmann@gmx.de>
*/
class OFSConf
{
protected:
    /**
     * Protected default constructor of OFSConf only for serialization
     * @remark Use Instance() for instantiating an object.
     */
    OFSConf();

public:
    /**
     * Destructor of OFSConf, frees the Confuse objects.
     */
    ~OFSConf();

public:
    /**
     * Returns an automatic pointer to the one and only OFSConf object.<br>
     * Creates an instance of the objects at the first call (singleton pattern)
     * @return Pointer to the instance of the OFSConf object
     */
    static OFSConf& Instance();

    /**
     * Initializes the Confuse parser and invokes it.
     * @return true if the file was parsed successfully<br>
     *         false if errors occured whilst parsing
     */
    bool ParseFile();

//    string GetRemoteShareName(const int nIndex);
    /**
     * Returns the remote path that has been found in the configuration file.
     * @return Path of the Remote
     */
    string GetRemotePath();
    /**
     * Returns the backing tree path that has been found in the configuration file.
     * @return Path of the backing tree
     */
    string GetBackingTreePath();

protected:

    /**
     * Returns true if the file has already been parsed successfully and false otherwise.
     */
    bool m_bFileParsed;
    /**
     * Pointer to data structure, Confuse uses for parsing.
     */
    cfg_t* m_pCFG;

    /**
     * Automatic pointer to to the one and only OFSConf object
     * (Used according to singleton pattern.)
     */
    static std::auto_ptr<OFSConf> theOFSConfInstance;

private:
    /**
     * Static mutex object that guarantees that there is only one instance.
     */
    static Mutex m_mutex;
    string remotePath;
    string backingPath;
};

#endif
