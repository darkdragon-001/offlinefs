/***************************************************************************
 *   Copyright (C) 2007 by ,,,   *
 *   xxx@blacktron2   *
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

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
void print_usage (FILE* stream, int exit_code)
{
fprintf(stream, "Usage: auto_test options\n");
fprintf(stream, "-m --max Maximal Packet size in the test\n");
fprintf(stream, "-i --min Minimal Packet size in the test\n");
fprintf(stream, "-r --repetitions Repetitions of every Packet size per at once\n");
fprintf(stream, "-o --overalrep overal repetitions \n");
fprintf(stream, "-f --footstep Footstep size \n");
fprintf(stream, "-s --sleep sleep between packetsizes\n");
fprintf(stream, "-h --help Display this usage information.\n");

exit (exit_code);
};
