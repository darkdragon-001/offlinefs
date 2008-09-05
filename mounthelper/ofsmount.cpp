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
#include <ofsconf.h>
#include "options.h"
#include <assert.h>
#include <ofshash.h>
#include <iostream>
using namespace std;

#define MAX_PATH 1024

int main(int argc, char *argv[])
{
    // Ermittelt die Optionen aus der Kommandozeile.
    char szOptions[10];
    char* pszOptions = szOptions;
//    my_options(argc - 2, &argv[2], &pszOptions);
    my_options(argc, argv, &pszOptions);

    // Oeffnet die Konfigurationsdatei.
    OFSConf& conf = OFSConf::Instance();
    conf.ParseFile();

    cout << szOptions << endl;

    //////////////////////////////////////////////////////////////////////////
    // MOUNT
    //////////////////////////////////////////////////////////////////////////

    char* pMountArgumente[5];
    char szDateisystem[] = "-t";
    char szMount[] = "mount";
    pMountArgumente[0] = szMount;
    pMountArgumente[1] = szDateisystem;
    pMountArgumente[2] = new char[MAX_PATH + 1];
    pMountArgumente[3] = new char[MAX_PATH + 1];
    pMountArgumente[4] = new char[MAX_PATH + 1];

    cout << "Achtung!!!" << endl;
    cout << "0.) " << argv[0] << endl;
    cout << "1.) " << argv[1] << endl;
//    cout << "2.) " << argv[2] << endl;
//    cout << "3.) " << argv[3] << endl;
//    cout << "4.) " << argv[4] << endl;
    char ch708;
//    cin >> ch708;

    // Ermittelt die Backing-Tree- und Remote-Pfade.
    // ZU ERLEDIGEN: Muß noch verhasht werden.
    cout << argv[0] << endl;
    char* pchDoppelPunktPos = strchr(argv[1], ':');
//    assert(pchDoppelPunktPos != NULL);
    int nDoppelPunktIndex = int (pchDoppelPunktPos - argv[1]);

    cout << nDoppelPunktIndex << endl;
//    cin >> ch708;

    // Legt das Dateisystem fest.
    strncpy(pMountArgumente[2], argv[1], nDoppelPunktIndex);

    // Legt die Server-Share fest.
    strncpy(pMountArgumente[3], &((argv[1])[nDoppelPunktIndex + 3]), MAX_PATH);
    strncpy(pMountArgumente[4], conf.GetRemotePath().c_str(), MAX_PATH);

//    pArgumente[2] = szOptions;

    string strHash = ofs_hash(pMountArgumente[2]);
    strcat(pMountArgumente[3], "/");
    strcat(pMountArgumente[3], strHash.c_str());

    cout << pMountArgumente[0] << endl;
    cout << pMountArgumente[1] << endl;
    cout << pMountArgumente[2] << endl;
    cout << pMountArgumente[3] << endl;
    cout << pMountArgumente[4] << endl;

    // Mountet die Share, die vom Benutzer übergeben wurde.
//    return execv("mount", pMountArgumente);
    int nErg = execv("s", pMountArgumente);
    cout << nErg << endl;
    cin >> ch708;

    return nErg;

    //////////////////////////////////////////////////////////////////////////
    // OFS
    //////////////////////////////////////////////////////////////////////////

    char* pArgumente[3];
    pArgumente[0] = new char[MAX_PATH + 1];
    pArgumente[1] = new char[MAX_PATH + 1];

    // Ermittelt die Backing-Tree- und Remote-Pfade.
    // ZU ERLEDIGEN: Muß noch verhasht werden.
    strncpy(pArgumente[0], conf.GetBackingTreePath().c_str(), MAX_PATH);
    strncpy(pArgumente[1], conf.GetRemotePath().c_str(), MAX_PATH);

    pArgumente[2] = szOptions;

    cout << pArgumente[0] << endl;
    cout << pArgumente[1] << endl;

    // Ruft das Offline-Dateisystem auf.
    return execv("ofs", pArgumente);
}

// "ext3://ServerVerz /LokalVerz -o rw"
