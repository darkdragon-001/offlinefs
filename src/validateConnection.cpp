#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include <iostream>
#include <fcntl.h>
#include <netdb.h>
#include <validateConnection.h>

/** 
 * process to check the connection to a remote server
 * Input variables:
 * char* in: route to server: <protocol>://<ip or dns>
 */
int checkConnection(char* in) {
	char protocol[10];
	int len = strlen (in);
	char* host = strstr(in,"://");
	//get length of protocol
	int end = len - strlen(host);
	
	//copy protocolname on second array
	for (int i=0; i < len ;i++) {
		if( i == end ) {
			protocol[i] = NULL;
			break;
		}
		protocol[i] = in[i];
	}
	//generate protocol information
	struct servent* servent = getservbyname( protocol,"");
	//cut :// form server ip
	host += 3;
	//get server adresse
	struct in_addr addr;
	in_addr_t t = inet_addr(host);
	if ( t == -1) {
		struct hostent* hostent = gethostbyname(host);
		addr = *(struct in_addr*) hostent->h_addr;
	} else {
		addr.s_addr = t;
	}
	//build socket with correct protocol
	int type = SOCK_STREAM;
	if(strcmp(servent->s_proto,"udp") == 0)
		type = SOCK_DGRAM;
	int sock = socket(AF_INET, type, 0);
	if(sock < 1)
		return SOCKERR;
	
	//prepare connection
	struct sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = servent->s_port;
	sockAddr.sin_addr = addr;
	
	struct timeval timeout;
	timeout.tv_sec = TIMEOUT;

	return timed_connect(sock, sockAddr, &timeout);
}

/**
 * open a connection to a socket with specified Timeout
 * socket : the socked id to connect
 * sockAddr : the remote address to connect to
 * timeout : the timeout
 */
int timed_connect(int socket, sockaddr_in sockAddr, struct timeval *timeout) {
    int res, opt;

    if ((opt = fcntl(socket, F_GETFL, NULL)) < 0)
        return ERR;

    if (fcntl(socket, F_SETFL, opt | O_NONBLOCK) < 0)
        return ERR;

    if ((res = connect(socket, (sockaddr*)&sockAddr, sizeof(sockAddr))) < 0) {
        if (errno == EINPROGRESS) {
            fd_set wait_set;

            FD_ZERO(&wait_set);
            FD_SET(socket, &wait_set);

            res = select(socket + 1, NULL, &wait_set, NULL, timeout);
        }
    } else {
        res = 1;
    }

    if (fcntl(socket, F_SETFL, opt) < 0)
        return ERR;

	if (res < 0) {
        return ERR;
    } else if (res == 0) {
        errno = ETIMEDOUT;
        return TIMEOUTERR;
    } else {
        socklen_t len = sizeof(opt);

        if (getsockopt(socket, SOL_SOCKET, SO_ERROR, &opt, &len) < 0)
            return ERR;

        if (opt) {
            errno = opt;
            return ERR;
        }
    }

    return SUCCESS;
}
