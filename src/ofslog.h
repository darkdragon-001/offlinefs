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
#ifndef OFSLOG_H
#define OFSLOG_H

/**
	@author Matthias Petri <Matthias.Petri@gmail.com>
*/
class ofslog {
public:
    /**
     * initialise the logging infrastructure
     * @return logging system successfully initialised?
     */
    static bool init();
    /**
     * info log msg
     * @param log msg
     */    
    static void info(const char *fmt, ...);
    /**
     * debug log msg
     * @param log msg
     */    
    static void debug(const char *fmt, ...);
    /**
     * error log msg
     * @param log msg
     */    
    static void error(const char *fmt, ...);
    /**
     * warning log msg
     * @param log msg
     */    
    static void warning(const char *fmt, ...);
    /**
     * notice log msg
     * @param log msg
     */    
    static void notice(const char *fmt, ...);
    /**
     * critical log msg
     * @param log msg
     */    
    static void critical(const char *fmt, ...);
    /**
     * actual log function
     * @param log msg
     */    
    static void log(int loglvl,const char *fmt,va_list ap);
};

#endif

