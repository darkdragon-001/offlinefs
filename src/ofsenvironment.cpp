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
#include "ofsenvironment.h"
#include "ofsexception.h"
#include "ofsconf.h"
#include "ofshash.h"
#include "ofslog.h"
#include <getopt.h>
#include <sstream>
#include <cstring>

#include <cstdlib>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

std::auto_ptr<OFSEnvironment> OFSEnvironment::theOFSEnvironmentInstance;
Mutex OFSEnvironment::m;
Mutex OFSEnvironment::initm;
bool OFSEnvironment::initialized = false;

OFSEnvironment& OFSEnvironment::Instance()
{
    if(!initialized) {
        throw new OFSException("OFS Environment not initialized", 1,true);
    }
    MutexLocker obtain_lock(m);
    if (theOFSEnvironmentInstance.get() == 0)
      theOFSEnvironmentInstance.reset(new OFSEnvironment());
    return *theOFSEnvironmentInstance;
}

OFSEnvironment::OFSEnvironment()
{
}


OFSEnvironment::~OFSEnvironment()
{
}


void OFSEnvironment::init(int argc, char *argv[]) throw(OFSException)
{
	string lRemotePath = "";
	string lCachePath = "";
	string lShareID = "";
	string lMountOptions = "";
	bool lAllowOther = false;
	bool lUnmount = true;
    bool lUseFSCache = false;
	list<string> lListenDevices;
    list<string> lMountOptionsList;

    enum {
    	USER_OPT = 0,
    	UID_OPT,
    	GROUP_OPT,
    	GID_OPT
    };

    char * const token [] = {
    		"user",
    		"uid",
    		"group",
    		"gid",
    		NULL
    };

    uid_t l_uid = -1;
    gid_t l_gid = -1;
    char * subopts;
    char * value;

	MutexLocker obtain_lock(initm);
	initialized = true;
	OFSEnvironment &env = Instance();
	
	env.ofsdir = "/var/ofs";
	
	// parse command line
	// TODO: cache-and remote path have to be custom for each share
	int nextopt;
	// define allowed options
	const char* const short_options = ":r:b:l:i:p:o:nht";
	const struct option long_options[] = {
		{"remote", required_argument, NULL, 'r'},
		{"backing", required_argument, NULL, 'b'},
		{"listen", required_argument, NULL, 'l'},
		{"shareid", required_argument, NULL, 'i'},
		{"options", required_argument, NULL, 'o'},
		{"nounmount", required_argument, NULL, 'n'},
		{"remoteoptions", required_argument, NULL, 'p'},
		{"allowother", required_argument, NULL, 't'},
		{"help", no_argument, NULL, 'h'}
	};
	
	if (argc < 3)
		throw OFSException("Not enough parameters", 1,true);
	// binary name, mountpoint and remote path
	env.binarypath = argv[0];
	env.mountPoint = argv[1];
	env.shareURL = argv[2];
	ofslog::debug("shareURL: %s", env.shareURL.c_str());
	// parsing paramaters ('man getopt_long' for usage)
	while((nextopt =
	    getopt_long(argc, argv, short_options, long_options, NULL)) != -1){
		switch(nextopt) {
		   case 'r': // remote mountpoint
			lRemotePath=optarg;
			break;
		   case 'b': // cache tree root
			lCachePath=optarg;
			break;
		   case 'l': // devices to listen on
			char *saveptr;
			char *strdevice;
			strdevice = strtok_r(optarg, ",", &saveptr);
			lListenDevices.clear();
			while(strdevice != NULL) {
				lListenDevices.push_back(strdevice);
				strdevice = strtok_r(NULL, ",", &saveptr);
			}
			break;
		   case 'i': // custom share identifier
			lShareID = optarg;
			break;
		   case 'p': // mount options
   // FIXME: save options for use in mount command, no need to parse those!
			   char *mntopt;
               mntopt = optarg;
			   lMountOptions = mntopt;
               char *optsaveptr;
               char *stroption;
               stroption = strtok_r(mntopt, ",", &optsaveptr);
               lMountOptionsList.clear();
               while(stroption != NULL) {
                  lMountOptionsList.push_back(stroption);
                  stroption = strtok_r(NULL, ",", &optsaveptr);
               }
			   break;
			case 'o':
				subopts = optarg;
				while (*subopts != '\0') {
					switch(getsubopt(&subopts, token, &value)) {
					case USER_OPT:
						if (l_uid != -1) {
							cerr << "User id already set" << endl;
							exit (EXIT_FAILURE);
						}
						struct passwd * userinfo;
						userinfo = getpwnam(value);
						if (userinfo == NULL) {
							cerr << "User not found" << endl;
							exit (EXIT_FAILURE);
						}
						l_uid = userinfo->pw_uid;
						break;
					case UID_OPT:
						if (l_uid != -1) {
							cerr << "User id already set" << endl;
							exit (EXIT_FAILURE);
						}
						l_uid = static_cast<uid_t>(atol(value));
						break;
					case GROUP_OPT:
						if (l_gid != -1) {
							cerr << "Group id already set" << endl;
							exit (EXIT_FAILURE);
						}
						struct group * groupinfo;
						groupinfo = getgrnam(value);
						if (groupinfo == NULL) {
							cerr << "Group not found" << endl;
							exit (EXIT_FAILURE);
						}
						l_gid = groupinfo->gr_gid;
						break;
					case GID_OPT:
						if (l_gid != -1) {
							cerr << "Group id already set" << endl;
							exit (EXIT_FAILURE);
						}
						l_gid = static_cast<gid_t>(atol(value));
						break;
					default: // unknown option (ignore)
						// FIXME: deal with unknown options
						break;
					};
				}
				break;
		   case 't': // allow other users access to the file system
			lAllowOther = true;
			break;
		   case 'n': // do not unmount remote share after exit
			lUnmount = false;
			break;
		   case 'h': // display help
			throw OFSException("User wants help",1);
		   case '?': // invalid parameter
			throw OFSException("Unknown parameter: "+optopt,1);
		   case ':': // missing argument
			throw OFSException(
				"Missing argument for parameter: "+optopt,1);
		   default: // unknown behavior
			throw OFSException(
			"Undefined parameter or error while parsing command line"
			,1,true);
		}
		
	}

	// FIXME: This should become an OFS option
    // parsing the mount options
    if (!lMountOptionsList.empty())
    {
       list<string>::iterator i;
       for(i=lMountOptionsList.begin(); i != lMountOptionsList.end(); ++i)
       {
          // option 'fsc': "Try to use FSCache"
          if (*i == "fsc")
             lUseFSCache = true;
       }
    }

	// variable from config file
	OFSConf &ofsconf = OFSConf::Instance();
	// share ID
	if(lShareID=="")
		env.shareID = ofs_hash(env.shareURL);
	else
		env.shareID = lShareID;
	// remote path
	if(lRemotePath=="")
		env.remotePath = ofsconf.GetRemotePath()+"/"+env.shareID;
	else
		env.remotePath = lRemotePath;
	// cache path
	if(lCachePath=="")
		env.cachePath = ofsconf.GetBackingTreePath()+"/"+env.shareID;
	else
		env.cachePath = lCachePath;
	// network interfaces to listen
	if(lListenDevices.empty())
		env.listendevices = ofsconf.GetListenDevices();
	else
		env.listendevices = lListenDevices;
    // mount options
    env.mountoptions = lMountOptions;
	// unmount flag
	env.unmount = lUnmount;
	// allow other flag
	env.allowother = lAllowOther;
    // use FSCache flag
    env.usefscache = lUseFSCache;
    env.uid = (l_uid == -1) ? 0 : l_uid;
    env.gid = (l_gid == -1) ? 0 : l_gid;

}

list<string> OFSEnvironment::getListenDevices()
{
	return listendevices;
}

string OFSEnvironment::getUsageString(string executable)
{
	stringstream usage;
	usage << "Usage: "+executable+" <mountpoint> <url> [<options>]"
		<< endl;
	usage << "Options are:" << endl;
	usage << "-r --remote          Absolute path to the remote mount point"
		<< endl;
	usage << "-b --backing         Absolute path to the backing (cache) root"
		<< endl;
	usage << "-l --listen          Comma separated list of network interfaces to "
		<< " listen for plug/unplug events" << endl;
	usage << "-i --shareid         Internal unique share identifier" << endl;
	usage << "-t --allowother      Allow all users access to the file system" << endl;
	usage << "-o --options         Options for OFS" << endl;
	usage << "-p --remoteoptions   Mount options as given to the remote mount command via -o" << endl;
	usage << "-n --nounmount       Do not unmount the remote file system on exit" << endl;
	usage << "-h --help            Print this screen and exit" << endl;
	return usage.str();
}


