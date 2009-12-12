/***************************************************************************
 *   Copyright (C) 2008 by Frank Gsellmann,,,   *
 *   frank@frank-laptop   *
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

//#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <ofsconf.h>
#include "options.h"
#include <assert.h>
#include <ofshash.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <cstring>
using namespace std;

#define MAX_PATH 1024

/**
 * @TODO: This is very very unclean
 * @param argc 
 * @param argv[] 
 * @return 
 */
int main(int argc, char *argv[])
{
    assert(argc > 2);
    // Ermittelt die Optionen aus der Kommandozeile.
    char szOptions[10];
    char* pszOptions = szOptions;
    string shareurl = argv[1];
    string sharepath;
    string remotefstype;
    string shareremote;
    string remotemountpoint;
    string ofsmountpoint = argv[2];
    //////////////////////////////////////////////////////////////////////////
    // MOUNT
    //////////////////////////////////////////////////////////////////////////

    char* pMountArgumente[8];
    pMountArgumente[0] = "mount";
    pMountArgumente[1] = "-t";
    pMountArgumente[2] = NULL;
    pMountArgumente[3] = NULL;
    pMountArgumente[4] = NULL;
    pMountArgumente[5] = "-o";
    pMountArgumente[6] = NULL;
    pMountArgumente[7] = NULL; // terminator

    // Ermittelt die Remote-Pfade.
    //cout << argv[0] << endl;
    char* pchDoppelPunktPos = strchr(argv[1], ':');
    assert(pchDoppelPunktPos != NULL);
    int nDoppelPunktIndex = int (pchDoppelPunktPos - argv[1]);

    remotefstype = shareurl.substr(0,nDoppelPunktIndex);
    sharepath = shareurl.substr(nDoppelPunktIndex+3);
    // handle special protocols
    if(remotefstype == "smb" || 
       remotefstype == "smbfs" || 
       remotefstype == "cifs")
        shareremote = string("//") + sharepath;
    else if(remotefstype == "file") {
	shareremote = string("/") + sharepath;
	remotemountpoint = shareremote;
    } else
        shareremote = sharepath;

    char* pArgumente[8];

    if(remotefstype != "file") {
    	// Legt das Dateisystem fest.
    	pMountArgumente[2] = (char*)remotefstype.c_str();

    	// Oeffnet die Konfigurationsdatei.
    	OFSConf& conf = OFSConf::Instance();
    	//conf.ParseFile(); //obsolete

    	// Legt die Server-Share fest.
    	pMountArgumente[3] = (char*)shareremote.c_str();
    	remotemountpoint = conf.GetRemotePath()+"/"+ofs_hash(shareurl);
    	pMountArgumente[4] = (char*)remotemountpoint.c_str();

    	//    my_options(argc - 2, &argv[2], &pszOptions);
    	my_options(argc, argv, &pszOptions);
    	pMountArgumente[6] = pszOptions;
	//    pArgumente[2] = szOptions;

    
    	// Mountet die Share, die vom Benutzer übergeben wurde.
    	int childpid = fork();
    	int status;
    	if(childpid == 0) {
    		/*cout << endl << "mount" << " ";
		cout << pMountArgumente[1] << " ";
    		cout << pMountArgumente[2] << " ";
    		cout << pMountArgumente[3] << " ";
    		cout << pMountArgumente[4] << " ";
    		cout << pMountArgumente[5] << " ";
    		cout << pMountArgumente[6] << " ";
    		cout << endl;*/
    		// make the mount point and ignore errors of it does exits
       		mkdir(pMountArgumente[4], 0777);
       		execvp("mount", pMountArgumente);
       		perror(strerror(errno));
       		return -errno;
    	}
    	if(childpid < 0) {
        	perror(strerror(errno));
        	return -errno;
    	}
    	//waitpid(childpid, &status, 0);
    	int childpid2 = wait(&status);
	//    cout << status << " - (" << childpid << "/" << childpid2 << ") - " << WEXITSTATUS(status) << endl;
    	int exitstatus = WEXITSTATUS(status);
    	if(WIFEXITED(status) && exitstatus) {
       		errno = exitstatus;
       		perror(strerror(errno));
       		exit(exitstatus);
    	}
    
    	//////////////////////////////////////////////////////////////////////////
    	// OFS
    	//////////////////////////////////////////////////////////////////////////


    	pArgumente[0] = "ofs";
    	pArgumente[1] = (char *)ofsmountpoint.c_str();
    	pArgumente[2] = (char *)shareurl.c_str();
    	pArgumente[3] = "-o"; // allow all user access to filesystem

	pArgumente[4] = "-p"; // mount options
	pArgumente[5] = pszOptions;
	pArgumente[6] = NULL; // terminator
    } else {
        pArgumente[0] = "ofs";
        pArgumente[1] = (char *)ofsmountpoint.c_str();
        pArgumente[2] = (char *)shareurl.c_str();
        pArgumente[3] = "-r";
        pArgumente[4] = (char *)sharepath.c_str();
        pArgumente[5] = "-n";
        if(geteuid() == 0) {
            pArgumente[6] = "-o";
            pArgumente[7] = NULL;
        } else
            pArgumente[6] = NULL;
    }
//    cout << pArgumente[1] << endl;
//    cout << pArgumente[2] << endl;

    // Ruft das Offline-Dateisystem auf.
    execvp("/sbin/ofs", pArgumente);    
    perror(strerror(errno));

    return -errno;
    //return 0;
}
