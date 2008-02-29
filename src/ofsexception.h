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
#ifndef OFSEXCEPTION_H
#define OFSEXCEPTION_H
#include <exception>
#include <string>
using namespace std;

/**
	@author 
*/
class OFSException : public exception {
public:
    OFSException(string message, int posixerrno);

    OFSException(const OFSException &e);
    ~OFSException() throw();
    OFSException & operator=(OFSException &);
    virtual const char * what() const throw();
    int get_posixerrno();
private:
	string message;
	int posixerrno;
};

#endif
