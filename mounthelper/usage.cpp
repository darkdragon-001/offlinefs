/***************************************************************************
 *   Copyright (C) 2011 by Peter Trommler                                  *
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

#include <iostream>
#include <stdlib.h>

using namespace std;

void print_usage (std::ostream & stream, int exit_code)
{
	stream << "Usage: mount.ofs remotetarget dir -hV -o remotefsoptions" << endl;
	stream << "\t-o --option\tOptions for remote file system mount" << endl;
	stream << "\t-h --help\tPrint this information." << endl;
	stream << "\t-V --version\tPrint version" << endl;

	exit (exit_code);
};
