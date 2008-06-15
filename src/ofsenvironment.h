/***************************************************************************
 *   Copyright (C) 2007 by                                                 *
 *                 Frank Gsellmann, Tobias Jaehnel, Carsten Kolassa        *
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
#ifndef OFSENVIRONMENT_H
#define OFSENVIRONMENT_H
#include "mutexlocker.h"
#include <string>
#include <list>
using namespace std;

/**
 * @author Tobias Jaehnel <tjaehnel@gmail.com>
 *
 * Manages configuration values for the current process. Those are determined
 * - from configuration file via OFSConf
 * - from the command line, which overrides the config file
 */
class OFSEnvironment{
public:
    /**
     * Get singleton instance
     * @return singleton instance
     */
    static OFSEnvironment& Instance();

    ~OFSEnvironment();
    /**
     * Initialize the Environment
     * load data from config file
     * and parse commandline
     * @param args Commandline parameters
     */
    static void init();
    /**
     * Get the path, the remote share is mounted to
     * @return remote share path
     */
    string getRemotePath();
    /**
     * Get the root of the backingtree path
     * @return backingtree path
     */
    string getCachePath();
protected:
    OFSEnvironment();
private:
    static std::auto_ptr<OFSEnvironment> theOFSEnvironmentInstance;
    static Mutex m;
    static Mutex initm;
    string remotePath;
    string cachePath;
    static bool initialized;
};

#endif
