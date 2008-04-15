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

//using namespace std;

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
    char* pArgumente[3];
    pArgumente[0] = new char[MAX_PATH + 1];
    pArgumente[1] = new char[MAX_PATH + 1];

    // Ermittelt die Backing-Tree- und Remote-Pfade.
    // ZU ERLEDIGEN: Muß noch verhasht werden.
    strncpy(pArgumente[0], conf.GetBackingTreePath().c_str(), MAX_PATH);
    strncpy(pArgumente[1], conf.GetRemotePath().c_str(), MAX_PATH);

    pArgumente[2] = szOptions;

    // Ruft das Offline-Filesystem auf.
    return execv("ofs", pArgumente);
}
