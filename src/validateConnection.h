/*
 *  validateConnection.h
 *  ValidateConnection
 *
 *  Created by MW on 13.12.09.
 */
int timed_connect(int socket, sockaddr_in sockAddr, struct timeval *timeout);
int checkConnection(char* in);

const int TIMEOUT=5;

const int ERR = -1;
const int SUCCESS = 0;
const int SOCKERR = -2;
const int TIMEOUTERR = -4;

