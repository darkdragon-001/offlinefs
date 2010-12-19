/***************************************************************************
 *   Copyright (C) 2010 by                                                 *
 *                 Frank Gsellmann, Tobias Jaehnel, Carsten Kolassa,	   *
 *                 Frank Jurak										       *
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

#include "lazywrite.h"
#include "ofslog.h"
#include "filesystemstatusmanager.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
/* diese funktion ist für das anstoßen der reintegration zuständig
   dafür muss sie permanent laufen oder zumindest im Netzbetrieb
   muss die im TODO beschrieben Ereignisse behandeln

   IDEE: Thread wird im Offline Modus suspended bis das Signal für Netz kommt
*/
void Sighandler(int sig)
{
	if(FilesystemStatusManager::Instance().islazywrite())
	{
	ofslog::info("Veränderte Daten werden auf das Netzlaufwerk geschrieben");
	SynchronizationManager::Instance().ReintegrateAll(OFSEnvironment::Instance().getShareID().c_str());
	FilesystemStatusManager::Instance().setsync(true);
	}
	return;
}
Lazywrite::Lazywrite(int i)
{

}

Lazywrite::~Lazywrite()
{

}

void Lazywrite::startLazywrite()
{
	ofslog::info("Layzwrite gestartet");
	int sec = 300;
	//Catch Signal for Reintegration on Shutdown
	typedef void (*sighandler_t)(int);
	signal(SIGTERM, &Sighandler);

	//Reintegration alle 5 Minuten
	while (true) {
			if (FilesystemStatusManager::Instance().islazywrite()) {
				ofslog::info("Start Reintegration");
				SynchronizationManager::Instance().ReintegrateAll(
				                OFSEnvironment::Instance().getShareID().c_str());

			}
			else {
				ofslog::info("Lazywrite deaktiviert");
				//return;
			}
			sleep(sec);
		}
}
