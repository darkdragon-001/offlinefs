/***************************************************************************
 *   Copyright (C) 2007, 2011 by                                           *
 *      Frank Gsellmann, Tobias Jaehnel, Carsten Kolassa, Peter Trommler   *
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "ofsenvironment.h"
#include "ofsexception.h"
#include "ofsconf.h"
#include "ofshash.h"
#include "ofslog.h"
#include <getopt.h>
#include <stdlib.h>
#include <sstream>
#include <cstring>

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
	bool lReadWrite = false;
	bool lReadOnly = false;
	bool lAllowOther = false;
	bool lUnmount = true;
	bool lUseFSCache = false;

	list<string> lListenDevices;
	list<string> lMountOptionsList;

	MutexLocker obtain_lock(initm);
	initialized = true;
	OFSEnvironment &env = Instance();

	env.ofsdir = "/var/ofs";
	env.lazywrite=false;
	// TODO: use enumeration for lazy write parameter!
	env.lwoption='n';  //c=CPU n=Network t=Timer

	// TODO: cache-and remote path have to be custom for each share
	int nextopt;

	const char* const short_options = ":r:b:l:i:p:o:nhVz:";
	const struct option long_options[] = {
			{"version",0,NULL,'V'},
			{"options", required_argument, NULL, 'o'},
			{"help", no_argument, NULL, 'h'},
			{"remote", required_argument, NULL, 'r'},
			{"remoteoptions", required_argument, NULL, 'p'},
			{"backing", required_argument, NULL, 'b'},
			{"listen", required_argument, NULL, 'l'},
			{"shareid", required_argument, NULL, 'i'},
			{"allowother", no_argument, NULL, 't'},
			{"nounmount", required_argument, NULL, 'n'},
			{"lazywrite", required_argument, NULL, 'z'},
			{NULL,0,NULL,0} /* Required at end of array. */
	};

	if (argc < 3)
		throw OFSException("Not enough parameters", 1,true);
	// binary name, mount point and remote path
	// TODO: support multiple remote file systems
	env.binarypath = argv[0];
	env.mountPoint = argv[2];
	env.shareURL = argv[1];
	ofslog::debug("shareURL: %s", env.shareURL.c_str());

	enum {
		REMOTE_OPTIONS_OPT = 0,
		READ_WRITE_OPT,
		READ_ONLY_OPT,
		BACKING_OPT,
		LISTEN_OPT,
		SHARE_ID_OPT,
		ALLOWOTHER_OPT,
		NOUNMOUNT_OPT,
		FS_CACHE_OPT,
		LAZYWRITE_OPT
	};

	char const * const mount_option_names[] = {
			"remoteoptions",
			"rw",
			"ro",
			"backing",
			"listen",
			"shareid",
			"allowother",
			"nounmount",
			"fsc",
			"lazywrite",
			NULL
	};

	char * subopts;
	char * value;
	string remote_options;

	while((nextopt =
			getopt_long(argc, argv, short_options, long_options, NULL)) != -1){
		switch(nextopt) {
		case 'r': // remote mount point
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
		case 'o': // ofs options
			// TODO: this method is too long re-factor sub options parsing into private method
			subopts = optarg;
			int position;

			while (*subopts != '\0') {
				switch (getsubopt(&subopts, const_cast<char* const*>(mount_option_names), &value)) {
				case REMOTE_OPTIONS_OPT:
					// TODO: check if value == NULL
					// this would be so much nicer with boost::regex
					remote_options = string(value);
					position = remote_options.find(':');
					while (position != string::npos) {
						remote_options.replace(position, 1, ",");
						position = remote_options.find(':', position + 1);
					}
					lMountOptions = remote_options.c_str();
					break;
				case READ_WRITE_OPT:
					if (lReadOnly) throw OFSException("Cannot set ro and rw", 1, true);
					lReadWrite = true;
					break;
				case READ_ONLY_OPT:
					if (lReadWrite) throw OFSException("Cannot set ro and rw", 1, true);
					lReadOnly = true;
					break;
				case BACKING_OPT:
					// TODO: check if value == NULL;
					lCachePath = value;
					break;
				case LISTEN_OPT:
					// TODO: check if value == NULL
					char *saveptr;
					char *strdevice;
					strdevice = strtok_r(value, ":", &saveptr);
					lListenDevices.clear();
					while(strdevice != NULL) {
						lListenDevices.push_back(strdevice);
						strdevice = strtok_r(NULL, ":", &saveptr);
					}
					break;
				case SHARE_ID_OPT:
					// TODO: check if value == NULL
					lShareID = value;
					break;
				case ALLOWOTHER_OPT:
					lAllowOther = true;
					break;
				case NOUNMOUNT_OPT:
					lUnmount = false;
				case FS_CACHE_OPT:
					lUseFSCache = true;
					break;
				case LAZYWRITE_OPT:
					// TODO: check if value == NULL
					env.lazywrite = true;
					env.lwoption = value[0];
				default:
					throw OFSException(
							(string("Undefined parameter or error while parsing command line: ") + subopts).c_str(),
							1,true);
				}
			}
		case 'p': // mount options
			char *mntopt;
			mntopt = optarg;
			lMountOptions = mntopt;
			break;
		case 't': // allow other users access to the file system
			lAllowOther = true;
			break;
		case 'n': // do not unmount remote share after exit
			lUnmount = false;
			break;
		case 'h': // display help
			throw OFSException("User wants help",1);
		case 'z': // TODO: Perhaps need another parameter here (timeout, ?)
			env.lazywrite = true;
			env.lwoption = optarg[0];
			break;
		case 'V': /* -V or --version */
#if HAVE_CONFIG_H
			cout << argv[0] << " (" << PACKAGE_NAME << " version " << PACKAGE_VERSION << ")" << endl;
			exit(EXIT_SUCCESS);
			break;
#endif /* HAVE_CONFIG_H */
		case '?': // invalid parameter
			throw OFSException("Unknown parameter: "+optopt,1);
		case ':': // missing argument
			throw OFSException(
					"Missing argument for parameter: "+optopt,1);
		default: // unknown behavior
			throw OFSException(
					"Undefined parameter or error while parsing command line",
					1,true);
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
				}

list<string> OFSEnvironment::getListenDevices()
{
	return listendevices;
}

string OFSEnvironment::getUsageString(string executable)
{
	stringstream usage;
	usage << "Usage: "+executable+" <remotetargeturi> <dir> [<options>]"
			<< endl;
	usage << "Options are:" << endl;
	usage << "\t-V --version\tPrint version" << endl;
	usage << "\t-r --remote     URI to remote mount point"
			<< endl;
	usage << "\t-b --backing    Absolute path to the backing (cache) root"
			<< endl;
	usage << "\t-l --listen     Comma separated list of network interfaces to "
			<< "listen for plug/unplug events" << endl;
	usage << "\t-i --shareid    Internal unique share identifier" << endl;
	usage << "\t-t --allowother Allow all users access to the filesystem" << endl;
	usage << "\t-p --remoteoptions    Mount options as given to the mount command via -o" << endl;
	usage << "\t-n --nounmount  Do not unmount the remote share on exit" << endl;
	usage << "\t-h --help       Print this screen and exit" << endl;
	return usage.str();
}


