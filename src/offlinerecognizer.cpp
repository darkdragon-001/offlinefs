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
#include "offlinerecognizer.h"
#include "filesystemstatusmanager.h"
#include "ofslog.h"
#include <stdlib.h>
#include <unistd.h>

OfflineRecognizer::OfflineRecognizer(string strconninfo)
{
	int len, protlen;
	char *conninfo = new char[strconninfo.length()];
	strncpy(conninfo, strconninfo.c_str(), strconninfo.length());

	if (conninfo) {
		len = strlen(conninfo);
		m_host = strstr(conninfo,"://"); //substitute '://*'
		protlen = len - strlen(m_host);
	
		m_prot = new char[protlen+1];
		memcpy(m_prot, conninfo, protlen);
		m_prot[protlen] = '\0';
	
		if (m_host) {
			m_host = strstr(conninfo,"@");
			if (m_host)
				m_host +=1;
			else {
				m_host = strstr(conninfo,"://");
				m_host +=3;
			}
			char* path = strstr(m_host, ":"); //substitute path with ':' (sshfs)
			if (path) {
				path[0] = '\0';
			} else {
				path = strstr(m_host, "/"); //substitute path with '/' 
				if (path) {
					path[0] = '\0';
				}
			}
	
		} else {
			m_host = NULL;
		}
	} else {
		m_host = NULL;
		m_prot = NULL;
	}
}


OfflineRecognizer::~OfflineRecognizer()
{
	delete m_host;
	delete m_prot;
}


/* 
 * Provide hardcoded protocol information
 */
struct prot_info OfflineRecognizer::getProtInfoFor(/*char* protocol*/) {
	prot_info retVal;
	char* protocol = m_prot;

	/* Default port value */
	retVal.port = 0;

	if (strcmp("ssh", protocol) == 0 || strcmp("sshfs", protocol) == 0 || strcmp("sshd", protocol) == 0) {
		retVal.type = SOCK_STREAM;
		retVal.port = htons(22);
	}
	else if (strcmp("smb", protocol) == 0 || strcmp("smbfs", protocol) == 0 || strcmp("cif", protocol) == 0 || strcmp("cifs", protocol) == 0) {
		retVal.type = SOCK_STREAM;
		retVal.port = htons(445);
	}
	else if (strcmp("netfs", protocol) == 0 || strcmp("nfs", protocol) == 0 || strcmp("nfsd", protocol) == 0) {
		retVal.type = SOCK_DGRAM;
		retVal.port = htons(111);
		/* TODO: nfsv4 => type=tcp, port=2049 */
	}
	else if (strcmp("afs", protocol) == 0 || strcmp("afsd", protocol) == 0) {
		retVal.type = SOCK_DGRAM;
		retVal.port = htons(7000);
	}

	return retVal;
}

/** 
 * process to check the connection to a remote server
 * Input variables:
 * char* in: route to server: <protocol>://<ip or dns>
 * Returns:	CONN_ERR 	on connection failure
 *		DNS_ERR		on dns lookup failure
 */
int OfflineRecognizer::checkConnection(/*char* in*/) {
	char* protocol;//[10];
	char* host;
	int len, end, type, sock;
	prot_info protInfo;
	struct servent* servent;
	struct in_addr addr;
	struct hostent* hostent;
	struct sockaddr_in sockAddr;
	
	if (!m_prot)
		return MALFORMED;

	/*len = strlen (in);
	host = strstr(in,"://");*/

	if (!m_host)
		return MALFORMED;

	/* Get length of protocol string */
	//end = len - strlen(host);
	
	/* Copy protocolname on second array */
	/*for (int i=0; i < len ;i++) {
		if( i == end ) {
			protocol[i] = '\0';
			break;
		}
		protocol[i] = in[i];
	}*/
	protocol = m_prot;
	host = m_host;

	/* Try to retrieve protocol information by getservbyname() */
	servent = getservbyname( protocol,""); // on ubuntu, this fails...
	if (servent) { 
		/* On success, fill struct protInfo */
		if(strcmp(servent->s_proto,"udp") == 0)
			protInfo.type = SOCK_DGRAM; //udp
		else
			protInfo.type = SOCK_STREAM; //tcp
		protInfo.port = servent->s_port;
	} else {	
		/* Try to retrieve protocol information by hardcoded getProtInfoFor() */
		protInfo = getProtInfoFor(/*protocol*/);
		if (protInfo.port == 0)
			return UNKNOWN_PROT;
	}
	//std::cout << "protocol: " << protocol << "\ntype: " << protInfo.type << "\nport: " << ntohs(protInfo.port) << std::endl;
		
	/* Cut :// form server string */
	//host += 3;

	/* Get server address */
	hostent = gethostbyname(host);
	if (!hostent)
		/* DNS lookup failed */ 
		return DNS_ERR;
	addr = *(struct in_addr*) hostent->h_addr;

	/* Build socket with correct protocol */
	sock = socket(AF_INET, protInfo.type, 0);
	if(sock < 0)
		return SOCK_ERR;
	
	/* Prepare connection info */
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = protInfo.port;
	sockAddr.sin_addr = addr;
	
	/* Test connection */
    	if (connect(sock, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0) {
		//std::cout << "port: " << ntohs(sockAddr.sin_port) << "\naddr: " << sockAddr.sin_addr.s_addr << std::endl;
		if (sockAddr.sin_port == htons(445) && protInfo.type == SOCK_STREAM) { //dirty: try old samba port
			sockAddr.sin_port = htons(139);
			if (connect(sock, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0) {
				return CONN_ERR;
			} 
		} else {
			return CONN_ERR;
		}
    	} 
	
	return SUCCESS;
}

void OfflineRecognizer::startRecognizer() {
	ofslog::info("OfflineRecognizer started");
	int result, msec;
	bool isAvailable;
	msec = 1000000;
    	while (true) {
		result = checkConnection();	
		// if the protocol is unknown, we cannot determine if the server
		// is available, wherefore we asume availability
		if(result == UNKNOWN_PROT)
			result = SUCCESS;

		// online-offline toggle
		isAvailable = FilesystemStatusManager::Instance().isAvailable();
		if (result != SUCCESS && isAvailable) {
			if (msec < 5000000)
				msec = msec + 1000000;
			else {
				ofslog::warning("OfflineRecognizer failed to connect to server, setting mountpoint unavailable!");
				ofslog::info("Lazy write disabled");
				//disconnect
				FilesystemStatusManager::Instance().setAvailability(false);
				msec = 1000000;
			}
		} else if (result == SUCCESS && !isAvailable) {
			if (checkConnection() == SUCCESS) {//try again, just to be sure
				ofslog::info("OfflineRecognizer triggering reconnection to server");
				//connect
				FilesystemStatusManager::Instance().setAvailability(true);
				//pthread LW wieder aktivieren
			}
			msec = 1000000;
		}
		usleep(msec);
	}
}
