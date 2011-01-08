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
	ofslog::info("Write back Changes");
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
	ofslog::info("Lazy write activated");
	int sec = 300;
	//Catch Signal for Reintegration on Shutdown
	struct sigaction new_action;

	/* Set up the structure to specify the new action. */
	new_action.sa_handler = Sighandler;
	sigemptyset (&new_action.sa_mask);
	new_action.sa_flags = 0;
	sigaction (SIGHUP, &new_action, NULL);
	sigaction (SIGTERM, &new_action, NULL);


	//Reintegration alle 5 Minuten
	while (true) {
			if (FilesystemStatusManager::Instance().islazywrite()) {
				ofslog::info("Start Write back");
				SynchronizationManager::Instance().ReintegrateAll(
				                OFSEnvironment::Instance().getShareID().c_str());

			}
			else {
				ofslog::info("Lazy write disabled");
				//return;
			}
			sleep(sec);
		}
}
