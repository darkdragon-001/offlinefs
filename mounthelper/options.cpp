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

#include "printusage.h"
#include <string.h>

int my_options(int argc, char* argv[], char** ppszOptions)
{
    int next_option;
    const char* const short_options = "ho:V";
    const struct option long_options[]=
    {
        {"help",0,NULL,'h'},
        {"options",1,NULL,'o'},
        {"version",0,NULL,'V'},
        {NULL,0,NULL,0} /* Required at end of array. */
    };
    do
    {
        next_option = getopt_long (argc, argv, short_options, long_options, NULL);
        switch (next_option)
        {
        case 'h': /* -h or --help */
            /* User has requested usage information. Print it to standard
            output, and exit with exit code zero (normal termination). */
            print_usage (stdout, 0);
        case 'o': /* -o or --options */
            optarg;
            strncpy(*ppszOptions, optarg, strlen(optarg));
            break;
        case 'V': /* -V or --version */
            /* User has requested version information. Print it to standard
            output, and exit with exit code zero (normal termination). */
//            cout << "version" << endl;
        case '?': /* The user specified an invalid option. */
            /* Print usage information to standard error, and exit with exit
            code one (indicating abnormal termination). */
            print_usage (stderr, 1);
        case -1: /* Done with options. */
            break;
        default: /* Something else: unexpected. */
            abort ();
        }
    }
    while (next_option != -1);
    return 0;
}
