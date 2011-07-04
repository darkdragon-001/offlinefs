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
#include "ofsexception.h"
#include <string>
#include <list>
#include <memory>
#include <sys/types.h>

using namespace std;

/**
 * @author Tobias Jaehnel <tjaehnel@gmail.com>
 * @author Samuel Walz <samuel.walz@gmail.com>
 * @author Peter Trommler <ptrommler@acm.org>
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
     * @param argc 
     * @param argv[] 
     * @return true of command line is OK, false other
     */
    static void init(int argc, char *argv[]) throw(OFSException);

    /**
     * Get the path, the remote share is mounted to
     * @return remote share path
     */
    inline string getRemotePath() { return remotePath; };
    /**
     * Get the root of the backingtree path
     * @return backingtree path
     */
    inline string getCachePath() { return cachePath; };
    /**
     * Get the path this filesystem is mounted to
     * @return mountpoint
     */
    inline string getMountPoint() { return mountPoint; };
    /**
     * Get the hashed URL of the remote share as unique identifier
     * used in mountpoint and backing path for example
     * @return share ID
     */
    inline string getShareID() { return shareID; };
    /**
     * Get the URL of the share
     * @return share URL
     */
    inline string getShareURL() { return shareURL; };
    /**
     * Get the complete path of the called ofs binary
     * @return path of binary
     */
    inline string getBinaryPath() { return binarypath; };
    /**
     * Mount read write?
     */
    inline bool isReadWrite() {return readwrite; };
    /**
     * Should the remote share be unmounted when this fs is unmounted
     * @return unmount flag
     */
    inline bool isUnmount() { return unmount; };
    /**
     * Should the filesystem be accessable by all users
     * @return allowother flag
     */
    inline bool isAllowOther() { return allowother; };
    /**
     * Should be tried to use FSCache?
     * @return usefscache flag
     */
    inline bool isFSCacheUser() { return usefscache; };

    /**
     * Get the global OFS directory where all persistence files,
     * cache and remote mountpoints are in
     * @return path to the ofs directory
     */
    inline string getOfsDir() { return ofsdir; };
    /**
     * Get the mount options that should be passed to the mount command
     * via the -o parameter when mounting the remote share
     * @return mount parameters
     */
    inline string getMountOptions() { return mountoptions; };

    /**
     * @return user id for OFS daemon
     */
    inline uid_t getUid() { return uid; };

    /**
     * @return group id for OFS daemon
     */
    inline gid_t getGid() { return gid; };

    /**
     * Return the list of devices the system should listen for plug/unplug
     * @return list of devices as strings
     */
    list<string> getListenDevices();
    /**
     * Create the usage string
     * @param executable the called executable
     * @return the usage string
     */

    inline bool getlazywrite() { return lazywrite; };
    /**
     * Return if lazywrite activated or not
     */

    inline int getlwoption() { return lwoption; };
    /**
     * Return which Syncmodus is selected
     */
    static string getUsageString(string executable="ofs");
protected:
    OFSEnvironment();
private:
    static std::auto_ptr<OFSEnvironment> theOFSEnvironmentInstance;
    static Mutex m;
    static Mutex initm;
    string remotePath;
    string cachePath;
    string mountPoint;
    string shareID;
    string shareURL;
    string binarypath;
    string mountoptions;
    bool readwrite;
    bool unmount;
    bool allowother;
    bool usefscache;
    bool lazywrite;
    int lwoption;  //c=CPU n=Network t=Timer
    string ofsdir;
    uid_t uid;
    gid_t gid;
protected:
    list<string> listendevices;
    static bool initialized;
};

#endif
