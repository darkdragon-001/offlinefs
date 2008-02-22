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
#include "ofsexception.h"

OFSException::OFSException(string message, int posixerrno)
{
	this->message = message;
	this->posixerrno = posixerrno;
}
OFSException::OFSException(const OFSException &e)
{
	message = e.message;
	posixerrno = e.posixerrno;
}

OFSException::~OFSException() throw()
{
}

/*!
    \fn OFSException::operator=(OFSException &)
 */
OFSException & OFSException::operator=(OFSException &e)
{
	this->message = e.message;
	this->posixerrno = e.posixerrno;
	return *this;
}


/*!
    \fn OFSException::what() const throw()
 */
const char * OFSException::what() const throw()
{
	return message.c_str();
}
