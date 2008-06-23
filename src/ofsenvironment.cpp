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
#include <getopt.h>
#include <sstream>

std::auto_ptr<OFSEnvironment> OFSEnvironment::theOFSEnvironmentInstance;
Mutex OFSEnvironment::m;
Mutex OFSEnvironment::initm;
bool OFSEnvironment::initialized = false;

OFSEnvironment& OFSEnvironment::Instance()
{
    if(!initialized)
        throw new OFSException("OFS Environment not initialized", 1);
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
	bool lAllowOther = false;
	bool lUnmount = true;
	list<string> lListenDevices;
	
	MutexLocker obtain_lock(initm);
	initialized = true;
	OFSEnvironment &env = Instance();
	// parse command line
	// TODO: cache-and remote path have to be custom for each share
	int nextopt;
	// define allowed options
	const char* const short_options = ":r:b:l:i:onh";
	const struct option long_options[] = {
		{"remote", required_argument, NULL, 'r'},
		{"backing", required_argument, NULL, 'b'},
		{"listen", required_argument, NULL, 'l'},
		{"shareid", required_argument, NULL, 'i'},
		{"allowother", required_argument, NULL, 'o'},
		{"nounmount", required_argument, NULL, 'n'},
		{"help", no_argument, NULL, 'h'}
	};
	
	if (argc < 3)
		throw OFSException("Not enough parameters", 1);
	// binary name, mountpoint and remote path
	env.binarypath = argv[0];
	env.mountPoint = argv[1];
	env.shareURL = argv[2];
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
		   case 'o': // allow other users access to the filesystem
			lAllowOther = true;
			break;
		   case 'n': // do not unmount remote share after ext
			lUnmount = false;
			break;
		   case 'h': // display help
			throw OFSException("User wants help",1);
		   case '?': // invalid parameter
			throw OFSException("Unknown parameter: "+optopt,1);
		   case ':': // missing argument
			throw OFSException(
				"Missing argument for parameter: "+optopt,1);
		   default: // unknown behaviour
			throw OFSException(
			"Undefined parameter or error while parsing commandline"
			,1);
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
	// unmount flag
	env.unmount = lUnmount;
	// allow other flag
	env.allowother = lAllowOther;
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
	usage << "-r --remote     Absolute path to the remote mountpoint"
		<< endl;
	usage << "-b --backing    Absolute path to the backing (cache) root"
		<< endl;
	usage << "-l --listen     Comma separated list of network interfaces to "
		<< " listen for plug/unplug events" << endl;
	usage << "-i --shareid    Internal unique share identifier" << endl;
	usage << "-o --allowother Allow all users access to the filesystem" << endl;
	usage << "-n --nounmount  Do not unmount the remote share on exit" << endl;
	usage << "-h --help       Print this screen and exit" << endl;
	return usage.str();
}


