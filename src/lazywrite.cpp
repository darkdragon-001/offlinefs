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
#include <stdio.h>
/* This Method control the Reintegration if Lazywrite active
 * on Shutdown or Log-off the reintegration will start in fuse_destroy
 *
*/
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
	bool tosync=false;  //For Sync ever x Seconds set it to 'true'!!

	//Variable for load CPU
	FILE *fpipe;
	char *command ="grep -o \'[0-9]\\+\\.[0-9]\\+*\' /proc/loadavg | sed -n \"3p\"";
	char line[10];
	int is, max = 5;

	//Variable for Load Network
/*	FILE *fpipe;
	char *command ="grep eth0 /proc/net/dev | awk -F: \'{print  $2}\' | awk \'{print $9}\'";
	char line[255];
	long int is=0, old = 0;
	long int diff=0;
*/
	while (true) {
		if (!(FilesystemStatusManager::Instance().issync())) {

			// re-integration per Load CPU
			fpipe=(FILE*)popen(command,"r");
			fgets(line, sizeof line, fpipe);
			is=atol(line);
			if(is<max)
			{
				tosync=true;
			}

			//re-integration per Load Network
			//get the Different for the next 5 Minutes of the Transmitted Byte from /proc/net/dev
/*			for(int x=0;x<30;x++)
			{
			fpipe=(FILE*)popen(command,"r");
			fgets(line, sizeof line, fpipe);
			is=(atol(line))/1024; //byte->Kbyte
			if(is>old && old!=0)
			{
			  diff = diff + (is - old);
			}
			old = is;
			sleep(10);
			}
			diff=diff/1024; //Kbyte->Mbyte
			if(diff<10) //if the different smaller then 10 Mbyte
			{
				tosync=true;
			}
*/
			if(FilesystemStatusManager::Instance().islazywrite() && tosync){
			ofslog::info("Start Write back");
			SynchronizationManager::Instance().ReintegrateAll(OFSEnvironment::Instance().getShareID().c_str());
			FilesystemStatusManager::Instance().setsync(true);
			}
		}
		else {
			ofslog::info("No Changes");
			}
		sleep(sec);
		}
}
