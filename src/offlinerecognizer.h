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
#ifndef OFFLEINRECOGNIZE_H
#define OFFLEINRECOGNIZE_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

/*
 * Protocol information struct
 */
struct prot_info {
        int type;
        int port;
};

const int SUCCESS = 0;
const int CONN_ERR = -1;
const int DNS_ERR = -2;
const int SOCK_ERR = -4;
const int UNKNOWN_PROT = -8;
const int MALFORMED = -16;

/**
	@author 
*/
class OfflineRecognizer{
public:
    OfflineRecognizer(char* conninfo);

    ~OfflineRecognizer();

	private:
		char* m_host;
		char* m_prot;
		struct prot_info getProtInfoFor(/*char* protocol*/);
		int checkConnection(/*char* in*/);
        public:
		void startRecognizer();
};

#endif
